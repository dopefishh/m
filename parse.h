#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

uint64_t parse_int64(FILE *f);
void write_int64(FILE *f, uint64_t i);

char *get_line(FILE *f);

#endif
