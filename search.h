#ifndef SEARCH_H
#define SEARCH_H

#include "db.h"
#include "config.h"

struct query {
	char *query;
};

void search_db(struct db *);

struct list *search(struct db *, struct query *);

struct query *parse_query(char *);

char *query_to_string(struct query *);

#endif
