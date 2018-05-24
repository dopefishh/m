#include <stdio.h>
#include <getopt.h>

#include "../config.h"
#include "../util.h"

void update_usage(FILE *out, char *arg0)
{
	fprintf(out,
		"Usage: %s [...] update [OPTS]\n"
		"\n"
		"Options:\n"
		"  -h,--help               Show this help\n"
		"  -f,--force              Force reread the entire database\n"
		"  -v,--verbose            Write the database in verbose format\n"
		"  -x,--filesystem         Stay within one filesystem\n"
		, arg0);
}

static struct option update_lopts[] =
{
	{"force",       no_argument, 0, 'f'},
	{"filesystem",  no_argument, 0, 'x'},
	{"verbose",     no_argument, 0, 'v'},
	{"help",        no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static const char *update_optstring = "hfxv";

void update_cli(int argc, char **argv, char *argv0)
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
		case 'v':
			command.fields.update_opts.verbose = true;
			break;
		case 'h':
			usage("update", stdout, argv0);
			exit(EXIT_SUCCESS);
		default:
			usage(NULL, stderr, argv0);
			die("");
		}
	}
}
