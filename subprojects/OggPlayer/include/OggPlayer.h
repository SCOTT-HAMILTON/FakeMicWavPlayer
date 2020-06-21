#ifndef OGG_PLAYER_H
#define OGG_PLAYER_H

#include <string>

class OggPlayer {
public:
	static int init(const std::string& oggFilename, const std::string& device);
	static int play();

private:
	static std::string filename;
	static std::string device;
};


#endif //OGG_PLAYER_H
