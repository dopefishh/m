#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "config.h"
#include "util.h"
#include "exclude.h"
#include "parse.h"
#include "log.h"

#ifdef USE_MP3
#include "id3map.h"
#endif

#define ASSIGNFREE(a, v) {free(a); a = v;}

struct mcommand command =
	{ .database    = NULL
	, .config      = NULL
	, .libraryroot = NULL
	, .logfile     = NULL
#ifdef USE_MP3
	, .id3mapping  = NULL
#endif
	, .command     = c_print
	};

static struct option lopts[] =
{
	{"verbose",     no_argument,       0, 'v'},
	{"silent",      no_argument,       0, 's'},
	{"help",        optional_argument, 0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"config",      required_argument, 0, 'c'},
	{"libraryroot", no_argument,       0, 'r'},
	{"db",          required_argument, 0, 'd'},
	{"exclude",     required_argument, 0, 'e'},
#ifdef USE_MP3
	{"id3map",      required_argument, 0, '3'},
#endif
	{0, 0, 0, 0}
};

static const char *optstring = \
	"+"    \
	"c:"   \
	"d:"   \
	"h::"  \
	"l:"   \
	"r:"   \
	"s"    \
	"v"    \
	"V"    \
	"3:"   \
	"e:";

void version(FILE *out)
{
	safe_fputs("``m'' Version 0.1\n", out);
}

void usage(char *cmd, FILE *out, char *arg0)
{
	version(out);
	if(cmd == NULL){
		fprintf(out,
			"Usage: %s [OPTS] [COMMAND [COMMANDOPTS]]\n"
			"\n"
			"Options:\n"
			"  -v,--verbose            Increase verbosity\n"
			"  -s,--silent             Decrease verbosity\n"
			"  -h,--help     [COMMAND] Print this help or the command specific\n"
			"  -l,--log         FILE   Log to FILE instead of stdout\n"
			"  --version               Print the version\n"
			"\n"
			"  -c,--config      FILE   Use the specified config\n"
			"  -d,--db          FILE   Use the specified database\n"
			"  -r,--libraryroot FILE   User FILE as the database root\n\n"
			"  -e,--exclude     GLOB   Add GLOB to the exclusion list\n"
#ifdef USE_MP3
			"\n"
			"MP3 specific options\n"
			"  -3,--id3map    ID:KEY   Add ID:KEY to the id3map\n"
#endif
			"\n"
			"Commands (default: update)\n"
			"  print                   Print the database\n"
			"  update                  Update the database\n"
			, arg0);
	} else if (strcmp(cmd, "print") == 0) {
		fprintf(out,
			"Usage: %s [...] print [OPTS]\n"
			"\n"
			"Options:\n"
			, arg0);
	} else if (strcmp(cmd, "update") == 0) {
		fprintf(out,
			"Usage: %s [...] update [OPTS]\n"
			"\n"
			"Options:\n"
			"  -f,--force              Force reread the entire database\n"
			"  -x,--filesystem         Stay within one filesystem\n"
			, arg0);
	}
}

static struct option print_lopts[] =
{
	{"help",        no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static const char *print_optstring = "+h:";

void parse_print_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, print_optstring, print_lopts, &oi)) != -1){
		switch (c) {
		case 'h':
			usage("print", stdout, argv[0]);
			exit(EXIT_SUCCESS);
		default:
			usage(NULL, stderr, argv[0]);
			die("");
		}
	}
}

static struct option update_lopts[] =
{
	{"force",       no_argument, 0, 'f'},
	{"filesystem",  no_argument, 0, 'x'},
	{"help",        no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static const char *update_optstring = "+h:fx";

void parse_update_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, update_optstring, update_lopts, &oi)) != -1){
		switch (c) {
		case 'f':
			command.fields.update_opts.force_update = true;
			break;
		case 'x':
			command.fields.update_opts.fix_filesystem = true;
			break;
		case 'h':
			usage("print", stdout, argv[0]);
			exit(EXIT_SUCCESS);
		default:
			usage(NULL, stderr, argv[0]);
			die("");
		}
	}
}

void parse_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, optstring, lopts, &oi)) != -1){
		switch (c) {
		case 'c':
			logmsg(debug, "Set config location: %s\n", optarg);
			ASSIGNFREE(command.config, resolve_tilde(optarg));
			break;
		case 'd':
			logmsg(debug, "DB location: %s\n", optarg);
			free(command.database);
			ASSIGNFREE(command.database,
				rtrimc(resolve_tilde(optarg), '/'));
			break;
		case 'V':
			version(stdout);
			exit(EXIT_SUCCESS);
		case 'h':
			printf("optarg: %s\n", optarg);
			usage(optarg, stdout, argv[0]);
			exit(EXIT_SUCCESS);
		case 'l':
			ASSIGNFREE(command.logfile, resolve_tilde(optarg));
			logmsg(debug, "Log output set to: %s\n", optarg);
			break;
		case 'r':
			logmsg(debug, "Library root set to %s\n", optarg);
			ASSIGNFREE(command.libraryroot, resolve_tilde(optarg));
			break;
		case 's':
			decrease_loglevel();
			logmsg(debug, "Decreased loglevel\n");
			break;
		case 'v':
			increase_loglevel();
			logmsg(debug, "Increased loglevel\n");
			break;
		case 'e':
			exclude_add(safe_strdup(optarg));
			logmsg(debug, "Exclusion pattern added\n");
			break;
#ifdef USE_MP3
		case '3':
			id3map_add_from_string(optarg);
			logmsg(debug, "Id3map mapping added\n");
			break;
#endif
		default:
			usage(NULL, stderr, argv[0]);
			die("");
		}
	}

	if (optind < argc) {
		logmsg(debug, "Positional arguments\n");
		if (strcmp(argv[optind], "print") == 0) {
			command.command = c_print;
			parse_print_cli(optind, argv);
		} else if (strcmp(argv[optind], "update") == 0) {
			command.command = c_update;
			parse_update_cli(optind, argv);
		} else {
			logmsg(warn, "Unknown command: %s\n", argv[optind]);
			usage(NULL, stderr, argv[0]);
			die("");
		}
	} else {
		logmsg(debug, "No positional arguments\n");
	}
}

void parse_config()
{
	logmsg(debug, "Going to parse config at: %s\n", command.config);
	if(!path_exists(command.config)){
		logmsg(info, "No config file found at: %s. Defaults used.\n",
			command.config);
		return;
	}

	FILE *f = safe_fopen(command.config, "r");
	char *l = NULL, *k, *v, *t;
	while(!feof(f)){
		free(l);
		l = get_line(f);

		//Check for comment
		t = strchr(l, '#');
		if(t != NULL)
			t[0] = '\0';

		if(strlen(l) == 0)
			continue;

		v = strchr(l, '=');
		if(v == NULL){
			logmsg(warn, "Malformed config line, no '=' found\n");
			continue;
		}

		v++[0] = '\0';
		k = trim(l);
		v = trim(v);

		if (strcmp("database", k) == 0){
			logmsg(debug, "Set database to: %s\n", v);
			free(command.database);
			command.database = resolve_tilde(v);
		} else if (strcmp("library", k) == 0){
			logmsg(debug, "Set libraryroot to: %s\n", v);
			free(command.libraryroot);
			command.libraryroot = resolve_tilde(v);
#ifdef USE_MP3
		} else if (strcmp("id3mapping", k) == 0){
			logmsg(debug, "Parsing id3map entr{y,ies}\n", v);
			char *tok;
			tok = strtok(v, ",");
			while(tok != NULL){
				id3map_add_from_string(tok);
				tok = strtok(NULL, ",");
			}
#endif
		} else if (strcmp("exclude", k) == 0){
			logmsg(debug, "Parsing exclude pattern\n", v);
			exclude_add(safe_strdup(v));
		} else {
			logmsg(warn, "Unknown config line: %s\n", k);
		}
	}
	free(l);

	safe_fclose(f);
}

void free_config()
{
	safe_free(3, command.database, command.libraryroot, command.config);
	exclude_free();

#ifdef USE_MP3
	id3map_free();
#endif
}
