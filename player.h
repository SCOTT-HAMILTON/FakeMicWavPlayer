#ifndef PLAYER_H
#define PLAYER_H

#if defined(__cplusplus)
#define _FAKELIB_START_HEADER_DECL extern "C" {
#define _FAKELIB_END_HEADER_DECL }
#else
#define _FAKELIB_START_HEADER_DECL
#define _FAKELIB_END_HEADER_DECL
#endif

_FAKELIB_START_HEADER_DECL

typedef struct play_arguments {
	const char *fileName;
	const char *device;
} play_arguments_t;

void play(const play_arguments_t *play_arguments);

_FAKELIB_END_HEADER_DECL

#endif // PLAYER_H
