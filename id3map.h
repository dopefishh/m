#ifndef ID3MAP_H
#define ID3MAP_H

void id3map_parse_conf(char *);
void id3map_add(char *, char *);
char *id3map_get(char *);
void id3map_free();

#endif
