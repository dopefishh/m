#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define ENTRYH(type, name) \
	void set_##name(type n);\
	type get_##name()

void parse_config();
void parse_cli(int argc, char **argv);
void free_config();

ENTRYH(char *, database);
ENTRYH(char *, config);
ENTRYH(char *, libraryroot);
ENTRYH(bool, force_reread);
ENTRYH(bool, dont_update);
ENTRYH(bool, fix_filesystem);

#ifdef USE_MP3
ENTRYH(char *, id3mapping);
char *id3map_get(char *id);
#endif

#endif
