#ifndef CURL_DOWNLOAD_H
#define CURL_DOWNLOAD_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t size;
} MemoryBuffer;

int download_webpage(const char *url, MemoryBuffer *mem);
void free_memory_buffer(MemoryBuffer *mem);

#endif // CURL_DOWNLOAD_H
