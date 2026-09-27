#ifndef PRODUCER_H
#define PRODUCER_H

#include "buffer.h"

typedef struct {
    Stadium *stadiums;
    int count;
    BoundedBuffer *buffer_csv;
    BoundedBuffer *buffer_mysql;
} ProducerArgs;

void *producer_thread_func(void *arg);

#endif // PRODUCER_H
