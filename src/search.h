#ifndef SEARCH_H
#define SEARCH_H

#include "db.h"
#include "config.h"

struct query {
	char *query;
};

void search_db(struct db *);
struct query *parse_query(char *);
char *query_to_string(struct query *);
void search_key_add(char *);
void search_key_free();

#endif
