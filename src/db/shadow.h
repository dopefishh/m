#ifndef DB_SHADOW_H
#define DB_SHADOW_H

#include <stdint.h>
#include <stdbool.h>

#include "../db.h"
#include "../list.h"
#include "../format.h"

typedef struct listitem db_file_list;
typedef struct listitem shadow_db_fork_list;

struct shadow_db_fork {
	char *value;
	bool isfork;
	uint64_t num;
	union {
		struct shadow_db_fork *forks;
		db_file_list *leafs;
	} data;
};

struct shadow_db {
	uint64_t numkeys;
	formatting *keys;
	struct shadow_db_fork root;
};

struct shadow_db *index_db(struct db *, struct listitem *);
void free_db_index(struct shadow_db *);

#endif
