#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"
#include "database.h"

static void escape_csv_field(const char *input, char *output, size_t out_size) {
    if (strchr(input, ',') || strchr(input, '"') || strchr(input, '\n')) {
        snprintf(output, out_size, "\"%s\"", input);
    } else {
        snprintf(output, out_size, "%s", input);
    }
}

int write_stadiums_to_csv(const char *filepath, const Stadium *stadiums, int count) {
    if (!filepath || !stadiums) return -1;

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[ERROR] Cannot create/write CSV file at: %s\n", filepath);
        return -1;
    }

    fprintf(fp, "stadium_id,stadium_name,city,state,capacity,sport,home_team,opened_year,surface\n");

    for (int i = 0; i < count; i++) {
        Stadium s = stadiums[i];
        char name_esc[200], city_esc[150], state_esc[150], sport_esc[150], team_esc[200], surf_esc[100];
        escape_csv_field(s.stadium_name, name_esc, sizeof(name_esc));
        escape_csv_field(s.city, city_esc, sizeof(city_esc));
        escape_csv_field(s.state, state_esc, sizeof(state_esc));
        escape_csv_field(s.sport, sport_esc, sizeof(sport_esc));
        escape_csv_field(s.home_team, team_esc, sizeof(team_esc));
        escape_csv_field(s.surface, surf_esc, sizeof(surf_esc));

        fprintf(fp, "%d,%s,%s,%s,%d,%s,%s,%d,%s\n",
                s.stadium_id, name_esc, city_esc, state_esc, s.capacity,
                sport_esc, team_esc, s.opened_year, surf_esc);
    }

    fclose(fp);
    printf("[CSV Exporter] Successfully exported %d records to %s\n", count, filepath);
    return 0;
}

int export_database_to_csv(MYSQL *conn, const char *filepath) {
    Stadium *stadiums = NULL;
    int count = 0;

    if (db_get_all_stadiums(conn, &stadiums, &count) != 0) {
        fprintf(stderr, "[ERROR] Failed to query database records for CSV export\n");
        return -1;
    }

    int res = write_stadiums_to_csv(filepath, stadiums, count);
    if (stadiums) free(stadiums);
    return res;
}
