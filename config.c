#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "config.h"
#include "util.h"
#include "format.h"
#include "list.h"
#include "exclude.h"
#include "parse.h"
#include "log.h"
#include "search.h"

#ifdef USE_MP3
#include "id3map.h"
#endif

#include "config/search.h"
#include "config/update.h"
#include "config/print.h"

struct mcommand command =
	{ .database    = NULL
	, .config      = NULL
	, .libraryroot = NULL
	, .logfile     = NULL
	, .fmt         = NULL
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
	{"format",      required_argument, 0, 'f'},
#ifdef USE_MP3
	{"id3map",      required_argument, 0, '3'},
#endif
	{0, 0, 0, 0}
};

static const char *optstring = \
	"+"   \
	"c:"  \
	"d:"  \
	"h::" \
	"f:"  \
	"l:"  \
	"r:"  \
	"s"   \
	"v"   \
	"V"   \
	"3:"  \
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
			"  -f,--format      FMT    Format the output with FMT\n"
#ifdef USE_MP3
			"\n"
			"MP3 specific options\n"
			"  -3,--id3map    ID:KEY   Add ID:KEY to the id3map\n"
#endif
			"\n"
			"Commands (default: update)\n"
			"  print                   Print the database\n"
			"  update                  Update the database\n"
			"  search                  Search the database\n"
			, arg0);
	} else if (strcmp(cmd, "print") == 0) {
		print_usage(out, arg0);
	} else if (strcmp(cmd, "update") == 0) {
		update_usage(out, arg0);
	} else if (strcmp(cmd, "search") == 0) {
		search_usage(out, arg0);
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
		case 'f':
			logmsg(debug, "Output format: %s\n", optarg);
			fmt_free(command.fmt);
			command.fmt = parse_fmt_atoms(optarg);
			break;
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
		argc -= optind;
		argv += optind;
		optind = 0;
		logmsg(debug, "Positional arguments\n");
		if (strcmp(argv[optind], "print") == 0) {
			command.command = c_print;
			print_cli(argc, argv, argv[0]);
		} else if (strcmp(argv[optind], "update") == 0) {
			command.command = c_update;
			update_cli(argc, argv, argv[0]);
		} else if (strcmp(argv[optind], "search") == 0) {
			command.command = c_search;
			search_cli(argc, argv, argv[0]);
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
			ASSIGNFREE(command.database, resolve_tilde(v));
		} else if (strcmp("library", k) == 0){
			logmsg(debug, "Set libraryroot to: %s\n", v);
			ASSIGNFREE(command.libraryroot, resolve_tilde(v));
#ifdef USE_MP3
		} else if (strcmp("id3mapping", k) == 0){
			logmsg(debug, "Parsing id3map entr{y,ies}\n", v);
			char *tok;
			tok = strtok(v, ",");
			while(tok != NULL){
				id3map_add_from_string(trim(tok));
				tok = strtok(NULL, ",");
			}
#endif
		} else if (strcmp("exclude", k) == 0){
			logmsg(debug, "Parsing exclude pattern\n", v);
			exclude_add(safe_strdup(v));
		} else if (strcmp("searchkeys", k) == 0){
			logmsg(debug, "Parsing search keys\n", v);
			char *tok;
			tok = strtok(v, ",");
			while(tok != NULL){
				search_key_add(safe_strdup(trim(tok)));
				tok = strtok(NULL, ",");
			}
		} else if (strcmp("format", k) == 0){
			logmsg(debug, "Set output format to: %s\n", v);
			fmt_free(command.fmt);
			command.fmt = parse_fmt_atoms(v);
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
	fmt_free(command.fmt);

#ifdef USE_MP3
	id3map_free();
#endif

	search_key_free();
}
