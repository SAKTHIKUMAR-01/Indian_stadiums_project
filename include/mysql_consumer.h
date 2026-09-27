#ifndef MYSQL_CONSUMER_H
#define MYSQL_CONSUMER_H

#include "buffer.h"

typedef struct {
    BoundedBuffer *buffer;
} MySQLConsumerArgs;

void *mysql_consumer_thread_func(void *arg);

#endif // MYSQL_CONSUMER_H
