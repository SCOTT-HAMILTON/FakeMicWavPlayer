#ifndef OGG_H
#define OGG_H

#include <array>
#include <cstdio>
#include <functional>
#include <vorbis/codec.h>

using write_callback_t = std::function<int(const void * ptr, size_t size, size_t count)>;
extern void _VDBG_dump(void);

class OggDecoder
{
public:
	static int init(const char* filename, write_callback_t callback);
	static int playNonBlocking();
	static void clean();
	static void play(const char* filename, write_callback_t callback);

private:
	static int eos;
	static int convsize;
	static ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
	static ogg_stream_state os; /* take physical pages, weld into a logical
							stream of packets */
	static ogg_page         og; /* one Ogg bitstream page. Vorbis packets are inside */
	static ogg_packet       op; /* one raw packet of data for decode */

	static vorbis_info      vi; /* struct that stores all the static vorbis bitstream
							settings */
	static vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	static vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	static vorbis_block     vb; /* local working space for packet->PCM decode */
	static char *buffer;
	static int bytes;
	static FILE* musicFile;
	static write_callback_t write_callback;

};

#endif // OGG_H
