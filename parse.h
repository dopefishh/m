#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

#include "db.h"

#define LABELED(verbose, indent, label, file, pat, writefun, arg) {\
	if(verbose){\
		for(uint64_t i = indent; i>0; i--)\
			safe_fprintf(f, "\t");\
		safe_fprintf(f, "%s: " pat "\n", label, arg);\
	} else {\
		writefun(f, arg);\
	}\
}
#define LABELEDI(verbose, indent, label, file, arg)\
	LABELED(verbose, indent, label, file, "%llu", write_int64, arg)
#define LABELEDS(verbose, indent, label, file, arg)\
	LABELED(verbose, indent, label, file, "%s", write_string, arg)

#define PARSE(verbose, file, var, parsefun) {\
	if(verbose){\
		char *_l = get_line(f);\
		char *_p = _l;\
		while(*_p++ != ':');\
		_p++;\
		_p[strlen(_p)] = '\0';\
		var = safe_strdup(_p);\
		free(_l);\
		\
	} else {\
		var = parsefun(file);\
	}\
}

uint64_t parse_int64(FILE *f);
char *parse_string(FILE *f);
void write_int64(FILE *f, uint64_t i);
void write_string(FILE *f, char *s);

char *get_line(FILE *f);

// DB stuff
void parse_db_file(FILE *f, struct db_file *r);
void parse_db_entry(FILE *f, struct db_entry *r);
void write_db_file(uint64_t indent, FILE *f, struct db_file *e, bool verbose);
void write_db_entry(uint64_t indent, FILE *f, struct db_entry *e, bool verbose);

#endif
