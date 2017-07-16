#include <emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>

#include <iostream>


extern "C" void queue_push (char *test);

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
		bool re;
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
		struct tm time;
		unsigned short tenthousandths;
		unsigned long ticks;
		void skew(short skew);
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

std::vector<message> queue; // Should this still be a global?
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
	re = buffer & 32; // bit 5
}

void daqstatus::set(std::string hex) {
	int buffer = std::stoi (hex,nullptr,16);
	ppsc = buffer & 1; // bit 0
	trig_interrrupt = buffer & 2; // bit 1
	gps_corrupt = buffer & 4; // bit 2
	cpld_pps_mismatch = buffer & 8; // bit 3
}

void daq_time::skew(short skew) {
	time.tm_sec--;
	tenthousandths += 10000;
	tenthousandths += (skew * 10);
	while (tenthousandths >= 10000) {
		tenthousandths -= 10000;
		time.tm_sec++;
	}
}

void daq_time::init (tm time, short gps_ms, unsigned long tick, unsigned long gps_tick){
	//time = time;
	tenthousandths = (gps_ms * 10);
	ticks = (tick - gps_tick) % (4294967295); // modulo subtract because this slong overflows every 100 sec
}

void queue_read(){
	
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

	while (std::getline(s, component, ' ')) { //loop through all the components
		switch(iter) {
			case 0:
				trigger = std::stoul(component, nullptr, 16);
				break;
			case 1:
				m.re[0].set(component);
				break;
			case 2:
				m.fe[0].set(component);
				break;
			case 3:
				m.re[1].set(component);
				break;
			case 4:
				m.fe[1].set(component);
				break;
			case 5:
				m.re[2].set(component);
				break;
			case 6:
				m.fe[2].set(component);
				break;
			case 7:
				m.re[3].set(component);
				break;
			case 8:
				m.fe[3].set(component);
				break;
			case 9:
				gps_cpld = std::stoul(component, nullptr, 16);
				break;
			case 10:
				for (int i = 0; i < 6; i++) {
					gps_time[i] = *(component.c_str() + i);
				}
				for (int i = 0; i < 3; i++) {
					gps_ms_char[i] = *(component.c_str() + i + 7);
				}
				break;
			case 11:
				for (int i = 0; i < 6; i++) {
					gps_time[i+6] = *(component.c_str() + i);
				}
				gps_time[10] = *component.c_str();
				break;
			case 12:
				m.gps_valid = ("A" == component);
				break;
			case 13:
				m.sat_num = std::stoul(component, nullptr, 0);
				break;
			case 14:
				m.daqstat.set(component);
				break;
			case 15:
				skew = std::stoi(component, nullptr, 0);
			default:
				break;
		}
		iter++;
	}
	strptime(gps_time, "%H%M%S%d%m%y", &gps_update);
	gps_ms = std::stoul(gps_ms_char, nullptr, 0);
	m.time.init(gps_update, gps_ms, trigger, gps_cpld);
	m.time.skew(skew);
	return m;
}

void queue_push (char *test) {
	std::stringstream s;
	s << test; // Initialize stream
	std::string line; //holds a line
	while (std::getline(s, line)) { //loop through all the lines
		if (line.length() > 72) { //why 72? a line is 72 bytes plus a \r\n
			queue.push_back(
				deserialize_string(
					line.substr(0, 71) //ditch the \r\n
				)
			);
		}
	}
	if (queue.size() > 0) {
		queue_read();
	}
}
