#ifndef FILE_H
#define FILE_H

#include <stdbool.h>

#include "db.h"

void process_file(char *p, struct db_file *f);
void free_file(struct db_file f);

void file_tag_init(struct db_file *f, uint64_t numtags);
void file_tag_insert(struct db_file *f, struct db_tag tag);
bool file_tag_split_eq(char *k, char **key, char **value);

#endif
