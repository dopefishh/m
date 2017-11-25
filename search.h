#ifndef SEARCH_H
#define SEARCH_H

#include "db.h"

struct query {
	char *query;
};

struct resultset *search(struct db *, struct query *);

struct query *parse_query(char *);

#endif
