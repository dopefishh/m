#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "util.h"
#include "parse.h"
#include "log.h"

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

static struct option lopts[] =
{
	{"verbose",     no_argument,       0, 'v'},
	{"silent",      no_argument,       0, 's'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'e'},
	{"config",      required_argument, 0, 'c'},
	{"libraryroot", no_argument,       0, 'r'},
	{"db",          required_argument, 0, 'd'},
	{"force",       no_argument,       0, 'f'},
	{"dontupdate",  no_argument,       0, 'n'},
	{"filesystem",  no_argument,       0, 'x'},
	{0, 0, 0, 0}
};

void version(FILE *out)
{
	safe_fputs("``m'' Version 0.1\n", out);
}

void usage(FILE *out, char *arg0)
{
	version(out);
	fprintf(out,
		"Usage: %s [OPTS]\n"
		"\n"
		"Options:\n"
		"  -v,--verbose     Increase verbosity\n"
		"  -s,--silent      Decrease verbosity\n"
		"  -h,--help        Print this help\n"
		"  --version        Print the version\n"
		"\n"
		"  -c,--config      FILE Use the specified config\n"
		"  -d,--db          FILE Use the specified database\n"
		"  -f,--force            Force reread the entire database\n"
		"  -n,--dontupdate       Don't update the database\n"
		"  -l,--log         FILE Log to FILE instead of stdout\n"
		"  -r,--libraryroot FILE Log to FILE instead of stdout\n"
		"  -x,--filesystem       Stay within one filesystem\n"
		, arg0);
}

void parse_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, "c:d:fhl:nr:svx", lopts, &oi)) != -1){
		switch (c) {
		case 'c':
			logmsg(debug, "Set config location: %s\n", optarg);
			set_config(resolve_tilde(optarg));
			break;
		case 'd':
			logmsg(debug, "DB location: %s\n", optarg);
			set_database(resolve_tilde(optarg));
			break;
		case 'e':
			version(stdout);
			exit(EXIT_SUCCESS);
		case 'f':
			set_force_reread(true);
			break;
		case 'h':
			usage(stdout, argv[0]);
			exit(EXIT_SUCCESS);
		case 'l':
			set_logfile(resolve_tilde(optarg));
			logmsg(debug, "Log output set to: %s\n", optarg);
			break;
		case 'n':
			set_dont_update(true);
			break;
		case 'r':
			logmsg(debug, "Library root set to %s\n", optarg);
			set_libraryroot(resolve_tilde(optarg));
			break;
		case 's':
			decrease_loglevel();
			logmsg(debug, "Decreased loglevel\n");
			break;
		case 'v':
			increase_loglevel();
			logmsg(debug, "Increased loglevel\n");
			break;
		case 'x':
			set_fix_filesystem(true);
			logmsg(debug, "Fixed to one filesystem\n");
			break;
		default:
			usage(stderr, argv[0]);
			die("");
		}
	}

	if (optind < argc) {
		usage(stderr, argv[0]);
		die("Positional arguments not allowed\n");
	}
}

void parse_config()
{
	logmsg(debug, "Going to parse config at: %s\n", get_config());
	if(!path_exists(get_config())){
		logmsg(info, "No config file found at: %s. Defaults used.\n",
			get_config());
		return;
	}

	FILE *f = safe_fopen(get_config(), "r");
	char *l = NULL, *k, *v;
	while(!feof(f)){
		free(l);
		l = get_line(f);
		if((v = strchr(l, '#')) != NULL)
			v[0] = '\0';
		if((v = strchr(l, '=')) == NULL){
			if(strlen(l) > 0)
				logmsg(warn, "Can't parse '%s' in config\n", l);
			continue;
		}

		k = trim(l);
		v = trim(++v);

		if (strcmp("database", k) == 0){
			logmsg(debug, "Set database to: %s\n", v);
			set_database(v);
		} else {
			logmsg(warn, "Unknown config line: %s\n", k);
		}
	}
	free(l);

	safe_fclose(f);
}

ENTRY(char *, database, FREE, NULL);
ENTRY(char *, config, FREE, NULL);
ENTRY(char *, libraryroot, FREE, NULL);
ENTRY(bool, force_reread, NOT_FREE, false);
ENTRY(bool, dont_update, NOT_FREE, false);
ENTRY(bool, fix_filesystem, NOT_FREE, false);
