#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cleaner.h"

static void strip_footnotes_and_normalize(char *str) {
    if (!str) return;

    char buffer[256];
    int j = 0;
    int in_bracket = 0;

    for (int i = 0; str[i] != '\0' && j < 250; i++) {
        if (str[i] == '[') {
            in_bracket = 1;
            continue;
        }
        if (str[i] == ']') {
            in_bracket = 0;
            continue;
        }
        if (in_bracket) continue;

        if (str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
            buffer[j++] = ' ';
        } else {
            buffer[j++] = str[i];
        }
    }
    buffer[j] = '\0';

    // Strip leading spaces
    char *start = buffer;
    while (*start && isspace((unsigned char)*start)) start++;

    // Strip trailing spaces
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    snprintf(str, 150, "%s", start);
}

void clean_stadium_record(Stadium *s) {
    if (!s) return;

    strip_footnotes_and_normalize(s->stadium_name);
    strip_footnotes_and_normalize(s->city);
    strip_footnotes_and_normalize(s->state);
    strip_footnotes_and_normalize(s->sport);
    strip_footnotes_and_normalize(s->home_team);
    strip_footnotes_and_normalize(s->surface);

    if (strlen(s->city) == 0) strcpy(s->city, "Unknown");
    if (strlen(s->state) == 0) strcpy(s->state, "Unknown");
    if (strlen(s->sport) == 0) strcpy(s->sport, "Cricket");
    if (strlen(s->home_team) == 0) strcpy(s->home_team, "Local Team");
    if (strlen(s->surface) == 0) strcpy(s->surface, "Grass");

    if (s->capacity < 0) s->capacity = 0;
    if (s->opened_year < 1800 || s->opened_year > 2030) s->opened_year = 0;
}

void clean_stadium_dataset(Stadium *stadiums, int count) {
    printf("[Cleaner] Normalizing %d extracted stadium records...\n", count);
    for (int i = 0; i < count; i++) {
        stadiums[i].stadium_id = i + 1;
        clean_stadium_record(&stadiums[i]);
    }
    printf("[Cleaner] Data normalization complete!\n");
}
