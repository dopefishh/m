#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#include "log.h"
#include "exclude.h"
#include "config.h"
#include "parse.h"
#include "util.h"
#include "db.h"
#include "file.h"

#define M_DB_VERSION 1ull
void free_files(struct db_file *f, int nfile);
void free_dirs(struct db_entry *d, int ndir);

struct intlist {
	int value;
	struct intlist *next;
};

//Comparison functions
int db_file_cmp(const void *a1, const void *a2)
{
	return strcmp(((struct db_file *) a1)->path,
		((struct db_file *) a2)->path);
}

int db_entry_cmp(const void *a1, const void *a2)
{
	return strcmp(((struct db_entry *) a1)->dir,
		((struct db_entry *) a2)->dir);
}

struct db *init_db()
{
	logmsg(debug, "Initializing db\n");
	struct db *r = safe_malloc(sizeof(struct db));
	r->last_modified = safe_time(&r->initialized);
	r->version = M_DB_VERSION;
	r->rootpath = safe_strdup(command.libraryroot);
	//Parse root
	r->root = safe_calloc(sizeof(struct db_entry), 1);
	r->root->dir = safe_strdup("/");
	return r;
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

void recurse(char *rp, dev_t dev, struct db_entry *entry)
{
	char *p = realpath(rp, NULL);
	//Go through exclude list to see if we need to skip it
	if(exclude(rp)){
		logmsg(warn, "Skipping %s because of an exclusion pattern\n", p);
		free(p);
		return;
	}

	//Save old values
	struct db_file *oldfs = entry->files;
	struct db_entry *oldds = entry->dirs;
	uint64_t oldnf = oldfs == NULL ? 0 : entry->nfile;
	uint64_t oldnd = oldds == NULL ? 0 : entry->ndir;
	logmsg(debug, "Recursing in %s, with %d %d olds\n", p, oldnf, oldnd);

	//Check permissions
	struct stat buf;
	if(stat(p, &buf) != 0){
		logmsg(warn, "Skipping %s because of an error\n", p);
		perror("stat");
		return;
	}

	//Read directory and count files
	DIR *d = opendir(p);
	if(d == NULL){
		logmsg(warn, "Skipping %s because opendir failed\n", p);
		perror("opendir");
		return;
	}

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
			logmsg(warn, "Skipping %s because of an error\n", pp);
			perror("stat");
			free(pp);
			continue;
		}
		if(buf.st_dev != dev && command.fields.update_opts.fix_filesystem){
			logmsg(debug,
				"Skipping %s, it is at a different fs\n", pp);
			free(pp);
			continue;
		}

		//Check if file already exists
		if(S_ISDIR(buf.st_mode))
			curdir++;
		if(S_ISREG(buf.st_mode))
			curfile++;
		free(pp);
	}

	//Allocate the arrays
	entry->nfile = curfile;
	entry->ndir = curdir;
	entry->dirs = safe_calloc(entry->ndir, sizeof(struct db_entry));
	entry->files = safe_calloc(entry->nfile, sizeof(struct db_file));

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
			logmsg(warn, "Skipping %s because of an error\n", pp);
			perror("stat");
			continue;
		}
		if(buf.st_dev != dev && command.fields.update_opts.fix_filesystem){
			logmsg(info, "Skipping %s, it is at a different fs\n",
				de->d_name);
			continue;
		}

		if(S_ISDIR(buf.st_mode)){
			logmsg(debug, "dirno %d: %s\n", curdir, pp);
			//Find matching entry, this will stop immediatly when
			//there was no old entry
			struct db_entry *e = &entry->dirs[curdir++];
			e->dir = safe_strdup(de->d_name);

			for(uint64_t i = 0; i<oldnd; i++){
				if(strcmp(de->d_name, oldds[i].dir) == 0){
					logmsg(debug, "Found an old dbentry\n");
					e->nfile = oldds[i].nfile;
					e->ndir = oldds[i].ndir;
					e->files = oldds[i].files;
					e->dirs = oldds[i].dirs;
					i = oldnd;
				}
			}

			recurse(pp, dev, e);
		}
		if(S_ISREG(buf.st_mode)){
			logmsg(debug, "file: %s\n", pp);
			struct db_file *f = &entry->files[curfile++];
			f->path = safe_strdup(de->d_name);
			f->mtime = buf.st_mtime;
			f->size = buf.st_size;
			f->tags = NULL;
			for(uint64_t i = 0; i<oldnf; i++){
				if(strcmp(de->d_name, oldfs[i].path) == 0){
					logmsg(debug, "Found an old dbentry\n");
					if(buf.st_mtime <= oldfs[i].mtime){
						f->tags = oldfs[i].tags;
						//Make sure they are not freed
						oldfs[i].tags = NULL;
					}
					i = oldnf;
				}
			}

			if(f->tags == NULL){
				process_file(pp, f);
			}
		}
		free(pp);
	}
	safe_closedir(d);

	//Sort collections for quicker search
	qsort(entry->files, entry->nfile, sizeof(struct db_file), &db_file_cmp);
	qsort(entry->dirs, entry->ndir, sizeof(struct db_entry), &db_entry_cmp);

	//Free stuff up
	for(uint64_t i = 0; i<oldnd; i++)
		free(oldds[i].dir);
	for(uint64_t i = 0; i<oldnf; i++)
		free_file(oldfs[i]);
	safe_free(3, oldfs, oldds, p);
}

void update_db(struct db *db)
{
	logmsg(debug, "Updating db with root: %s\n", command.libraryroot);
	struct stat buf;
	if(stat(command.libraryroot, &buf) == -1)
		perrordie("stat");

	if(strcmp(db->rootpath, command.libraryroot) != 0){
		logmsg(warn, "This db has a different root. Aborting\n");
		logmsg(warn, "dbroot: %s, libraryroot: %s\n",
			db->rootpath, command.libraryroot);
		return;
	}

	recurse(command.libraryroot, buf.st_dev, db->root);
}

struct db *get_db(char *path)
{
	logmsg(debug, "Db requested at: %s\n", path);
	if(path_exists(path)){
		logmsg(debug, "Path exists\n");
		if(command.fields.update_opts.force_update){
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
	safe_fprintf(f, "- %s (%dd, %df)\n", e->dir, e->ndir, e->nfile);
	for(uint64_t i = 0; i<e->ndir; i++)
		print_db_entry(indent+1, &(e->dirs[i]), f);
	for(uint64_t i = 0; i<e->nfile; i++){
		if(e->files[i].tags == NULL)
			continue;
		for(int j = 0; j<indent+1; j++)
			safe_fputs("  ", f);
		safe_fprintf(f, "| %s", e->files[i].path);
		if(e->files[i].tags != NULL)
			safe_fprintf(f,
				" with %lu tags\n", e->files[i].tags->ntags);
		else
			safe_fputs("\n", f);
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
	for(uint64_t i = 0; i<e.nfile; i++)
		free_file(e.files[i]);
	for(uint64_t i = 0; i<e.ndir; i++)
		free_dbentry(e.dirs[i]);
	safe_free(3, e.dir, e.files, e.dirs);
}

void free_db(struct db *db)
{
	free_dbentry(*db->root);
	safe_free(3, db->rootpath, db->root, db);
}
