#ifndef FAKE_MIC_WAV_PLAYER_LIB_H
#define FAKE_MIC_WAV_PLAYER_LIB_H

#include <FakeLib.h>
#include <string>
#include <exception>

class NegativeVolumeError : public std::exception
{
public:
	const char* what(){
		return "Volume passed is negative";
	}
};

class TooHighVolumeError : public std::exception
{
public:
	const char* what(){
		return "Volume passed is above 100%";
	}
};

class FakeMicWavPlayer {

public:
	static int init(const std::string& fileName,
		   std::string combinedSlavesList,
		   std::string sourceProcessBinary);
	static int play();
	static int set_volume(double volume);
	static void clean();

	static FakeLib fakeLib;

private:
	static sink_infos_t fakeCombinedSink;

};

#endif //FAKE_MIC_WAV_PLAYER_LIB_H
