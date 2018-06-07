#include <stdio.h>
#include <getopt.h>

#include "../config.h"
#include "../util.h"

void print_usage(FILE *out, char *arg0)
{
	fprintf(out,
		"Usage: %s [...] print [OPTS]\n"
		"\n"
		"Options:\n"
		, arg0);
}

static struct option print_lopts[] =
{
	{"help",        no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static const char *print_optstring = "h";

void print_cli(int argc, char **argv, char *argv0)
{
	int oi = 0;
	int c;
	while((c = getopt_long(argc, argv, print_optstring, print_lopts, &oi)) != -1){
		switch (c) {
		case 'h':
			usage("print", stdout, argv0);
			exit(EXIT_SUCCESS);
		default:
			usage(NULL, stderr, argv0);
			die("");
		}
	}
}
