#ifndef OGG_H
#define OGG_H

#include <functional>

using write_callback_t = std::function<int(const void * ptr, size_t size, size_t count)>;

extern void _VDBG_dump(void);

int decode_ogg(const char* filename, write_callback_t callback);

#endif // OGG_H
