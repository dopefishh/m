#ifndef DB_H
#define DB_H

#include <stdint.h>

struct db {
	uint64_t version;
	time_t last_modified;
	time_t initialized;
	struct db_entry *head;
};

struct db_entry {
	char *root;
};

struct db *get_db(char *path);
void save_db(struct db *db, char *path);

void print_db(struct db *db, FILE *f);
void free_db(struct db *db);

#endif
