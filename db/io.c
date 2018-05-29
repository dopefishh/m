#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "../util.h"
#include "../db.h"
#include "../log.h"

#define serialize(f, indent, pat, label, item) {\
	for(uint64_t i = 0; i<indent; i++)\
		safe_fputc('\t', f);\
	fprintf(f, "%s: " pat "\n", label, item);\
}

static inline void serialize_int(FILE *f, uint64_t indent, char *label, uint64_t i, bool verbose)
{
	if(verbose){
		for(uint64_t i = 0; i<indent; i++)
			safe_fputc('\t', f);
		fprintf(f, "%s: %lu\n", label, i);
	} else {
		if(safe_fwrite((void *)&i, 8, 1, f) != 1){
			die("Unable to write int");
		}
	}
}

static inline void serialize_string(FILE *f, uint64_t indent, char *label, char *s, bool verbose)
{
	if(verbose){
		for(uint64_t i = 0; i<indent; i++)
			safe_fputc('\t', f);
		fprintf(f, "%s: ", label);
		while(*s != '\0'){
			if(*s == '\n')
				safe_fputc('\\', f);
			safe_fputc(*s++, f);
		}
		safe_fputc('\n', f);
	} else if(safe_fwrite(s, strlen(s)+1, 1, f) != 1) {
		die("Unable to write string");
	}
}

#define deserialize(f, line, c) {\
	line = get_line(f);\
	c = strchr(line, ':');\
	if(c == NULL)\
		die("No : found: %s", line);\
	if(strlen(c) == 1)\
		die("No value after the :");\
	c+=2;\
}

static inline uint64_t deserialize_int(FILE *f, bool verbose)
{
	uint64_t b = 0;
	if(verbose){
		char *line, *c;
		deserialize(f, line, c);

		errno = 0;
		b = strtoll(c, NULL, 10);
		if(errno != 0)
			die("Couldn't parse int from: %s\n", c);

		free(line);
	} else if(safe_fread((void *)&b, 8, 1, f) != 1) {
		die("Expected 64bit Integer");
	}
	return b;
}

static inline char *deserialize_string(FILE *f, bool verbose)
{
	char *b;
	if(verbose){
		char *line, *c;
		deserialize(f, line, c);
		b = safe_strdup(c);
		free(line);
	} else {
		size_t total = 16, read = 0;
		b = safe_malloc(total);
		do {
			if(read >= total)
				b = safe_realloc(b, total*=2);
			b[read++] = safe_getc(f);
			if(b[read-1] == EOF)
				die("Expected string");
		} while(b[read-1] != '\0');
	}
	return b;
}

static inline void serialize_db_file(FILE *f, uint64_t indent, struct db_file *e, bool verbose)
{
	serialize_string(f, indent, "path", e->path, verbose);
	serialize_int(f, indent, "mtime", e->mtime, verbose);
	serialize_int(f, indent, "size", e->size, verbose);
	if(e->tags == NULL){
		serialize_int(f, indent, "ntags", 0, verbose);
		return;
	}
	serialize_int(f, indent, "ntags", e->ntags, verbose);
	for(uint64_t i = 0; i<e->ntags; i++){
		serialize_string(f, indent, "key", e->tags[i].key, verbose);
		serialize_string(f, indent, "value", e->tags[i].value, verbose);
	}
}

static inline void serialize_db_entry(FILE *f, uint64_t indent, struct db_entry *e, bool verbose)
{
	serialize_string(f, indent, "dir", e->dir, verbose);
	serialize_int(f, indent, "nfile", e->nfile, verbose);
	serialize_int(f, indent, "ndir", e->ndir, verbose);
	for(uint64_t i = 0; i<e->nfile; i++)
		serialize_db_file(f, indent+1, &e->files[i], verbose);
	for(uint64_t i = 0; i<e->ndir; i++)
		serialize_db_entry(f, indent+1, &e->dirs[i], verbose);
}

void serialize_db(FILE *f, struct db *db, bool verbose)
{
	logmsg(debug, "Serializing the db %s\n", verbose ? "verbosely" : "binary");
	serialize_int(f, 0, "verbose", verbose ? 1 : 0, verbose);
	serialize_int(f, 0, "version", db->version, verbose);
	serialize_int(f, 0, "initialized", db->initialized, verbose);
	serialize_int(f, 0, "last_modified", db->last_modified, verbose);
	serialize_string(f, 0, "root", db->rootpath, verbose);
	serialize_db_entry(f, 0, db->root, verbose);
}

static inline void deserialize_db_file(FILE *f, struct db_file *r, bool verbose)
{
	r->path = deserialize_string(f, verbose);
	r->mtime = deserialize_int(f, verbose);
	r->size = deserialize_int(f, verbose);
	r->tags = NULL;
	r->ntags = deserialize_int(f, verbose);
	if(r->ntags == 0)
		return;
	r->tags = safe_malloc(r->ntags*sizeof(struct db_tag));
	for(uint64_t i = 0; i<r->ntags; i++){
		r->tags[i].key = deserialize_string(f, verbose);
		r->tags[i].value = deserialize_string(f, verbose);
	}
}


static inline void deserialize_db_entry(FILE *f, struct db_entry *r, bool verbose)
{
	r->dir = deserialize_string(f, verbose);
	r->nfile = deserialize_int(f, verbose);
	r->ndir = deserialize_int(f, verbose);
	r->files = safe_calloc(r->nfile, sizeof(struct db_file));
	r->dirs = safe_calloc(r->ndir, sizeof(struct db_entry));
	for(uint64_t i = 0; i<r->nfile; i++)
		deserialize_db_file(f, &r->files[i], verbose);
	for(uint64_t i = 0; i<r->ndir; i++)
		deserialize_db_entry(f, &r->dirs[i], verbose);
}

struct db *deserialize_db(FILE *f)
{
	struct db *db = safe_malloc(sizeof(struct db));
	int v =  safe_getc(f);
	safe_ungetc(v, f);
	bool verbose = v == 'v';
	//Pop the verbose flag
	deserialize_int(f, verbose);

	db->version = deserialize_int(f, verbose);
	db->initialized = deserialize_int(f, verbose);
	db->last_modified = deserialize_int(f, verbose);
	db->rootpath = deserialize_string(f, verbose);
	db->root = safe_malloc(sizeof(struct db_entry));
	deserialize_db_entry(f, db->root, verbose);
	return db;
}
