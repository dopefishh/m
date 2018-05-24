#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "config.h"
#include "db.h"
#include "log.h"
#include "search.h"
#include "util.h"
#include "xdg.h"

int main(int argc, char **argv)
{
	//Initialize
	init_logging();

	//Set defaults
	command.config = get_config_path();
	char *d = get_data_dir();
	command.database = get_file(d, "db");
	free(d);
	d = resolve_tilde("~/Music");
	command.libraryroot = safe_getenv("XDG_MUSIC_DIR", d);
	free(d);

	//Gather settings
	parse_cli(argc, argv);
	parse_config();

	//Load database and possibly update
	struct db *db = get_db(command.database);
	switch (command.command)
	{
		case c_print:
			logmsg(info, "Printing db\n");
			print_db(db, stdout);
			break;
		case c_update:
			logmsg(info, "Updating db\n");
			update_db(db);
			save_db(db, command.database, command.fields.update_opts.verbose);
			break;
		case c_search:
			logmsg(info, "Searching db\n");
			search_db(db);
			break;
		default:
			logmsg(warn, "Unknown command\n");
			break;
	}

	//Free up pointers
	free_db(db);
	free_config();
	return EXIT_SUCCESS;
}
