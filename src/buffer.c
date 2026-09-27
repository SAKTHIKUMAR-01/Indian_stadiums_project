#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

void buffer_init(BoundedBuffer *buf) {
    if (!buf) return;
    buf->in = 0;
    buf->out = 0;
    buf->count = 0;
    sem_init(&buf->empty, 0, BUFFER_CAPACITY);
    sem_init(&buf->full, 0, 0);
    pthread_mutex_init(&buf->mutex, NULL);
}

void buffer_destroy(BoundedBuffer *buf) {
    if (!buf) return;
    sem_destroy(&buf->empty);
    sem_destroy(&buf->full);
    pthread_mutex_destroy(&buf->mutex);
}

void buffer_push(BoundedBuffer *buf, const Stadium *item) {
    sem_wait(&buf->empty);
    pthread_mutex_lock(&buf->mutex);

    buf->data[buf->in] = *item;
    buf->in = (buf->in + 1) % BUFFER_CAPACITY;
    buf->count++;

    pthread_mutex_unlock(&buf->mutex);
    sem_post(&buf->full);
}

Stadium buffer_pop(BoundedBuffer *buf) {
    sem_wait(&buf->full);
    pthread_mutex_lock(&buf->mutex);

    Stadium item = buf->data[buf->out];
    buf->out = (buf->out + 1) % BUFFER_CAPACITY;
    buf->count--;

    pthread_mutex_unlock(&buf->mutex);
    sem_post(&buf->empty);

    return item;
}
