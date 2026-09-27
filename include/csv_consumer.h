#ifndef CSV_CONSUMER_H
#define CSV_CONSUMER_H

#include "buffer.h"

typedef struct {
    BoundedBuffer *buffer;
    const char *csv_path;
} CSVConsumerArgs;

void *csv_consumer_thread_func(void *arg);

#endif // CSV_CONSUMER_H
