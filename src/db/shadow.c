#include <stdint.h>
#include <stdbool.h>

#include "shadow.h"
#include "../db.h"
#include "../list.h"
#include "../format.h"
#include "../log.h"
#include "../util.h"

struct shadow_dbl {
	uint64_t numkeys;
	struct listitem *keys;
	uint64_t num;
	struct listitem *forks;
};

struct shadow_db_forkl {
	char *value;
	bool isfork;
	uint64_t num;
	union {
		struct listitem *forks;
		struct listitem *leafs;
	} data;
};

bool pred(void *a, void *b)
{
	return strcmp(
		((struct shadow_db_forkl *)a)->value,
		((struct shadow_db_forkl *)b)->value) == 0;
}

void *index_db_fun(void *st, struct db_file *f)
{
	struct shadow_dbl *sdbl = (struct shadow_dbl *)st;
	for(uint16_t i = 0; i<sdbl->numkeys; i++){
		char *value = sformat(&sdbl->keys[i], f);
		void *item = list_find(sdbl->forks, (void *)value, pred, NULL);
		if(item == NULL){
			//append
		} else {
			//don't append
		}

	}

	return (void *)sdbl;
}

struct shadow_db *index_db(struct db *db, struct listitem *keys)
{
//	struct shadow_db *sdb = safe_malloc(sizeof(struct shadow_db));
//	sdb->keys = list_to_array(keys, &(sdb->numkeys));
//	logmsg(warn, "%lu keys\n", sdb->numkeys);
//	logmsg(debug, "Index the database\n");
//
//	//Make an intermediate linked list tree
//	struct shadow_dbl *sdbl = safe_malloc(sizeof(struct shadow_dbl));
//	sdbl->num = 0;
//	sdbl->forks = NULL;
//	sdbl->numkeys = sdb->numkeys;
//	sdbl->keys = sdb->keys;
//
//	sdbl = db_iterate(db->root, sdbl, &index_db_fun);

	//Convert to array

	return NULL;
}

void free_db_index(struct shadow_db *sdb)
{
//	safe_free(2, sdb->keys, sdb);
}
