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
	return 0;
}

int OggPlayer::play() {
	decode_ogg(filename.c_str(), PaPlayer::write_callback);
	if (PaPlayer::finish() != 0)
		return 1;
	PaPlayer::clean();

	return 0;
}

std::string OggPlayer::filename;
std::string OggPlayer::device;
