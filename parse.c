#include <stdio.h>
#include <stdint.h>

#include "util.h"
#include "db.h"
#include "log.h"

uint64_t parse_int64(FILE *f)
{
	uint64_t b;
	if(safe_fread((void *)&b, 8, 1, f) != 1)
		die("Expected 64bit Integer");
	return b;
}
char *parse_string(FILE *f)
{
	size_t total = 16, read = 0;
	char *b = safe_malloc(total);
	do {
		if(read >= total)
			b = safe_realloc(b, total*=2);
		b[read++] = safe_getc(f);
		if(b[read-1] == EOF)
			die("Expected string");
	} while(b[read-1] != '\0');
	return b;
}

void write_int64(FILE *f, uint64_t i)
{
	if(safe_fwrite((void *)&i, 8, 1, f) != 1)
		die("Unable to write 64bit Integer");
}

void write_string(FILE *f, char *s)
{
	if(safe_fwrite(s, strlen(s)+1, 1, f) != 1)
		die("Unable to write string");
}

char *get_line(FILE *f)
{
	size_t total = 16, read = 0;
	char *b = NULL;
	while(true){
		b = (char *)safe_realloc(b, total*=2);
		b = b;
		safe_fgets(b+read, total-read, f);
		read = strlen(b);
		if(feof(f)){
			break;
		}
		if(b[read-1] == '\n'){
			if(read > 1 && b[read-2] == '\\'){
				b[read-1] = '\0';
				b[read-2] = '\0';
				read -= 2;
			} else {
				b[read-1] = '\0';
				break;
			}
		}
	}
	return b;
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
