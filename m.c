#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "log.h"
#include "util.h"
#include "db.h"
#include "xdg.h"
#include "config.h"


int main(int argc, char **argv)
{
	//Initialize
	init_logging();

	//Set defaults
	set_config(get_config_path());
	char *d = get_data_dir();
	set_database(get_file(d, "db"));
	free(d);
	d = resolve_tilde("~/Music");
	set_libraryroot(safe_getenv("XDG_MUSIC_DIR", d));
	free(d);

	//Gather settings
	parse_cli(argc, argv);
	parse_config();

	//Load database
	struct db *db = get_db(get_database());

	//Update db
	update_db(db);

	//Save database
	save_db(db, get_database());

	//Free up pointers
	free_db(db);
	free(get_database());
	free(get_config());
	return EXIT_SUCCESS;
}
