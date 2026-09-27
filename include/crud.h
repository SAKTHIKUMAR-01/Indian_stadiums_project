#ifndef CRUD_H
#define CRUD_H

#include <mysql.h>

void run_crud_menu(MYSQL *conn);
int run_full_pipeline(MYSQL *conn);

#endif // CRUD_H
