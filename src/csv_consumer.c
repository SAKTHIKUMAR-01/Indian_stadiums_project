#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_consumer.h"

static void escape_csv_field(const char *input, char *output, size_t out_size) {
    if (strchr(input, ',') || strchr(input, '"') || strchr(input, '\n')) {
        snprintf(output, out_size, "\"%s\"", input);
    } else {
        snprintf(output, out_size, "%s", input);
    }
}

void *csv_consumer_thread_func(void *arg) {
    CSVConsumerArgs *cargs = (CSVConsumerArgs *)arg;
    FILE *fp = fopen(cargs->csv_path, "w");
    if (!fp) {
        fprintf(stderr, "[ERROR] CSV Consumer Thread failed to open CSV file: %s\n", cargs->csv_path);
        return NULL;
    }

    printf("[CSV Consumer Thread] Started writing to CSV dataset: %s\n", cargs->csv_path);

    // Write CSV Header
    fprintf(fp, "stadium_id,stadium_name,city,state,capacity,sport,home_team,opened_year,surface\n");

    int written_count = 0;

    while (1) {
        Stadium s = buffer_pop(cargs->buffer);
        if (s.stadium_id == -1) {
            // Sentinel received -> stop
            break;
        }

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

        written_count++;
    }

    fclose(fp);
    printf("[CSV Consumer Thread] Successfully wrote %d records to %s!\n", written_count, cargs->csv_path);
    return NULL;
}
