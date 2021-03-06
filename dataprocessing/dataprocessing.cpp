#include <emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <assert.h> 


extern "C" void queue_push (char *test);

extern "C" char get_index_0();

extern "C" char get_index_1();

extern "C" char get_index_2();

extern "C" char get_index_3();

class risingedgecount {
	public:
		// bits 0-4: TMC count whatever that means
		// Units? Who knows
		// Endianness?
		unsigned char tmc;
		// bit 5: Is it a rising edge?
		bool re;
		// bit 6: Its reserved, OR ELSE
		bool reserved;
		// bit 7: Is it a new event?
		bool event_new;
		// Initialize based on hex string
		void set(std::string hex);
};

class fallingedgecount {
	public:
		// bits 0-4: TMC count whatever that means
		// Units? Who knows
		// Endianness?
		unsigned char tmc;
		// bit 5: is it a rising edge
		bool fe;
		// Initialize based on hex string
		void set(std::string hex);
};

class daqstatus {
	public:
		// Endianness?

		// bit 0: is the PPS count up to date
		bool ppsc;

		// bit 1: is the trigger (what's this?)
		bool trig_interrrupt;

		// bit 2: is the GPS data corrupted
		bool gps_corrupt;

		// bit 3: I have to quote here
		// "Current or last 1PPS rate is not within 41666666 ±50 CPLD 
		// clock tick.s (This is a result of a GPS glitch, the DAQ uC 
		// being busy, or the CPLD oscillator not tuned correctly.)"
		bool cpld_pps_mismatch;

		// Intialize based on hex string
		void set(std::string hex);
};

class daq_time {
	public:
		// time
		struct tm time;
		// tenthousandths of a second past the second mark
		unsigned short tenthousandths;
		// 24 ns resolution ticks
		unsigned long ticks;
		// add clock skew
		void skew(short skew);
		// create things
		// why not daq_time::daq_time? I should fix this later but I was too lazy to refactor the message class.
		void init(tm time, short gps_ms, unsigned long tick, unsigned long gps_tick);
};

class message {
	public:
		// Do we use little endian or big?  It would be nice to know!

		daq_time time;

		// In the serialized message the next two arrays are interleaved.
		// But why make things hard when we need not
		// Count of rising edge for inputs 0-3
		risingedgecount re[4];
		// Count of falling edge for inputs 0-3
		fallingedgecount fe[4];

		// Is the GPS data valid
		bool gps_valid;

		// Number of visible GPS satellites
		unsigned char sat_num;

		// DAQ status
		daqstatus daqstat;
};

class queue {
	private:
		message last;
		bool reads[4] = {false, false, false, false};
	public:
		std::vector<message> messages;
		void push(message m);
		message read(char line);
};

queue mq = queue(); // Should this still be a global?
// Leaving it a global reduces re-allocation

void risingedgecount::set(std::string hex) {
	unsigned long buffer = std::stoul(hex,nullptr,16);
	tmc = buffer & 31; // first 5 bits
	re = buffer & 32; // bit 5
	reserved = buffer & 64; // bit 6
	event_new = buffer & 128; // bit 7
}

void fallingedgecount::set(std::string hex) {
	unsigned long buffer = std::stoul(hex,nullptr,16);
	tmc = buffer & 31; // first 5 bits
	fe = buffer & 32; // bit 5
}

void daqstatus::set(std::string hex) {
	// Like the rising/falling bytes we encode this using bit packing to take up less space.
	// Then we make it take up twice as much space by hex encoding it.  (What?)
	int buffer = std::stoi (hex,nullptr,16);
	ppsc = buffer & 1; // bit 0
	trig_interrrupt = buffer & 2; // bit 1
	gps_corrupt = buffer & 4; // bit 2
	cpld_pps_mismatch = buffer & 8; // bit 3
}

void daq_time::skew(short skew) {
	// Offset the time of the event by the clock skew at the end of the message
	
	// Prevent offset > tenthousandths place from making clock go negative
	time.tm_sec--;
	tenthousandths += 10000;
	
	// Add skew
	tenthousandths += (skew * 10);
	
	// If tenthousandths place >= 1 sec move up to seconds
	while (tenthousandths >= 10000) {
		tenthousandths -= 10000;
		time.tm_sec++;
	}
}

void daq_time::init (tm time, short gps_ms, unsigned long tick, unsigned long gps_tick){
	tenthousandths = (gps_ms * 10);
	ticks = (tick - gps_tick) % (4294967295); // modulo subtract because this long overflows every 100 sec
}

void queue::push(message m) {
	messages.push_back(m);
}

message queue::read(char line){
	reads[line] = true;
	return messages[messages.size()];
	char sum = 0;
	for (char i = 0; i < 4; i++) {
		sum += reads[i];
	}
	assert(sum > 4);
	if (sum == 4) {
		messages.pop_back();
	}
}

message deserialize_string(std::string line) {
	message m; // final output, parsed message

	// Convert line to stringstream so we can iterate through it
	std::stringstream s;
	s << line;

	std::string component; //holds a component

	unsigned char iter = 0; // Iterator. What part of the message are we on?

	char gps_time[12]; // GPS time comes in two chunks so we have to combine them before processing
	char gps_ms_char[3]; // GPS time includes milliseconds time_t does not

	struct tm gps_update;
	unsigned long trigger = 0;
	unsigned long gps_cpld = 0;
	unsigned short gps_ms = 0;
	short skew = 0;

	while (std::getline(s, component, ' ')) { // Split by spaces and loop through all the components
		switch(iter) {
			case 0: // Get the clock value at trigger
				trigger = std::stoul(component, nullptr, 16);
				break;
			case 1: // Rising edge 0
				m.re[0].set(component);
				break;
			case 2: // Falling edge 0
				m.fe[0].set(component);
				break;
			case 3: // Rising edge 1
				m.re[1].set(component);
				break;
			case 4: // Falling edge 1
				m.fe[1].set(component);
				break;
			case 5: // Rising edge 2
				m.re[2].set(component);
				break;
			case 6: // Falling edge 2
				m.fe[2].set(component);
				break;
			case 7: // Rising edge 3
				m.re[3].set(component);
				break;
			case 8: // Falling edge 3
				m.fe[3].set(component);
				break;
			case 9:  // Collect clock at gps persecond pulse
				gps_cpld = std::stoul(component, nullptr, 16);
				break;
			case 10: // Collect HHMMSS + milliseconds part of GPS time
				for (int i = 0; i < 6; i++) { // save HHMMSS for later processing
					gps_time[i] = *(component.c_str() + i);
				}
				for (int i = 0; i < 3; i++) { // Save ms for later seperate processing
					gps_ms_char[i] = *(component.c_str() + i + 7);
				}
				break;
			case 11: // Collect ddmmyy part of GPS time
				for (int i = 0; i < 6; i++) { // Save for later
					gps_time[i+6] = *(component.c_str() + i);
				}
				gps_time[10] = *component.c_str();
				break;
			case 12: // Is the GPS data valid
				// Lets use a strange format! 'A' for valid and 'V' for invalid.  (I have questions.)
				m.gps_valid = ("A" == component);
				break;
			case 13: // Numer of visible GPS satellites
				m.sat_num = std::stoul(component, nullptr, 0);
				break;
			case 14: // Read the DAQ status flag (See: void daqstatus::set)
				m.daqstat.set(component);
				break;
			case 15: // Get the clock skew
				skew = std::stoi(component, nullptr, 0);
			default:
				break;
		}
		iter++;
	}
	strptime(gps_time, "%H%M%S%d%m%y", &gps_update); // Read GPS time
	gps_ms = std::stoul(gps_ms_char, nullptr, 0);  // Read GPS MS
	m.time.init(gps_update, gps_ms, trigger, gps_cpld); // Intialize time class
	m.time.skew(skew); // Skew class
	return m;
}

void queue_push (char *test) {
	std::stringstream s;
	s << test; // Initialize stream
	std::string line; //holds a line
	while (std::getline(s, line)) { //loop through all the lines
		if (line.length() > 72) { //why 72? a line is 72 bytes plus a \r\n
			mq.push(
				deserialize_string(
					line.substr(0, 71) //ditch the \r\n
				)
			);
		}
	}
}

char get_index_0() {
	if (!mq.messages.size()) {
		return -1;
	}
	message last = mq.read(0);
	if (last.re[0].re) {
		return 1;
	} else if (last.fe[0].fe) {
		return 0;
	}
}

char get_index_1() {
	if (!mq.messages.size()) {
		return -1;
	}
	message last = mq.read(1);
	if (last.re[1].re) {
		return 1;
	} else if (last.fe[1].fe) {
		return 0;
	}
}

char get_index_2() {
	if (!mq.messages.size()) {
		return -1;
	}
	message last = mq.read(2);
	if (last.re[2].re) {
		return 1;
	} else if (last.fe[2].fe) {
		return 0;
	}
}

char get_index_3() {
	if (!mq.messages.size()) {
		return -1;
	}
	message last = mq.read(3);
	if (last.re[3].re) {
		return 1;
	} else if (last.fe[3].fe) {
		return 0;
	}
}
