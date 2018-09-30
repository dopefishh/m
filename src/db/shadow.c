#include <stdint.h>
#include <stdbool.h>

#include "shadow.h"
#include "../db.h"
#include "../list.h"
#include "../format.h"
#include "../log.h"
#include "../util.h"

typedef struct listitem * db_file_list;
typedef struct listitem * shadow_db_forkl_list;

struct shadow_dbl {
	// Number of keys
	uint64_t numkeys;
	// Array of formatlists
	formatting *keys;
	struct shadow_db_forkl *rootfork;
};

struct shadow_db_forkl {
	// Formatted value
	char *value;
	// Is a fork
	bool isfork;
	//isfork ? list of shadow_db_forkl : list of db_file
	union {
		shadow_db_forkl_list forks;
		db_file_list leafs;
	} data;
};

bool pred(void *a, void *b)
{
	logmsg(debug, "cmp '%s' with '%s'\n", a, ((struct shadow_db_forkl *)b)->value);
	return strcmp((char *)a, ((struct shadow_db_forkl *)b)->value) == 0;
}

void log_shadow_db_forkl(enum loglevel l, struct shadow_db_forkl *fork, int indent)
{
	for(int j = 0; j<indent; j++)
		logmsg(l, " ");
	logmsg(l, "value: %s ", fork->value);
	if(fork->isfork){
		logmsg(l, "isfork\n");
		struct listitem *head = fork->data.forks;
		while(head != NULL){
			log_shadow_db_forkl(l, head->value, indent+1);
			head = head->next;
		}
	} else {
		struct listitem *head = fork->data.leafs;
		logmsg(l, "isleafs\n");
		while(head != NULL){
			logmsg(l, "%s ", ((struct db_file *)head->value)->path);
			head = head->next;
		}
		logmsg(l, "\n");
	}
}

void log_shadow_dbl(enum loglevel l, struct shadow_dbl *sdbl)
{
	logmsg(l, "\n\n");
	logmsg(l, "shadowDBL:\nnumkeys: %lu\n", sdbl->numkeys);
	log_shadow_db_forkl(l, sdbl->rootfork, 0);
	logmsg(l, "\n\n");
}

void *index_db_fun(void *st, struct db_file *f)
{
	struct shadow_dbl *sdbl = (struct shadow_dbl *)st;
	logmsg(debug, "\nindex %s, numkeys: %lu\n", f->path, sdbl->numkeys);

	struct shadow_db_forkl *curfork = sdbl->rootfork;
	struct shadow_db_forkl *tfork;
	for(uint64_t i = 0; i < sdbl->numkeys; i++){
		char *value = sformat(sdbl->keys[i], f);
		logmsg(debug, "key[%lu]: %s\n", i, value);
		void *item = list_find(curfork->data.forks, (void *)value, pred, NULL);
		//append
		if(item == NULL){
			//Create new fork
			tfork = safe_malloc(sizeof(struct shadow_db_forkl));
			tfork->value = value;
			tfork->isfork = i + 1 >= sdbl->numkeys;
			tfork->data.forks = NULL;

			//Update forklist
			curfork->data.forks = list_prepend(curfork->data.forks, tfork);
			logmsg(debug, "appended a fork\n");
			curfork = tfork;
		//don't append
		} else {
			curfork = (struct shadow_db_forkl *)item;
			logmsg(debug, "don't append but pick existing fork\n");
		}
	}
	logmsg(debug, "fork to add it to: %s, isfork: %s\n", curfork->value, curfork->isfork ? "true" : "false");
	curfork->isfork = false;
	curfork->data.leafs = list_prepend(curfork->data.leafs, f);
	logmsg(debug, "fork has now %lu leafs\n", list_length(curfork->data.leafs));

	return st;
}

struct shadow_db *index_db(struct db *db, struct listitem *keys)
{
	struct shadow_db *sdb = safe_malloc(sizeof(struct shadow_db));
	sdb->keys = list_to_array(keys, &sdb->numkeys);

	//Make an intermediate linked list tree
	struct shadow_dbl *sdbl = safe_malloc(sizeof(struct shadow_dbl));
	sdbl->numkeys = sdb->numkeys;
	sdbl->keys = sdb->keys;

	//Add root fork
	sdbl->rootfork = safe_malloc(sizeof(struct shadow_db_forkl));
	sdbl->rootfork->value = NULL;
	sdbl->rootfork->isfork = sdbl->numkeys>0;
	sdbl->rootfork->data.forks = NULL;

	logmsg(debug, "numkeys: %lu\n", sdb->numkeys);

	sdbl = db_iterate(db->root, sdbl, &index_db_fun);
	log_shadow_dbl(debug, sdbl);

	//Convert to array

	return NULL;
}

void free_db_index(struct shadow_db *sdb)
{
	(void)sdb;
//	safe_free(2, sdb->keys, sdb);
}
