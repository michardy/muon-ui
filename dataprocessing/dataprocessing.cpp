#include <emscripten.h>
#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> queue; //can't use char[72] so we use a string

extern "C" void queue_push (char *test);

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
	while (true) {
		if (queue.size() > 0) {
			;
		}
	}
	return 0;
}
