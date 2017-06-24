#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "log.h"
#include "util.h"
#include "db.h"
#include "xdg.h"
#include "config.h"

static struct option long_options[] =
{
	/* These options set a flag. */
	{"verbose", no_argument,       0, 'v'},
	{"help",    no_argument,       0, 'h'},
	{"version", no_argument,       0, 'e'},
	/* These options don’t set a flag.
	We distinguish them by their indices. */
	{"config",  required_argument, 0, 'c'},
	{"db",      required_argument, 0, 'd'},
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
		"  -c,--config FILE Use the specified config\n"
		"  -d,--db     FILE Use the specified database\n"
		"  -f,--force       Force reread the entire database\n"
		"  -l,--log    FILE Log to FILE instead of stdout\n"
		, arg0);
}


int main(int argc, char **argv)
{
	int option_index = 0;
	int c;

	init_logging();

	//Set defaults
	set_config(get_config_path());
	char *d = get_data_dir();
	set_database(get_file(d, "db"));
	free(d);

	while((c = getopt_long(argc, argv,
			"vshl:d:c:", long_options, &option_index)) != -1){
		switch (c) {
		case 'c':
			logmsg(debug, "Set config location: %s\n", optarg);
			set_config(safe_strdup(optarg));
			break;
		case 'd':
			logmsg(debug, "DB location: %s\n", optarg);
			set_database(safe_strdup(optarg));
			break;
		case 'e':
			version(stdout);
			return EXIT_SUCCESS;
		case 'h':
			usage(stdout, argv[0]);
			return EXIT_SUCCESS;
		case 'l':
			set_logfile(optarg);
			logmsg(debug, "Log output set to: %s\n", optarg);
			break;
		case 's':
			decrease_loglevel();
			logmsg(debug, "Decreased loglevel\n");
			break;
		case 'v':
			increase_loglevel();
			logmsg(debug, "Increased loglevel\n");
			break;
		default:
			usage(stderr, argv[0]);
			return EXIT_FAILURE;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Positional arguments not allowed\n");
		usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	struct db *db = get_db(get_database());
	print_db(db, stdout);
	save_db(db, safe_strdup(get_database()));

	return EXIT_SUCCESS;
}
