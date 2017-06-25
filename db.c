#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#include "log.h"
#include "config.h"
#include "parse.h"
#include "util.h"
#include "db.h"

#define M_DB_VERSION 1ull

struct db *init_db()
{
	logmsg(debug, "Initializing db\n");
	struct db *r = (struct db *) safe_malloc(sizeof(struct db));
	r->last_modified = safe_time(&r->initialized);
	r->version = M_DB_VERSION;
	r->root = (struct db_entry *)safe_malloc(sizeof(struct db_entry));
	r->root->dir = get_libraryroot();
	r->root->numdirs = 0;
	r->root->numfiles = 0;
	r->root->dirs = NULL;
	r->root->files = NULL;
	return r;
}

struct db *load_db(char *path)
{
	logmsg(debug, "Loading db at: %s\n", path);
	struct db *db = (struct db*) safe_malloc(sizeof(struct db));
	FILE *f = safe_fopen(path, "r");
	db->version = parse_int64(f);
	db->initialized = parse_int64(f);
	db->last_modified = parse_int64(f);
	db->root = (struct db_entry *)safe_malloc(sizeof(struct db_entry));
	db->root->dir = parse_string(f);
	db->root->numdirs = 0;
	db->root->numfiles = 0;
	db->root->dirs = NULL;
	db->root->files = NULL;
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

void recurse(char *p, dev_t dev, struct db_entry *entry)
{
	struct stat buf;
	if(stat(p, &buf) != 0){
		logmsg(warn, "Skipping %s because of an error\n");
		perror("stat");
		return;
	}

	if(buf.st_dev != dev && get_fix_filesystem()){
		logmsg(debug, "Skipping %s because it is at a different fs\n");
		return;
	}

	if(!S_ISDIR(buf.st_mode)){
		if(!S_ISREG(buf.st_mode)){
			//
			logmsg(debug, "Skipping %s because it is not a dir\n");
		}
		return;
	}

	char *pp;
	DIR *d = safe_opendir(p);

	struct dirent *de;
	while((de = safe_readdir(d)) != NULL){
		if(strcmp(de->d_name, ".") == 0
				|| strcmp(de->d_name, "..") == 0)
			continue;
		pp = get_file(p, de->d_name);


		recurse(pp, dev, entry);
		free(pp);
	}
	safe_closedir(d);
}

void update_db(struct db *db)
{
	logmsg(debug, "Updating db with root: %s\n", get_libraryroot());
	struct stat buf;
	if(stat(get_libraryroot(), &buf) == -1)
		perrordie("stat");

	if(strcmp(db->root->dir, get_libraryroot()) != 0){
		logmsg(warn, "This db has a different root. Aborting\n");
		return;
	}
	recurse(get_libraryroot(), buf.st_dev, db->root);
	(void)db;
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
