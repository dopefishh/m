#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define ENTRYH(type, name) \
	void set_##name(type n);\
	type get_##name()

ENTRYH(char *, database);
ENTRYH(char *, config);
ENTRYH(bool, force_reread);

#endif
