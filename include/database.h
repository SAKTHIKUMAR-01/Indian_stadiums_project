#ifndef DATABASE_H
#define DATABASE_H

#include <mysql.h>
#include "stadium.h"

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASS ""
#define DB_NAME "indian_stadium_db"
#define DB_PORT 3306

MYSQL *db_connect(void);
int db_init_schema(MYSQL *conn);
int db_clear_table(MYSQL *conn);
int db_insert_stadium(MYSQL *conn, const Stadium *s);
int db_update_stadium(MYSQL *conn, const Stadium *s);
int db_delete_stadium(MYSQL *conn, int stadium_id);
int db_get_all_stadiums(MYSQL *conn, Stadium **stadiums_out, int *count_out);
int db_search_stadiums(MYSQL *conn, const char *field, const char *value, Stadium **stadiums_out, int *count_out);

#endif // DATABASE_H
