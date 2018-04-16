#include <stdio.h>
#include <getopt.h>

#include "../config.h"
#include "../util.h"
#include "../log.h"

void search_usage(FILE *out, char *arg0){
	fprintf(out,
		"Usage: %s [...] search [OPTS]\n"
		"\n"
		"Options:\n"
		"  -g,--group TAGPATTERN   Group the output results by the TAGPATTERN\n"
		"  -h,--help               Show this help\n"
		, arg0);
}

static struct option search_lopts[] =
{
	{"group",       required_argument, 0, 'g'},
	{"help",        no_argument,       0, 'h'},
	{0, 0, 0, 0}
};

static const char *search_optstring = "hg:";

void search_cli(int argc, char **argv)
{
	int oi = 0;
	int c;
	command.fields.search_opts.grouppattern = NULL;
	command.fields.search_opts.query = NULL;
	while((c = getopt_long(argc, argv, search_optstring, search_lopts, &oi)) != -1){
		switch (c) {
		case 'g':
			ASSIGNFREE(command.fields.search_opts.grouppattern,
				safe_strdup(optarg));
			break;
		case 'h':
			usage("print", stdout, argv[0]);
			exit(EXIT_SUCCESS);
		default:
			usage(NULL, stderr, argv[0]);
			die("");
		}
	}

	if (optind < argc) {
		size_t querylength = 0;
		for(int i = optind; i<argc; i++){
			//Add one for the space
			querylength += strlen(argv[i]) + 1;
		}

		free(command.fields.search_opts.query);
		char *q = (command.fields.search_opts.query = malloc(querylength));

		for(int i = optind; i<argc; i++){
			if(i > optind)
				strcat(q, " ");
			strcat(q, argv[i]);
		}

		optind = 0;
	} else {
		logmsg(warn, "At least one positional argument required\n");
		die("");
	}
}
