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
	struct db *r = safe_malloc(sizeof(struct db));
	r->last_modified = safe_time(&r->initialized);
	r->version = M_DB_VERSION;
	r->rootpath = safe_strdup(get_libraryroot());
	//Parse root
	r->root = safe_calloc(sizeof(struct db_entry), 1);
	r->root->dir = safe_strdup("/");
	return r;
}

void parse_db_file(FILE *f, struct db_file *r)
{
	logmsg(debug, "Parse db_file\n");
	r->path = parse_string(f);
	r->mtime = parse_int64(f);
	r->size = parse_int64(f);
	r->tags = NULL;
	long ntags = parse_int64(f);
	if(ntags == 0){
		logmsg(debug, "Parsed non music db_file\n");
		return;
	}
	r->tags = safe_malloc(sizeof(struct db_tags));
	r->tags->ntags = ntags;
	r->tags->keys = safe_malloc(ntags*sizeof(char *));
	r->tags->values = safe_malloc(ntags*sizeof(char *));
	for(long i = 0; i<ntags; i++){
		r->tags->keys[i] = parse_string(f);
		r->tags->values[i] = parse_string(f);
	}
	logmsg(debug, "Parsed db_file with %lu tags: %s\n", ntags, r->path);
}

void parse_db_entry(FILE *f, struct db_entry *r)
{
	logmsg(debug, "Parse db_entry\n");
	r->dir = parse_string(f);
	r->nfile = parse_int64(f);
	r->ndir = parse_int64(f);
	r->files = safe_calloc(r->nfile, sizeof(struct db_file));
	r->dirs = safe_calloc(r->ndir, sizeof(struct db_entry));
	for(long i = 0; i<r->nfile; i++)
		parse_db_file(f, &r->files[i]);
	for(long i = 0; i<r->ndir; i++)
		parse_db_entry(f, &r->dirs[i]);
	logmsg(debug, "Parsed db_entry: %s\n", r->dir);
}

void write_db_file(FILE *f, struct db_file *e)
{
	logmsg(debug, "Write db_file: %s\n", e->path);
	write_string(f, e->path);
	write_int64(f, e->mtime);
	write_int64(f, e->size);
	if(e->tags == NULL){
		write_int64(f, 0);
		logmsg(debug, "Written non music db_file\n");
		return;
	}
	write_int64(f, e->tags->ntags);
	for(long i = 0; i<e->tags->ntags; i++){
		write_string(f, e->tags->keys[i]);
		write_string(f, e->tags->values[i]);
	}
	logmsg(debug, "Written db_file with %lu tags\n", e->tags->ntags);
}

void write_db_entry(FILE *f, struct db_entry *e)
{
	logmsg(debug, "Write db_entry: %s\n", e->dir);
	write_string(f, e->dir);
	write_int64(f, e->nfile);
	write_int64(f, e->ndir);
	for(long i = 0; i<e->nfile; i++)
		write_db_file(f, &e->files[i]);
	for(long i = 0; i<e->ndir; i++)
		write_db_entry(f, &e->dirs[i]);
}

struct db *load_db(char *path)
{
	logmsg(debug, "Loading db at: %s\n", path);
	struct db *db = safe_malloc(sizeof(struct db));
	FILE *f = safe_fopen(path, "r");
	db->version = parse_int64(f);
	db->initialized = parse_int64(f);
	db->last_modified = parse_int64(f);
	db->rootpath = parse_string(f);
	//Parse root
	db->root = safe_malloc(sizeof(struct db_entry));
	parse_db_entry(f, db->root);
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
	write_string(f, db->rootpath);
	write_db_entry(f, db->root);
	safe_fclose(f);
}

void recurse(char *p, dev_t dev, struct db_entry *entry)
{
	bool update = entry->files != NULL || entry->dirs != NULL;
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
	long curdir = 0, curfile = 0;
	struct dirent *de;
	char *pp;
	while((de = safe_readdir(d)) != NULL){
		if(strcmp(de->d_name, ".") == 0
				|| strcmp(de->d_name, "..") == 0)
			continue;
		pp = get_file(p, de->d_name);
		if(stat(pp, &buf) != 0){
			logmsg(warn, "Skipping %s because of an error\n");
			perror("stat");
			free(pp);
			continue;
		}
		if(buf.st_dev != dev && get_fix_filesystem()){
			logmsg(debug, "Skipping %s, it is at a different fs\n", pp);
			free(pp);
			continue;
		}
		if(S_ISDIR(buf.st_mode))
			curdir++;
		if(S_ISREG(buf.st_mode))
			curfile++;
		free(pp);
	}

	//Check if we already have data
	if(update){
		logmsg(debug, "This db already contains stuff for this dir\n");
		return;
	// We don't already have data on this
	} else {
		entry->nfile = curfile;
		entry->ndir = curdir;
		//Allocate array
		entry->dirs = safe_calloc(entry->ndir, sizeof(struct db_entry));
		entry->files = safe_calloc(entry->nfile, sizeof(struct db_file));
	}

	logmsg(debug, "%s contains %lu dirs and %lu files\n",
		p, entry->ndir, entry->nfile);

	//Actually process
	safe_seekdir(d, dl);
	curdir = 0;
	curfile = 0;
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
			logmsg(info, "Skipping %s, it is at a different fs\n",
				de->d_name);
			continue;
		}

		if(S_ISDIR(buf.st_mode)){
			//Find matching entry
			if(update){

			} else {
				logmsg(debug, "dir: %s\n", pp);
				struct db_entry *e = &(entry->dirs[curdir++]);
				e->dir = safe_strdup(de->d_name);
				recurse(pp, dev, e);
			}
		}
		if(S_ISREG(buf.st_mode)){
			//Find matching entry
			if(update){

			} else {
				logmsg(debug, "file: %s\n", pp);
				struct db_file *f = &(entry->files[curfile++]);
				f->path = safe_strdup(de->d_name);
				f->mtime = buf.st_mtime;
				f->size = buf.st_size;
				process_file(pp, f);
			}
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

	if(strcmp(db->rootpath, get_libraryroot()) != 0){
		logmsg(warn, "This db has a different root. Aborting\n");
		logmsg(warn, "dbroot: %s, libraryroot: %s\n",
			db->rootpath, get_libraryroot());
		return;
	}

	recurse(get_libraryroot(), buf.st_dev, db->root);
}

struct db *get_db(char *path)
{
	logmsg(debug, "Db requested at: %s\n", path);
	if(path_exists(path)){
		logmsg(debug, "Path exists\n");
		if(get_force_reread()){
			logmsg(debug, "Forcing a new db anyways\n");
			return init_db();
		}
		return load_db(path);
	} else {
		return init_db();
	}
}

void print_db_entry(int indent, struct db_entry *e, FILE *f)
{
	for(int j = 0; j<indent; j++)
		safe_fputs("  ", f);
	safe_fprintf(f, "- %s\n", e->dir);
	for(long i = 0; i<e->ndir; i++)
		print_db_entry(indent+1, &(e->dirs[i]), f);
	for(long i = 0; i<e->nfile; i++){
		if(e->files[i].tags == NULL)
			continue;
		for(int j = 0; j<indent+1; j++)
			safe_fputs("  ", f);
		safe_fprintf(f, "| %s\n", e->files[i].path);
	}

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

	safe_fprintf(f, "Root: : %s\n", db->rootpath);

	print_db_entry(0, db->root, f);
}

void free_dbentry(struct db_entry e)
{
	for(long i = 0; i<e.nfile; i++)
		free_file(e.files[i]);
	for(long i = 0; i<e.ndir; i++){
		free_dbentry(e.dirs[i]);
	}
	safe_free(3, e.dir, e.dirs, e.files);
}

void free_db(struct db *db)
{
	free_dbentry(*db->root);
	safe_free(3, db->rootpath, db->root, db);
}
