#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

uint64_t parse_int64(FILE *f);
char *parse_string(FILE *f);
void write_int64(FILE *f, uint64_t i);
void write_string(FILE *f, char *s);

char *get_line(FILE *f);

#endif
