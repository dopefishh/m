#ifndef DB_H
#define DB_H

#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct db {
	uint64_t version;
	time_t last_modified;
	time_t initialized;
	char *rootpath;
	struct db_entry *root;
};

struct db_entry {
	char *dir;
	uint64_t nfile;
	uint64_t ndir;
	struct db_file *files;
	struct db_entry *dirs;
};

struct db_file {
	char *path;
	time_t mtime;
	size_t size;
	uint64_t ntags;
	//This is sorted
	struct db_tag *tags;
};

struct db_tag {
	char *key;
	char *value;
};

struct db *get_db(char *path);
void save_db(struct db *db, char *path, bool verbose);
void update_db(struct db *db);

void *iterate_db(struct db_entry *db, void *st, void *(*stf)(struct db_file *, void *));

//Comparison
int db_file_cmp(const void *a1, const void *a2);
int db_entry_cmp(const void *a1, const void *a2);

void print_db(struct db *db, FILE *f);
void free_db(struct db *db);

#endif
