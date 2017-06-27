#ifndef DB_H
#define DB_H

#include <time.h>
#include <stdint.h>
#include <stdio.h>

struct db {
	uint64_t version;
	time_t last_modified;
	time_t initialized;
	struct db_entry *root;
};

struct db_entry {
	char *dir;
	int numdirs;
	int numfiles;
	struct db_entry *dirs;
	struct db_file *files;
};

struct db_file {
	
};

struct db *get_db(char *path);
void save_db(struct db *db, char *path);
void update_db(struct db *db);

void print_db(struct db *db, FILE *f);
void free_db(struct db *db);

#endif
