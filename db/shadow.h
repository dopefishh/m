#ifndef DB_SHADOW_H
#define DB_SHADOW_H

#include <stdint.h>
#include <stdbool.h>

#include "../db.h"
#include "../list.h"

struct shadow_db {
	uint64_t num;
	char **keys;
};

struct shadow_db_fork {
	char *key;
	char *value;
	bool isfork;
	uint64_t num;
	union {
		struct shadow_db_fork *forks;
		struct db_file *leafs;
	} data;
};

struct shadow_db *index_db(struct db *, struct listitem *);
#endif
