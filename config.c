#include <stdlib.h>

#include "util.h"

#define FREE(i) free(i);
#define NOT_FREE(i)
#define ENTRY(type, name, free, initial) \
	type name = initial;\
	void set_##name(type n) {\
		free(name)\
		name = n;\
	}; \
	type get_##name() {\
		return name;\
	}


ENTRY(char *, database, FREE, NULL);
ENTRY(char *, config, FREE, NULL);
ENTRY(bool, force_reread, NOT_FREE, false);
