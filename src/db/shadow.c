#include <stdint.h>
#include <stdbool.h>

#include "../db.h"
#include "../list.h"
#include "../log.h"

struct shadow_db *index_db(struct db *db, struct listitem *keys)
{
	(void)db;
	(void)keys;
	logmsg(debug, "Index the database\n");
	return NULL;
}
