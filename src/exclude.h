#ifndef EXCLUDE_H
#define EXCLUDE_H

#include <stdbool.h>

void exclude_add(char *);
void exclude_free();
bool exclude(char *v);

#endif
