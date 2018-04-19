#ifndef ID3MAP_H
#define ID3MAP_H

#include <stdbool.h>

void id3map_add_from_string(char *);
void id3map_add(char *, char *, char *);
char *id3map_get(char *, char *);
void id3map_free();

#endif
