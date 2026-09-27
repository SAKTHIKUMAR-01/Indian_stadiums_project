#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

MYSQL *db_connect(void) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "[ERROR] mysql_init() failed\n");
        return NULL;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, NULL, DB_PORT, NULL, 0) == NULL) {
        fprintf(stderr, "[ERROR] MySQL connection error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    // Ensure database exists
    if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS " DB_NAME) != 0) {
        fprintf(stderr, "[ERROR] Failed to create database: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    if (mysql_select_db(conn, DB_NAME) != 0) {
        fprintf(stderr, "[ERROR] Failed to select database '%s': %s\n", DB_NAME, mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    return conn;
}

int db_init_schema(MYSQL *conn) {
    if (!conn) return -1;

    const char *sql = 
        "CREATE TABLE IF NOT EXISTS stadiums ("
        "  stadium_id INT PRIMARY KEY AUTO_INCREMENT,"
        "  stadium_name VARCHAR(150) NOT NULL,"
        "  city VARCHAR(100) DEFAULT 'Unknown',"
        "  state VARCHAR(100) DEFAULT 'Unknown',"
        "  capacity INT DEFAULT 0,"
        "  sport VARCHAR(100) DEFAULT 'Cricket',"
        "  home_team VARCHAR(150) DEFAULT 'None',"
        "  opened_year INT DEFAULT 0,"
        "  surface VARCHAR(50) DEFAULT 'Grass'"
        ");";

    if (mysql_query(conn, sql) != 0) {
        fprintf(stderr, "[ERROR] Failed to create table schema: %s\n", mysql_error(conn));
        return -1;
    }

    return 0;
}

int db_clear_table(MYSQL *conn) {
    if (!conn) return -1;
    if (mysql_query(conn, "TRUNCATE TABLE stadiums") != 0) {
        // Fallback if TRUNCATE fails
        mysql_query(conn, "DELETE FROM stadiums");
    }
    return 0;
}

int db_insert_stadium(MYSQL *conn, const Stadium *s) {
    if (!conn || !s) return -1;

    char esc_name[300], esc_city[200], esc_state[200], esc_sport[200], esc_team[300], esc_surface[100];
    mysql_real_escape_string(conn, esc_name, s->stadium_name, (unsigned long)strlen(s->stadium_name));
    mysql_real_escape_string(conn, esc_city, s->city, (unsigned long)strlen(s->city));
    mysql_real_escape_string(conn, esc_state, s->state, (unsigned long)strlen(s->state));
    mysql_real_escape_string(conn, esc_sport, s->sport, (unsigned long)strlen(s->sport));
    mysql_real_escape_string(conn, esc_team, s->home_team, (unsigned long)strlen(s->home_team));
    mysql_real_escape_string(conn, esc_surface, s->surface, (unsigned long)strlen(s->surface));

    char query[2048];
    snprintf(query, sizeof(query),
        "INSERT INTO stadiums (stadium_name, city, state, capacity, sport, home_team, opened_year, surface) "
        "VALUES ('%s', '%s', '%s', %d, '%s', '%s', %d, '%s')",
        esc_name, esc_city, esc_state, s->capacity, esc_sport, esc_team, s->opened_year, esc_surface);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "[ERROR] Insert query failed: %s\n", mysql_error(conn));
        return -1;
    }

    return 0;
}

int db_update_stadium(MYSQL *conn, const Stadium *s) {
    if (!conn || !s) return -1;

    char esc_name[300], esc_city[200], esc_state[200], esc_sport[200], esc_team[300], esc_surface[100];
    mysql_real_escape_string(conn, esc_name, s->stadium_name, (unsigned long)strlen(s->stadium_name));
    mysql_real_escape_string(conn, esc_city, s->city, (unsigned long)strlen(s->city));
    mysql_real_escape_string(conn, esc_state, s->state, (unsigned long)strlen(s->state));
    mysql_real_escape_string(conn, esc_sport, s->sport, (unsigned long)strlen(s->sport));
    mysql_real_escape_string(conn, esc_team, s->home_team, (unsigned long)strlen(s->home_team));
    mysql_real_escape_string(conn, esc_surface, s->surface, (unsigned long)strlen(s->surface));

    char query[2048];
    snprintf(query, sizeof(query),
        "UPDATE stadiums SET stadium_name='%s', city='%s', state='%s', capacity=%d, "
        "sport='%s', home_team='%s', opened_year=%d, surface='%s' WHERE stadium_id=%d",
        esc_name, esc_city, esc_state, s->capacity, esc_sport, esc_team, s->opened_year, esc_surface, s->stadium_id);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "[ERROR] Update query failed: %s\n", mysql_error(conn));
        return -1;
    }

    return 0;
}

int db_delete_stadium(MYSQL *conn, int stadium_id) {
    if (!conn) return -1;

    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM stadiums WHERE stadium_id=%d", stadium_id);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "[ERROR] Delete query failed: %s\n", mysql_error(conn));
        return -1;
    }

    return (int)mysql_affected_rows(conn);
}

static int fetch_stadium_results(MYSQL *conn, Stadium **stadiums_out, int *count_out) {
    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        fprintf(stderr, "[ERROR] Failed to store result: %s\n", mysql_error(conn));
        *count_out = 0;
        *stadiums_out = NULL;
        return -1;
    }

    int num_rows = (int)mysql_num_rows(res);
    if (num_rows == 0) {
        *count_out = 0;
        *stadiums_out = NULL;
        mysql_free_result(res);
        return 0;
    }

    Stadium *list = malloc(sizeof(Stadium) * num_rows);
    if (!list) {
        mysql_free_result(res);
        return -1;
    }

    MYSQL_ROW row;
    int idx = 0;
    while ((row = mysql_fetch_row(res))) {
        Stadium s;
        memset(&s, 0, sizeof(Stadium));
        s.stadium_id = row[0] ? atoi(row[0]) : 0;
        snprintf(s.stadium_name, sizeof(s.stadium_name), "%s", row[1] ? row[1] : "");
        snprintf(s.city, sizeof(s.city), "%s", row[2] ? row[2] : "");
        snprintf(s.state, sizeof(s.state), "%s", row[3] ? row[3] : "");
        s.capacity = row[4] ? atoi(row[4]) : 0;
        snprintf(s.sport, sizeof(s.sport), "%s", row[5] ? row[5] : "");
        snprintf(s.home_team, sizeof(s.home_team), "%s", row[6] ? row[6] : "");
        s.opened_year = row[7] ? atoi(row[7]) : 0;
        snprintf(s.surface, sizeof(s.surface), "%s", row[8] ? row[8] : "");

        list[idx++] = s;
    }

    mysql_free_result(res);
    *stadiums_out = list;
    *count_out = num_rows;
    return 0;
}

int db_get_all_stadiums(MYSQL *conn, Stadium **stadiums_out, int *count_out) {
    if (!conn) return -1;
    if (mysql_query(conn, "SELECT stadium_id, stadium_name, city, state, capacity, sport, home_team, opened_year, surface FROM stadiums ORDER BY stadium_id ASC") != 0) {
        fprintf(stderr, "[ERROR] Select query failed: %s\n", mysql_error(conn));
        return -1;
    }
    return fetch_stadium_results(conn, stadiums_out, count_out);
}

int db_search_stadiums(MYSQL *conn, const char *field, const char *value, Stadium **stadiums_out, int *count_out) {
    if (!conn || !field || !value) return -1;

    char esc_val[300];
    mysql_real_escape_string(conn, esc_val, value, (unsigned long)strlen(value));

    char query[2048];
    snprintf(query, sizeof(query),
        "SELECT stadium_id, stadium_name, city, state, capacity, sport, home_team, opened_year, surface "
        "FROM stadiums WHERE %s LIKE '%%%s%%' ORDER BY capacity DESC", field, esc_val);

    if (mysql_query(conn, query) != 0) {
        fprintf(stderr, "[ERROR] Search query failed: %s\n", mysql_error(conn));
        return -1;
    }

    return fetch_stadium_results(conn, stadiums_out, count_out);
}
