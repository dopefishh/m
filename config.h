#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>

#define ENTRYH(type, name) \
	void set_##name(type n);\
	type get_##name()

enum command {c_print,c_update,c_search};

struct mcommand {
	char *database;
	char *config;
	char *libraryroot;
	char *logfile;
#ifdef USE_MP3
	char *id3mapping;
#endif
	enum command command;
	union {
		struct {
			bool force_update;
			bool fix_filesystem;
		} update_opts;
		struct {
			char *q;
		} search_opts;
	} fields;
} command;

void usage(char *cmd, FILE *out, char *arg0);
void parse_config();
void parse_cli(int argc, char **argv);
void free_config();

#ifdef USE_MP3
void id3map_add(char *, char *);
char *id3map_get(char *);
void id3map_free();
#endif

#endif
