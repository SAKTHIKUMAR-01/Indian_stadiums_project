#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include "parser.h"

static void get_node_text(xmlNode *node, char *buffer, size_t buf_size) {
    if (!node || !buffer || buf_size == 0) return;
    buffer[0] = '\0';

    xmlChar *content = xmlNodeGetContent(node);
    if (content) {
        snprintf(buffer, buf_size, "%s", (const char *)content);
        xmlFree(content);
    }
}

static void strip_brackets_inplace(char *str) {
    if (!str) return;
    char buf[512];
    int j = 0;
    int in_b = 0;
    for (int i = 0; str[i] && j < 500; i++) {
        if (str[i] == '[') { in_b = 1; continue; }
        if (str[i] == ']') { in_b = 0; continue; }
        if (in_b) continue;
        if ((unsigned char)str[i] == 0xC2 || (unsigned char)str[i] == 0x86 || str[i] == '+' || str[i] == '*') continue;
        buf[j++] = str[i];
    }
    buf[j] = '\0';
    strcpy(str, buf);
}

static int is_table_row(xmlNode* node) {
    return (node->type == XML_ELEMENT_NODE && xmlStrcasecmp(node->name, (const xmlChar*)"tr") == 0);
}

static int is_table(xmlNode* node) {
    return (node->type == XML_ELEMENT_NODE && xmlStrcasecmp(node->name, (const xmlChar*)"table") == 0);
}

static void find_tables(xmlNode *root, xmlNode **tables, int *count, int max_tables) {
    xmlNode *cur = root;
    while (cur && *count < max_tables) {
        if (is_table(cur)) {
            tables[(*count)++] = cur;
        }
        if (cur->children) {
            find_tables(cur->children, tables, count, max_tables);
        }
        cur = cur->next;
    }
}

static void find_rows(xmlNode *parent, xmlNode **rows, int *count, int max_rows) {
    xmlNode *cur = parent;
    while (cur && *count < max_rows) {
        if (is_table_row(cur)) {
            rows[(*count)++] = cur;
        } else if (cur->children && xmlStrcasecmp(cur->name, (const xmlChar*)"table") != 0) {
            find_rows(cur->children, rows, count, max_rows);
        }
        cur = cur->next;
    }
}

static void str_tolower(char *str) {
    for (; *str; ++str) *str = (char)tolower((unsigned char)*str);
}

int parse_stadiums_from_html(const char *html_content, Stadium **stadiums_out, int *count_out) {
    if (!html_content || !stadiums_out || !count_out) return -1;

    *count_out = 0;
    *stadiums_out = NULL;

    htmlDocPtr doc = htmlReadMemory(html_content, (int)strlen(html_content), NULL, "UTF-8", HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) {
        fprintf(stderr, "[ERROR] libxml2 failed to parse HTML memory\n");
        return -1;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root) {
        fprintf(stderr, "[ERROR] Empty HTML document root\n");
        xmlFreeDoc(doc);
        return -1;
    }

    xmlNode *tables[50];
    int num_tables = 0;
    find_tables(root, tables, &num_tables, 50);

    printf("[libxml2] Found %d tables in downloaded HTML page\n", num_tables);

    int capacity_alloc = 200;
    Stadium *stadiums = malloc(sizeof(Stadium) * capacity_alloc);
    if (!stadiums) {
        xmlFreeDoc(doc);
        return -1;
    }

    int total_parsed = 0;
    int id_counter = 1;

    for (int t = 0; t < num_tables; t++) {
        xmlNode *rows[500];
        int num_rows = 0;
        find_rows(tables[t]->children, rows, &num_rows, 500);

        int col_stadium = -1;
        int col_capacity = -1;
        int col_city = -1;
        int col_state = -1;
        int col_sport = -1;
        int col_team = -1;
        int col_year = -1;

        for (int r = 0; r < num_rows; r++) {
            xmlNode *row = rows[r];
            xmlNode *cell = row->children;
            char cell_texts[20][512];
            int cell_count = 0;
            int is_header_row = 0;

            while (cell && cell_count < 20) {
                if (cell->type == XML_ELEMENT_NODE &&
                   (xmlStrcasecmp(cell->name, (const xmlChar*)"td") == 0 ||
                    xmlStrcasecmp(cell->name, (const xmlChar*)"th") == 0)) {
                    
                    if (xmlStrcasecmp(cell->name, (const xmlChar*)"th") == 0) {
                        is_header_row = 1;
                    }
                    get_node_text(cell, cell_texts[cell_count], sizeof(cell_texts[cell_count]));
                    strip_brackets_inplace(cell_texts[cell_count]);
                    cell_count++;
                }
                cell = cell->next;
            }

            if (cell_count < 3) continue;

            // Check if header row to map columns
            if (is_header_row || strstr(cell_texts[0], "Stadium") || strstr(cell_texts[1], "Stadium") || strstr(cell_texts[2], "Stadium")) {
                for (int c = 0; c < cell_count; c++) {
                    char lower[512];
                    snprintf(lower, sizeof(lower), "%s", cell_texts[c]);
                    str_tolower(lower);

                    if (strstr(lower, "stadium") || strstr(lower, "venue") || strstr(lower, "name")) {
                        if (col_stadium == -1) col_stadium = c;
                    } else if (strstr(lower, "capacity") || strstr(lower, "seating")) {
                        if (col_capacity == -1) col_capacity = c;
                    } else if (strstr(lower, "city") || strstr(lower, "location")) {
                        if (col_city == -1) col_city = c;
                    } else if (strstr(lower, "state") || strstr(lower, "region")) {
                        if (col_state == -1) col_state = c;
                    } else if (strstr(lower, "sport")) {
                        if (col_sport == -1) col_sport = c;
                    } else if (strstr(lower, "team") || strstr(lower, "tenant") || strstr(lower, "home")) {
                        if (col_team == -1) col_team = c;
                    } else if (strstr(lower, "open") || strstr(lower, "built") || strstr(lower, "year")) {
                        if (col_year == -1) col_year = c;
                    }
                }
                continue; // Skip header row processing as data
            }

            // Fallbacks if headers weren't found explicitly
            int idx_stadium = (col_stadium != -1) ? col_stadium : (cell_count >= 6 ? 2 : 0);
            int idx_capacity = (col_capacity != -1) ? col_capacity : (cell_count >= 6 ? 3 : 1);
            int idx_city = (col_city != -1) ? col_city : (cell_count >= 6 ? 4 : 2);
            int idx_state = (col_state != -1) ? col_state : (cell_count >= 6 ? 5 : 3);
            int idx_sport = (col_sport != -1) ? col_sport : 6;
            int idx_team = (col_team != -1) ? col_team : 7;
            int idx_year = (col_year != -1) ? col_year : 8;

            // Bounds check
            if (idx_stadium >= cell_count) idx_stadium = 0;
            if (idx_capacity >= cell_count) idx_capacity = 1;

            Stadium s;
            memset(&s, 0, sizeof(Stadium));
            s.stadium_id = id_counter;

            snprintf(s.stadium_name, sizeof(s.stadium_name), "%.140s", cell_texts[idx_stadium]);
            if (idx_city < cell_count) snprintf(s.city, sizeof(s.city), "%.90s", cell_texts[idx_city]);
            if (idx_state < cell_count) snprintf(s.state, sizeof(s.state), "%.90s", cell_texts[idx_state]);

            // Capacity extraction
            int cap_raw = 0;
            char *cap_str = cell_texts[idx_capacity];
            for (int k = 0; cap_str[k]; k++) {
                if (isdigit((unsigned char)cap_str[k])) {
                    cap_raw = cap_raw * 10 + (cap_str[k] - '0');
                }
            }
            if (cap_raw > 200000) {
                // If extra digits were concatenated, clamp to valid max or standard stadium capacity range
                while (cap_raw > 200000) cap_raw /= 10;
            }
            s.capacity = cap_raw;

            if (idx_sport < cell_count && strlen(cell_texts[idx_sport]) > 0) {
                snprintf(s.sport, sizeof(s.sport), "%.90s", cell_texts[idx_sport]);
            } else {
                snprintf(s.sport, sizeof(s.sport), "Multi-purpose");
            }

            if (idx_team < cell_count && strlen(cell_texts[idx_team]) > 0) {
                snprintf(s.home_team, sizeof(s.home_team), "%.140s", cell_texts[idx_team]);
            } else {
                snprintf(s.home_team, sizeof(s.home_team), "Local Teams");
            }

            if (idx_year < cell_count) {
                int yr = 0;
                char *yr_str = cell_texts[idx_year];
                for (int k = 0; yr_str[k]; k++) {
                    if (isdigit((unsigned char)yr_str[k])) {
                        yr = yr * 10 + (yr_str[k] - '0');
                    }
                }
                s.opened_year = (yr >= 1800 && yr <= 2030) ? yr : 0;
            }

            snprintf(s.surface, sizeof(s.surface), "Grass");

            if (strlen(s.stadium_name) > 2 && s.capacity > 100) {
                if (total_parsed >= capacity_alloc) {
                    capacity_alloc *= 2;
                    Stadium *temp = realloc(stadiums, sizeof(Stadium) * capacity_alloc);
                    if (temp) {
                        stadiums = temp;
                    } else {
                        break;
                    }
                }
                stadiums[total_parsed++] = s;
                id_counter++;
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    printf("[libxml2] Extracted %d stadium records from HTML tables\n", total_parsed);
    *stadiums_out = stadiums;
    *count_out = total_parsed;

    return 0;
}
