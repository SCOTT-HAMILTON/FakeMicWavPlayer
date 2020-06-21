#include "pa_player.h"
#include "ogg.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <pulse/error.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


int PaPlayer::init(const play_arguments_t *play_arguments) {
	/* The Sample format to use */
	static const pa_sample_spec ss = {
	    .format = PA_SAMPLE_S16LE, .rate = 44100, .channels = 2};
	s = NULL;

	/* Create a new playback stream */
	if (!(s = pa_simple_new(NULL, "Scott's Wav Player",
				PA_STREAM_PLAYBACK, /*device*/
				play_arguments->device, "Playing Music with Fake", &ss, NULL,
				NULL, &error))) {
		fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n",
			pa_strerror(error));
		fprintf(stderr, "The device to play on was '%s'\n",
			play_arguments->device);
		clean();
		return 1;
	}
	return 0;
}

int PaPlayer::finish() {
	/* Make sure that every single sample was played */
	if (pa_simple_drain(s, &error) < 0) {
		fprintf(stderr, __FILE__ ": pa_simple_drain() failed: %s\n",
			pa_strerror(error));
		clean();
		return 1;
	}
	return 0;
}
void PaPlayer::clean() {
	fputs("Cleaning...", stderr);
	if (s)
		pa_simple_free(s);
}
int PaPlayer::write_callback(const void * buf, size_t size, size_t count) {
	if (s == NULL) {
		fputs("Error, pa_simple instance is null, exiting\n", stderr);
		return 1;
	}
	int result = pa_simple_write(s, buf, (size_t)size*count, &error);
	if (result < 0) {
		fprintf(stderr,
			__FILE__ ": pa_simple_write() failed: %s\n",
			pa_strerror(error));
	}
	return result;
}
pa_simple* PaPlayer::s = NULL;
int PaPlayer::error;
