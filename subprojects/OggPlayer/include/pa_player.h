#ifndef PA_PLAYER_H
#define PA_PLAYER_H

#include <cstddef>
#include <pulse/simple.h>

#define BUFSIZE 1024

typedef struct play_arguments {
	const char *device;
} play_arguments_t;

class PaPlayer {
public:
	static int init(const play_arguments_t *play_arguments);
	static int finish();
	static void clean();
	static int write_callback(const void * ptr, std::size_t size, std::size_t count);

private:
	static pa_simple* s;
	static int error;

};


#endif //PA_PLAYER_H
