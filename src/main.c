#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "crud.h"

int main(void) {
    printf("=======================================================\n");
    printf("     INDIAN STADIUM MANAGEMENT SYSTEM - C PROJECT\n");
    printf("=======================================================\n");

    MYSQL *conn = db_connect();
    if (!conn) {
        fprintf(stderr, "[FATAL ERROR] Could not establish connection to MySQL database server!\n");
        fprintf(stderr, "Please verify MySQL server is running on 127.0.0.1:3306\n");
        return EXIT_FAILURE;
    }

    if (db_init_schema(conn) != 0) {
        fprintf(stderr, "[FATAL ERROR] Failed to initialize database schema!\n");
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // Check if stadiums table has data; if empty, run scraping pipeline automatically
    Stadium *stadiums = NULL;
    int count = 0;
    db_get_all_stadiums(conn, &stadiums, &count);
    if (stadiums) free(stadiums);

    if (count == 0) {
        printf("\n[Info] Database table is empty. Triggering initial web scraping pipeline...\n");
        run_full_pipeline(conn);
    } else {
        printf("\n[Info] Connected to MySQL. Database contains %d existing stadium records.\n", count);
    }

    // Launch interactive CRUD menu
    run_crud_menu(conn);

    mysql_close(conn);
    return EXIT_SUCCESS;
}
