#include "FakeLib.h"
#include "help.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		help();
		return 1;
	}

	char combinedSlavesList[1024];
	char sourceProcessBinary[1024];
	if (argc > 2 && strcmp(argv[2], "default") != 0) {
		strcpy(combinedSlavesList, argv[2]);
	} else {
		strcpy(combinedSlavesList, defaultCombinedSlavesList);
	}

	if (argc > 3 && strcmp(argv[3], "default") != 0) {
		strcpy(sourceProcessBinary, argv[3]);
	} else {
		strcpy(sourceProcessBinary, defaultSourceProcessBinary);
	}

	const char *fileName = argv[1];

	return FakeAndPlayWav(fileName, combinedSlavesList, sourceProcessBinary);
}
