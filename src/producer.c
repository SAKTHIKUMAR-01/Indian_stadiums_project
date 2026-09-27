#include <stdio.h>
#include <stdlib.h>
#include "producer.h"

void *producer_thread_func(void *arg) {
    ProducerArgs *pargs = (ProducerArgs *)arg;
    printf("[Producer Thread] Started pushing %d stadium records to bounded buffers...\n", pargs->count);

    for (int i = 0; i < pargs->count; i++) {
        Stadium s = pargs->stadiums[i];
        if (pargs->buffer_csv) {
            buffer_push(pargs->buffer_csv, &s);
        }
        if (pargs->buffer_mysql) {
            buffer_push(pargs->buffer_mysql, &s);
        }
    }

    // Push Sentinel record (-1) to signal end of stream to consumer threads
    Stadium sentinel;
    sentinel.stadium_id = -1;

    if (pargs->buffer_csv) {
        buffer_push(pargs->buffer_csv, &sentinel);
    }
    if (pargs->buffer_mysql) {
        buffer_push(pargs->buffer_mysql, &sentinel);
    }

    printf("[Producer Thread] Finished pushing all records and sentinel markers!\n");
    return NULL;
}
