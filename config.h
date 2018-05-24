#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stdio.h>

#include "list.h"

#define ASSIGNFREE(a, v) {free(a); a = v;}

enum command {c_print,c_update,c_search};

struct mcommand {
	char *database;
	bool verbose_database;
	char *config;
	char *libraryroot;
	char *logfile;
	struct listitem *fmt;
#ifdef USE_MP3
	char *id3mapping;
#endif
	enum command command;
	union {
		struct {
			bool force_update;
			bool fix_filesystem;
			bool verbose;
		} update_opts;
		struct {
			char *query;
			char *grouppattern;
		} search_opts;
	} fields;
} command;

void usage(char *cmd, FILE *out, char *arg0);
void parse_config();
void parse_cli(int argc, char **argv);
void free_config();

#endif
