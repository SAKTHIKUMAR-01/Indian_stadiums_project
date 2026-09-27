#ifndef PARSER_H
#define PARSER_H

#include "stadium.h"

int parse_stadiums_from_html(const char *html_content, Stadium **stadiums_out, int *count_out);

#endif // PARSER_H
