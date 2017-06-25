#include <stdio.h>
#include <stdbool.h>

#include "log.h"
#include "config.h"
#include "parse.h"
#include "util.h"
#include "db.h"

#define M_DB_VERSION 1ull

struct db *init_db()
{
	logmsg(debug, "Initializing db\n");
	struct db *r = (struct db*) safe_malloc(sizeof(struct db));
	r->head = NULL;
	r->last_modified = safe_time(&r->initialized);
	r->version = M_DB_VERSION;
	return r;
}

struct db *load_db(char *path)
{
	logmsg(debug, "Loading db at: %s\n", path);
	struct db *db = (struct db*) safe_malloc(sizeof(struct db));
	FILE *f = safe_fopen(path, "r");
	db->head = NULL;
	db->version = parse_int64(f);
	db->initialized = parse_int64(f);
	db->last_modified = parse_int64(f);
	safe_fclose(f);
	return db;
}

void save_db(struct db *db, char *path)
{
	logmsg(debug, "Going to save db at: %s\n", path);
	mkdir_p(path);
	db->last_modified = safe_time(NULL);

	mkdir_p(path);
	FILE *f = safe_fopen(path, "w");

	write_int64(f, db->version);
	write_int64(f, db->initialized);
	write_int64(f, db->last_modified);
	safe_fclose(f);
}

struct db *get_db(char *path)
{
	return path_exists(path) && get_force_reread() ? load_db(path) : init_db();
}

void print_db(struct db *db, FILE *f)
{
	safe_fputs("Database\n", f);
	if(db == NULL)
		safe_fputs("(NULL)\n", f);

	struct tm tm;
	localtime_r(&db->initialized, &tm);
	char b[200] = "";
	strftime(b, 200, "%c", &tm);
	safe_fprintf(f, "Initialized  : %s\n", b);

	localtime_r(&db->last_modified, &tm);
	strftime(b, 200, "%c", &tm);
	safe_fprintf(f, "Last modified: %s\n", b);
}

void free_db(struct db *db)
{
	free(db);
}
