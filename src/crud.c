#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "crud.h"
#include "stadium.h"
#include "database.h"
#include "csv.h"
#include "curl_download.h"
#include "parser.h"
#include "cleaner.h"
#include "buffer.h"
#include "producer.h"
#include "csv_consumer.h"
#include "mysql_consumer.h"

static void get_input_string(const char *prompt, char *buffer, size_t buf_size) {
    printf("%s", prompt);
    if (fgets(buffer, buf_size, stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

static int get_input_int(const char *prompt) {
    char buffer[64];
    int val = 0;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &val) == 1) {
                return val;
            }
        }
        printf("[!] Invalid integer format. Please re-enter.\n");
    }
}

static void print_stadium_table(const Stadium *stadiums, int count) {
    if (count <= 0 || !stadiums) {
        printf("\n[!] No stadium records found.\n");
        return;
    }

    printf("\n+-----+----------------------------------+-------------------+-------------------+----------+---------------+-------------------+------+--------+\n");
    printf("| ID  | Stadium Name                     | City              | State             | Capacity | Sport         | Home Team         | Year | Surface|\n");
    printf("+-----+----------------------------------+-------------------+-------------------+----------+---------------+-------------------+------+--------+\n");

    for (int i = 0; i < count; i++) {
        printf("| %-3d | %-32.32s | %-17.17s | %-17.17s | %-8d | %-13.13s | %-17.17s | %-4d | %-6.6s |\n",
               stadiums[i].stadium_id,
               stadiums[i].stadium_name,
               stadiums[i].city,
               stadiums[i].state,
               stadiums[i].capacity,
               stadiums[i].sport,
               stadiums[i].home_team,
               stadiums[i].opened_year,
               stadiums[i].surface);
    }
    printf("+-----+----------------------------------+-------------------+-------------------+----------+---------------+-------------------+------+--------+\n");
    printf(" Total Records: %d\n\n", count);
}

int run_full_pipeline(MYSQL *conn) {
    printf("\n=======================================================\n");
    printf("   STARTING MULTITHREADED STADIUM DATA PIPELINE\n");
    printf("=======================================================\n");

    const char *url = "https://en.wikipedia.org/wiki/List_of_stadiums_in_India";

    // 1. Download webpage via libcurl
    MemoryBuffer mem;
    if (download_webpage(url, &mem) != 0) {
        fprintf(stderr, "[PIPELINE ERROR] Download failed!\n");
        return -1;
    }

    // 2. Parse HTML table via libxml2
    Stadium *scraped_stadiums = NULL;
    int raw_count = 0;
    if (parse_stadiums_from_html(mem.data, &scraped_stadiums, &raw_count) != 0) {
        fprintf(stderr, "[PIPELINE ERROR] Parsing failed!\n");
        free_memory_buffer(&mem);
        return -1;
    }
    free_memory_buffer(&mem);

    // 3. Clean and normalize data
    clean_stadium_dataset(scraped_stadiums, raw_count);

    // 4. Clear database table before fresh ingestion
    if (conn) {
        db_clear_table(conn);
    }

    // 5. Setup Multithreading Bounded Buffer Architecture
    BoundedBuffer buffer_csv;
    BoundedBuffer buffer_mysql;
    buffer_init(&buffer_csv);
    buffer_init(&buffer_mysql);

    ProducerArgs pargs;
    pargs.stadiums = scraped_stadiums;
    pargs.count = raw_count;
    pargs.buffer_csv = &buffer_csv;
    pargs.buffer_mysql = &buffer_mysql;

    CSVConsumerArgs cargs;
    cargs.buffer = &buffer_csv;
    cargs.csv_path = "data/stadiums.csv";

    MySQLConsumerArgs margs;
    margs.buffer = &buffer_mysql;

    pthread_t producer_tid, csv_consumer_tid, mysql_consumer_tid;

    printf("[Pipeline] Spawning Producer & Consumer Threads...\n");
    pthread_create(&producer_tid, NULL, producer_thread_func, &pargs);
    pthread_create(&csv_consumer_tid, NULL, csv_consumer_thread_func, &cargs);
    pthread_create(&mysql_consumer_tid, NULL, mysql_consumer_thread_func, &margs);

    printf("[Pipeline] Synchronizing threads (pthread_join)...\n");
    pthread_join(producer_tid, NULL);
    pthread_join(csv_consumer_tid, NULL);
    pthread_join(mysql_consumer_tid, NULL);

    buffer_destroy(&buffer_csv);
    buffer_destroy(&buffer_mysql);

    if (scraped_stadiums) free(scraped_stadiums);

    printf("\n[PIPELINE SUCCESS] Data pipeline completed end-to-end!\n");
    printf("=======================================================\n\n");
    return 0;
}

void run_crud_menu(MYSQL *conn) {
    int choice = 0;

    while (1) {
        printf("\n=======================================================\n");
        printf("        INDIAN STADIUM MANAGEMENT SYSTEM (C & MySQL)   \n");
        printf("=======================================================\n");
        printf("  1. Add New Stadium (CREATE)\n");
        printf("  2. Display All Stadiums (READ)\n");
        printf("  3. Search Stadiums\n");
        printf("  4. Update Stadium Information (UPDATE)\n");
        printf("  5. Delete Stadium Record (DELETE)\n");
        printf("  6. Export Database Records to CSV\n");
        printf("  7. Re-run Web Scraper Pipeline (libcurl + libxml2 + Threads)\n");
        printf("  8. Exit System\n");
        printf("=======================================================\n");

        choice = get_input_int("Enter your choice (1-8): ");

        switch (choice) {
            case 1: { // CREATE
                printf("\n--- Add New Stadium ---\n");
                Stadium s;
                memset(&s, 0, sizeof(Stadium));

                get_input_string("Enter Stadium Name: ", s.stadium_name, sizeof(s.stadium_name));
                get_input_string("Enter City: ", s.city, sizeof(s.city));
                get_input_string("Enter State: ", s.state, sizeof(s.state));
                s.capacity = get_input_int("Enter Capacity: ");
                get_input_string("Enter Sport (e.g. Cricket, Football): ", s.sport, sizeof(s.sport));
                get_input_string("Enter Home Team: ", s.home_team, sizeof(s.home_team));
                s.opened_year = get_input_int("Enter Opening Year: ");
                get_input_string("Enter Surface (e.g. Grass, Turf): ", s.surface, sizeof(s.surface));

                clean_stadium_record(&s);

                if (db_insert_stadium(conn, &s) == 0) {
                    printf("[SUCCESS] Stadium '%s' added successfully to database!\n", s.stadium_name);
                } else {
                    printf("[ERROR] Failed to insert stadium into database.\n");
                }
                break;
            }
            case 2: { // READ ALL
                printf("\n--- Displaying All Stadiums ---\n");
                Stadium *list = NULL;
                int count = 0;
                if (db_get_all_stadiums(conn, &list, &count) == 0) {
                    print_stadium_table(list, count);
                    if (list) free(list);
                }
                break;
            }
            case 3: { // SEARCH
                printf("\n--- Search Stadiums ---\n");
                printf("  1. Search by Name\n");
                printf("  2. Search by City\n");
                printf("  3. Search by State\n");
                printf("  4. Search by Sport\n");
                int sub = get_input_int("Select search attribute (1-4): ");
                const char *field = "stadium_name";
                if (sub == 2) field = "city";
                else if (sub == 3) field = "state";
                else if (sub == 4) field = "sport";

                char val[150];
                get_input_string("Enter search term: ", val, sizeof(val));

                Stadium *list = NULL;
                int count = 0;
                if (db_search_stadiums(conn, field, val, &list, &count) == 0) {
                    print_stadium_table(list, count);
                    if (list) free(list);
                }
                break;
            }
            case 4: { // UPDATE
                printf("\n--- Update Stadium Information ---\n");
                int id = get_input_int("Enter Stadium ID to update: ");

                char query[256];
                snprintf(query, sizeof(query), "stadium_id = %d", id);
                Stadium *list = NULL;
                int count = 0;

                char id_str[32];
                snprintf(id_str, sizeof(id_str), "%d", id);
                if (db_search_stadiums(conn, "stadium_id", id_str, &list, &count) == 0 && count > 0) {
                    Stadium s = list[0];
                    if (list) free(list);

                    printf("\n[Current Record] Name: %s | City: %s | Capacity: %d | Sport: %s\n",
                           s.stadium_name, s.city, s.capacity, s.sport);

                    get_input_string("Enter New Stadium Name (Leave blank to keep current): ", s.stadium_name, sizeof(s.stadium_name));
                    get_input_string("Enter New City (Leave blank to keep current): ", s.city, sizeof(s.city));
                    get_input_string("Enter New State (Leave blank to keep current): ", s.state, sizeof(s.state));

                    char cap_str[64];
                    get_input_string("Enter New Capacity (Leave blank to keep current): ", cap_str, sizeof(cap_str));
                    if (strlen(cap_str) > 0) s.capacity = atoi(cap_str);

                    get_input_string("Enter New Sport: ", s.sport, sizeof(s.sport));
                    get_input_string("Enter New Home Team: ", s.home_team, sizeof(s.home_team));

                    char yr_str[64];
                    get_input_string("Enter New Year Opened: ", yr_str, sizeof(yr_str));
                    if (strlen(yr_str) > 0) s.opened_year = atoi(yr_str);

                    clean_stadium_record(&s);
                    s.stadium_id = id;

                    if (db_update_stadium(conn, &s) == 0) {
                        printf("[SUCCESS] Stadium ID %d updated successfully!\n", id);
                    } else {
                        printf("[ERROR] Failed to update stadium ID %d\n", id);
                    }
                } else {
                    printf("[!] Stadium ID %d not found.\n", id);
                }
                break;
            }
            case 5: { // DELETE
                printf("\n--- Delete Stadium Record ---\n");
                int id = get_input_int("Enter Stadium ID to delete: ");

                char confirm[10];
                get_input_string("Are you sure you want to delete this stadium? (y/N): ", confirm, sizeof(confirm));
                if (confirm[0] == 'y' || confirm[0] == 'Y') {
                    int rows = db_delete_stadium(conn, id);
                    if (rows > 0) {
                        printf("[SUCCESS] Stadium ID %d deleted from database.\n", id);
                    } else {
                        printf("[!] Stadium ID %d not found or deletion failed.\n", id);
                    }
                } else {
                    printf("[!] Deletion cancelled.\n");
                }
                break;
            }
            case 6: { // EXPORT CSV
                printf("\n--- Exporting Database to CSV ---\n");
                if (export_database_to_csv(conn, "data/stadiums.csv") == 0) {
                    printf("[SUCCESS] Exported all records to data/stadiums.csv\n");
                }
                break;
            }
            case 7: { // RE-RUN PIPELINE
                run_full_pipeline(conn);
                break;
            }
            case 8: { // EXIT
                printf("\n[System] Exiting Indian Stadium Management System. Goodbye!\n");
                return;
            }
            default:
                printf("\n[!] Invalid option (%d). Please choose between 1 and 8.\n", choice);
                break;
        }
    }
}
