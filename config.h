#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define ENTRYH(type, name) \
	void set_##name(type n);\
	type get_##name()

enum command {c_print,c_update,c_search};

struct mcommand {
	enum command command;
	union {
		struct {
			char *q;
		} search_opts;
		struct {
			bool force_update;
		} update_opts;
	} fields;
};

void parse_config();
void parse_cli(int argc, char **argv);
void free_config();

ENTRYH(char *, database);
ENTRYH(char *, config);
ENTRYH(char *, libraryroot);
ENTRYH(struct mcommand, command);
ENTRYH(bool, force_reread);
ENTRYH(bool, dont_update);
ENTRYH(bool, fix_filesystem);

#ifdef USE_MP3
ENTRYH(char *, id3mapping);
void id3map_add(char *, char *);
char *id3map_get(char *);
void id3map_free();
#endif

#endif
