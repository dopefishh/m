#ifndef FILE_H
#define FILE_H

#include "db.h"

void process_file(char *p, struct db_file *f);
void free_file(struct db_file *f);

#endif
