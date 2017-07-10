#include <emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>

#include <iostream>

std::vector<std::string> queue; //can't use char[72] so we use a string

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
	// bits 0-4: TMC count whatever that means
	// Units? Who knows
	// Endianness?
	unsigned char TMC;
	// bit 5: is it a rising edge
	bool RE;
};

class daqstatus {
	// Endianness?
	
	// bit 0: is the PPS count up to date
	bool ppsc;
	
	// bit 1: is the trigger (what's this?)
	bool trig_interrrupt;
	
	// bit 2: is the GPS data corrupted
	bool gps_corrupt;
	
	// bit 3: I have to quote here
	// "Current or last 1PPS rate is not within 41666666 ±50 CPLD clock tick.s (This is a result of a GPS glitch, the DAQ uC being busy, or the CPLD oscillator not tuned correctly.)"
	bool cpld_pps_mismatch;
};

class message {
	// Do we use little endian or big?  It would be nice to know!
	
	// 32 bit clock trigger count. Do not read data if value is 0
	// What does it mean? Only God knows
	// Endianness?
	unsigned long trigger;
	
	// Count of rising edge @ input 0
	risingedgecount re0;
	
	// Count of falling edge @ input 0
	fallingedgecount fe0;
	
	// Count of rising edge @ input 1
	risingedgecount re1;
	
	// Count of falling edge @ input 1
	fallingedgecount fe1;
	
	// Count of rising edge @ input 2
	risingedgecount re2;
	
	// Count of falling edge @ input 2
	fallingedgecount fe2;
	
	// Count of rising edge @ input 3
	risingedgecount re3;
	
	// Count of falling edge @ input 4
	fallingedgecount fe3;
	
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
}

int main() {
	EM_ASM(
		alert('hello world!');
	);
	risingedgecount rsc;
	std::string str("3A");
	rsc.set(str);
	return 0;
}
