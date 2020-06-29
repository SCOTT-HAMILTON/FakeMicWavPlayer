#ifndef FAKE_MIC_WAV_PLAYER_LIB_H
#define FAKE_MIC_WAV_PLAYER_LIB_H

#include <FakeLib.h>
#include <string>
#include <exception>

static const char *defaultSourceProcessBinary = "Discord";
static const char *fakeCombinedSinkName = "fakecombinedsink";
static const char *sourceLoopbackSinkName = "sourceLoopbackSink";
static const char *userVolumeControlSinkName = "userVolumeControlSink";
static const char *sourceLoopBackMixerSinkName = "sourceLoopBackMixerSink";
static const char *sourceLoopBackMixerMonitorName = "sourceLoopBackMixerSink.monitor";
static const char *defaultCombinedSlavesList =
    "alsa_output.pci-0000_00_1f.3.analog-stereo";

struct NegativeVolumeError : public std::exception
{
	const char* what() const noexcept override {
		return "Volume passed is negative";
	}
};

struct TooHighVolumeError : public std::exception
{
	const char* what() const noexcept override {
		return "Volume passed is above 100%";
	}
};

struct CantLoadModuleError : public std::exception
{
	const char* what() const noexcept override {
		return "Couldn't load module";
	}
};

class FakeMicWavPlayer {

public:
	static int init(const std::string& fileName,
			std::string source,
			std::string combinedSlavesList,
			std::string sourceProcessBinary);
	static int playNonBlocking();
	static int cleanPlayer();
	static int set_source_volume(double volume);
	static int set_user_volume(double volume);
	static void clean();

	static FakeLib fakeLib;

private:
	static sink_infos_t fakeCombinedSink;
	static sink_infos_t sourceLoopbackSink;
	static sink_infos_t userVolumeControlSink;
};

#endif //FAKE_MIC_WAV_PLAYER_LIB_H
