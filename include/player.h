#ifndef PLAYER_H
#define PLAYER_H


typedef struct play_arguments {
	const char *fileName;
	const char *device;
} play_arguments_t;

void play(const play_arguments_t *play_arguments);

#endif // PLAYER_H
