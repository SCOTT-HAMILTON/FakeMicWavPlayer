#ifndef FAKE_MIC_WAV_PLAYER_LIB_H
#define FAKE_MIC_WAV_PLAYER_LIB_H

#include <FakeLib.h>
#include <string>

int FakeAndPlayWav(const std::string& fileName,
		   std::string combinedSlavesList,
		   std::string sourceProcessBinary);

void clean();

#endif //FAKE_MIC_WAV_PLAYER_LIB_H
