#include <stdio.h>
#include <stdlib.h>
#include "mysql_consumer.h"
#include "database.h"

void *mysql_consumer_thread_func(void *arg) {
    MySQLConsumerArgs *margs = (MySQLConsumerArgs *)arg;

    MYSQL *conn = db_connect();
    if (!conn) {
        fprintf(stderr, "[ERROR] MySQL Consumer Thread failed to connect to MySQL database server\n");
        return NULL;
    }

    printf("[MySQL Consumer Thread] Started inserting stadium records into MySQL database...\n");

    int inserted_count = 0;

    while (1) {
        Stadium s = buffer_pop(margs->buffer);
        if (s.stadium_id == -1) {
            // Sentinel received -> stop
            break;
        }

        if (db_insert_stadium(conn, &s) == 0) {
            inserted_count++;
        }
    }

    mysql_close(conn);
    printf("[MySQL Consumer Thread] Successfully inserted %d records into MySQL database!\n", inserted_count);
    return NULL;
}
