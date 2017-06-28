#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#include <FLAC/metadata.h>

#include "log.h"
#include "config.h"
#include "parse.h"
#include "util.h"
#include "db.h"
#include "file.h"

#define M_DB_VERSION 1ull

struct db *init_db()
{
	logmsg(debug, "Initializing db\n");
	struct db *r = (struct db *) safe_malloc(sizeof(struct db));
	r->last_modified = safe_time(&r->initialized);
	r->version = M_DB_VERSION;
	r->root = (struct db_entry *)safe_malloc(sizeof(struct db_entry));
	r->root->dir = get_libraryroot();
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
	//init
	entry->dir = safe_strdup(p);

	//Check permissions
	struct stat buf;
	if(stat(p, &buf) != 0){
		logmsg(warn, "Skipping %s because of an error\n");
		perror("stat");
		return;
	}

	//Read directory and count files
	DIR *d = safe_opendir(p);
	long dl = safe_telldir(d);
	entry->nfile = 0;
	entry->ndir = 0;
	struct dirent *de;
	while((de = safe_readdir(d)) != NULL){
		if(strcmp(de->d_name, ".") == 0
				|| strcmp(de->d_name, "..") == 0)
			continue;
		if(stat(p, &buf) != 0){
			logmsg(warn, "Skipping %s because of an error\n");
			perror("stat");
			continue;
		}
		if(buf.st_dev != dev && get_fix_filesystem()){
			logmsg(debug, "Skipping %s because it is at a different fs\n");
			continue;
		}
		if(S_ISDIR(buf.st_mode))
			entry->ndir++;
		if(S_ISREG(buf.st_mode))
			entry->nfile++;
	}

	//Allocate array
	entry->files = safe_calloc(entry->nfile, sizeof(struct db_file));
	entry->dirs = safe_calloc(entry->ndir, sizeof(struct db_entry));

	//Actually process
	safe_seekdir(d, dl);
	char *pp;
	long curdir = 0, curfile = 0;
	while((de = safe_readdir(d)) != NULL){
		if(strcmp(de->d_name, ".") == 0
				|| strcmp(de->d_name, "..") == 0)
			continue;

		pp = get_file(p, de->d_name);
		if(stat(pp, &buf) != 0){
			logmsg(warn, "Skipping %s because of an error\n");
			perror("stat");
			continue;
		}
		if(buf.st_dev != dev && get_fix_filesystem()){
			logmsg(debug, "Skipping %s because it is at a different fs\n");
			continue;
		}

		logmsg(debug, "at: %s\n", pp);
		if(S_ISDIR(buf.st_mode)){
			logmsg(debug, "dir: %s\n", pp);
			recurse(pp, dev, &(entry->dirs[curdir++]));
		}
		if(S_ISREG(buf.st_mode)){
			struct db_file *f = &(entry->files[curfile++]);
			f->mtime = buf.st_mtime;
			f->size = buf.st_size;
			process_file(pp, f);
			logmsg(debug, "file: %s\n", pp);
		}
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

void free_dbfile(struct db_file *f)
{
	if(f == NULL)
		return;
}

void free_dbentry(struct db_entry *e)
{
	if(e == NULL)
		return;
	free(e->dir);
	for(long i = 0; i<e->nfile; i++)
		free_dbfile(&(e->files[i]));
	for(long i = 0; i<e->ndir; i++)
		free_dbentry(&(e->dirs[i]));
	free(e->dirs);
	free(e->files);
}

void free_db(struct db *db)
{
	free_dbentry(db->root);
	free(db->root);
	free(db);
}
