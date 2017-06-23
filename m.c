#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "util.h"
#include "xdg.h"

static int option_flag;



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
	puts("``m'' Version 0.1");
}

void usage(FILE *out, char *arg0)
{
	version(out);
	fprintf(out,
		"Usage: %s [OPTS]\n"
		"\n"
		"Options:\n"
		"  -v,--verbose     Increase verbosity\n"
		"  -h,--help        Print this help\n"
		"  --version        Print the version\n"
		"\n"
		"  -c,--config FILE Use the specified config\n"
		"  -d,--db     FILE Use the specified database\n"
		, arg0);
}


int main(int argc, char **argv)
{
	int option_index = 0;
	int c;

	while((c = getopt_long(argc, argv,
			"vhd:c:", long_options, &option_index)) != -1){
		switch (c) {
		case 0:
			printf("option %s", long_options[option_index].name);
			printf("flag %d", long_options[option_index].flag);
			if (optarg) {
				printf (" with arg %s", optarg);
			}
			printf("\n");
			break;
		case 'c':
			printf("Config location: %s\n", optarg);
			break;
		case 'd':
			printf("DB location: %s\n", optarg);
			break;
		case 'e':
			version(stdout);
			return EXIT_SUCCESS;
		case 'h':
			usage(stdout, argv[0]);
			return EXIT_SUCCESS;
		case 'v':
			usage(stdout, argv[0]);
			return EXIT_SUCCESS;
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

	return EXIT_SUCCESS;
}
