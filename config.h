#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define ENTRYH(type, name) \
	void set_##name(type n);\
	type get_##name()

void parse_config();
void parse_cli(int argc, char **argv);

ENTRYH(char *, database);
ENTRYH(char *, config);
ENTRYH(char *, libraryroot);
ENTRYH(bool, force_reread);
ENTRYH(bool, dont_update);
ENTRYH(bool, fix_filesystem);

#endif
