#include <emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>

#include <iostream>

std::vector<std::string> queue; //can't use char[72] so we use a string

extern "C" void queue_push (char *test); // Should this still be a global?
// Leaving it a global reduces re-allocation

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

class message {
	// Do we use little endian or big?  It would be nice to know!
	
	// 32 bit clock trigger count. Do not read data if value is 0
	// What does it mean? Only God knows
	// Endianness?
	unsigned long trigger;

	// In the serialized message the next two arrays are interleaved.
	// But why make things hard when we need not
	// Count of rising edge for inputs 0-3
	risingedgecount[4] re;
	// Count of falling edge for inputs 0-3
	fallingedgecount[4] fe;

	// CPLD count of time pulse from GPS
	// Endianness?
	unsigned long CPLD;

	// Time of the last GPS time update
	time_t gps_update;

	// Is the GPS data valid
	bool gps_valid;

	// Number of visible GPS satellites
	unsigned char sat_num;

	// DAQ status
	daqstatus daqstat;

	// GPS to PPS skew
	short skew;
};

void risingedgecount::set(std::string hex) {
	int buffer = std::stoi (hex,nullptr,16);
	tmc = buffer & 31; // first 5 bits
	re = buffer & 32; // bit 5
	reserved = buffer & 64; // bit 7
	event_new = buffer & 128; // bit 8
}

void fallingedgecount::set(std::string hex) {
	int buffer = std::stoi (hex,nullptr,16);
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

void queue_read(){
	
}

void deserialize_string(std::string hex) {
	
}

void queue_push (char *test) {
	std::stringstream s;
	s << test; //Create a stream
	std::string line; //holds a line
	while (std::getline(s, line)) { //loop through all the lines
		if (line.length() > 72) { //why 72? a line is 72 bytes plus a \r\n
			queue.push_back(
				line.substr(0, 71) //ditch the \r\n
			);
		}
	}
	if queue.size() > 0 {
		queue_read()
	}
}
