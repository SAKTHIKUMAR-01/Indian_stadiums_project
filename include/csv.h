#ifndef CSV_H
#define CSV_H

#include <mysql.h>
#include "stadium.h"

int write_stadiums_to_csv(const char *filepath, const Stadium *stadiums, int count);
int export_database_to_csv(MYSQL *conn, const char *filepath);

#endif // CSV_H
