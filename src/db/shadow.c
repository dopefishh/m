#include <stdint.h>
#include <stdbool.h>

#include "shadow.h"
#include "../db.h"
#include "../list.h"
#include "../log.h"
#include "../util.h"

struct shadow_db *index_db(struct db *db, struct listitem *keys)
{
	struct shadow_db *sdb = safe_malloc(sizeof(struct shadow_db));
	sdb->keys = list_to_array(keys, &(sdb->numkeys));
	logmsg(warn, "%lu keys\n", sdb->numkeys);
	logmsg(debug, "Index the database\n");

	//Make an intermediat linked list fork

	(void)db;
	return sdb;
}

void free_db_index(struct shadow_db *sdb)
{
	safe_free(2, sdb->keys, sdb);
}
