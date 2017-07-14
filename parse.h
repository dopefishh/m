#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

#include "db.h"

uint64_t parse_int64(FILE *f);
char *parse_string(FILE *f);
void write_int64(FILE *f, uint64_t i);
void write_string(FILE *f, char *s);

char *get_line(FILE *f);

// DB stuff
void parse_db_file(FILE *f, struct db_file *r);
void parse_db_entry(FILE *f, struct db_entry *r);
void write_db_file(FILE *f, struct db_file *e);
void write_db_entry(FILE *f, struct db_entry *e);

#endif
