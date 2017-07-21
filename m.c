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

	//Load database and possibly update
	struct db *db = get_db(get_database());
	if(!get_dont_update()){
		logmsg(info, "Updating db\n");
		update_db(db);
	} else {
		logmsg(info, "Not updating db\n");
	}

	//Save database
//	print_db(db, stdout);
	save_db(db, get_database());

	//Free up pointers
	free_db(db);
	free_config();
	return EXIT_SUCCESS;
}
