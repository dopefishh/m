#include <stdio.h>
#include <getopt.h>

#include "../config.h"
#include "../util.h"

void search_usage(FILE *out, char *arg0){
	fprintf(out,
		"Usage: %s [...] search [OPTS]\n"
		"\n"
		"Options:\n"
		"  -h,--help               Show this help\n"
		, arg0);
}

static struct option search_lopts[] =
{
	{"help",        no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static const char *search_optstring = "h";

void search_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, search_optstring, search_lopts, &oi)) != -1){
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

