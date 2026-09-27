#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>
#include <semaphore.h>
#include "stadium.h"

#define BUFFER_CAPACITY 10

typedef struct {
    Stadium data[BUFFER_CAPACITY];
    int in;
    int out;
    int count;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} BoundedBuffer;

void buffer_init(BoundedBuffer *buf);
void buffer_destroy(BoundedBuffer *buf);
void buffer_push(BoundedBuffer *buf, const Stadium *item);
Stadium buffer_pop(BoundedBuffer *buf);

#endif // BUFFER_H
