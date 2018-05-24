#include <stdio.h>
#include <stdint.h>

#include "util.h"
#include "db.h"
#include "log.h"

#define CHUNKSIZE 128

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
	size_t total = CHUNKSIZE, read = 0;
	char *b = safe_malloc(total);
	b[0] = '\0';
	while(true){
		if(total > 1024*1024*1024)
			die("Too big of a line\n");

		if(read >= total - 1)
			b = safe_realloc(b, total+=CHUNKSIZE);

		safe_fgets(b+read, total-read, f);
		read += strlen(b+read);

		if(feof(f))
			break;

		if(b[read-1] == '\n'){
			b[--read] = '\0';
			if(read > 0 && b[read-1] == '\\')
				b[--read] = '\0';
			else
				break;
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
	r->ntags = parse_int64(f);
	if(r->ntags == 0){
		logmsg(debug, "Parsed non music db_file\n");
		return;
	} else {
		logmsg(debug, "Going to parse %lu tags\n", r->ntags);
	}
	r->tags = safe_malloc(r->ntags*sizeof(struct db_tag));
	for(uint64_t i = 0; i<r->ntags; i++){
		r->tags[i].key = parse_string(f);
		r->tags[i].value = parse_string(f);
	}
	logmsg(debug, "Parsed db_file with %lu tags: %s\n", r->ntags, r->path);
}

void parse_db_entry(FILE *f, struct db_entry *r)
{
	logmsg(debug, "Parse db_entry\n");
	r->dir = parse_string(f);
	r->nfile = parse_int64(f);
	r->ndir = parse_int64(f);
	r->files = safe_calloc(r->nfile, sizeof(struct db_file));
	r->dirs = safe_calloc(r->ndir, sizeof(struct db_entry));
	for(uint64_t i = 0; i<r->nfile; i++)
		parse_db_file(f, &r->files[i]);
	for(uint64_t i = 0; i<r->ndir; i++)
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
	write_int64(f, e->ntags);
	for(uint64_t i = 0; i<e->ntags; i++){
		write_string(f, e->tags[i].key);
		write_string(f, e->tags[i].value);
	}
	logmsg(debug, "Written db_file with %lu tags\n", e->ntags);
}

void write_db_entry(FILE *f, struct db_entry *e)
{
	logmsg(debug, "Write db_entry: %s\n", e->dir);
	write_string(f, e->dir);
	write_int64(f, e->nfile);
	write_int64(f, e->ndir);
	for(uint64_t i = 0; i<e->nfile; i++)
		write_db_file(f, &e->files[i]);
	for(uint64_t i = 0; i<e->ndir; i++)
		write_db_entry(f, &e->dirs[i]);
}
