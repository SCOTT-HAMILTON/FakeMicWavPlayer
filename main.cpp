#include "FakeMicWavPlayerLib.h"
#include "help.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help();
		return 1;
	}
	
	if (std::string arg1(argv[1]); arg1 == "--help" ||
	     arg1  == "-h") {
		help();
		return 0;
	}

	std::string combinedSlavesList;
	std::string sourceProcessBinary;
	if (argc > 2 && std::string(argv[2]) == "default") {
		combinedSlavesList = argv[2];
	}

	if (argc > 3 && strcmp(argv[3], "default") != 0) {
		sourceProcessBinary = argv[3];
	}

	const char *fileName = argv[1];

	return FakeAndPlayWav(fileName, combinedSlavesList,
			      sourceProcessBinary);
}
