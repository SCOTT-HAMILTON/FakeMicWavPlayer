#include "OggPlayer.h"
#include "pa_player.h"
#include "ogg.h"

#include <iostream>

int OggPlayer::init(const std::string& oggFilename, const std::string& sink_device) {
	filename = oggFilename;
	device = sink_device;
	
	play_arguments_t playArgs = {
		.device = device.c_str()
	};
	if (PaPlayer::init(&playArgs) != 0)
		return 1;
	if (OggDecoder::init(filename.c_str(), PaPlayer::write_callback) != 0)
		return 1;
	return 0;
}
int OggPlayer::playNonBlocking()
{
	return OggDecoder::playNonBlocking();
}

int OggPlayer::play() {
	while (playNonBlocking() == 0);
	clean();
	return 0;
}

int OggPlayer::clean() {
	if (PaPlayer::finish() != 0)
		return 1;
	OggDecoder::clean();
	PaPlayer::clean();
	return 0;
}

std::string OggPlayer::filename;
std::string OggPlayer::device;
