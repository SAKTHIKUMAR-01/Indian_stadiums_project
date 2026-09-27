#ifndef STADIUM_H
#define STADIUM_H

#define MAX_STR_LEN 150
#define SHORT_STR_LEN 100
#define SURFACE_LEN 50

typedef struct {
    int stadium_id;
    char stadium_name[MAX_STR_LEN];
    char city[SHORT_STR_LEN];
    char state[SHORT_STR_LEN];
    int capacity;
    char sport[SHORT_STR_LEN];
    char home_team[MAX_STR_LEN];
    int opened_year;
    char surface[SURFACE_LEN];
} Stadium;

#endif // STADIUM_H
