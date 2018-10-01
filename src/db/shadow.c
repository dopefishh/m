#include <stdint.h>
#include <stdbool.h>

#include "shadow.h"
#include "../db.h"
#include "../list.h"
#include "../format.h"
#include "../log.h"
#include "../util.h"

typedef struct listitem db_file_list;
typedef struct listitem shadow_db_forkl_list;

struct shadow_db_forkl {
	// Formatted value
	char *value;
	// Is a fork
	bool isfork;
	//isfork ? list of shadow_db_forkl : list of db_file
	union {
		shadow_db_forkl_list *forks;
		db_file_list *leafs;
	} data;
};

struct shadow_dbl {
	// Number of keys
	uint64_t numkeys;
	// Array of formatlists
	formatting *keys;
	struct shadow_db_forkl root;
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
	log_shadow_db_forkl(l, &sdbl->root, 0);
	logmsg(l, "\n\n");
}

void *index_db_fun(void *st, struct db_file *f)
{
	struct shadow_dbl *sdbl = (struct shadow_dbl *)st;
	logmsg(debug, "\nindex %s, numkeys: %lu\n", f->path, sdbl->numkeys);

	struct shadow_db_forkl *curfork = &sdbl->root;
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

/**
 * Destructively convert a linked fork to an array fork
 *
 */
struct shadow_db_fork *convert_listfork_to_array(struct shadow_db_forkl *lf)
{
	struct shadow_db_fork *f = safe_malloc(sizeof(struct shadow_db_fork));
	f->value = lf->value;
	f->isfork = lf->isfork;
	if(f->isfork){
		list_map(lf->data.forks, (list_map_fun)&convert_listfork_to_array);
		f->data.forks = list_to_array(lf->data.forks, &f->num);
		list_free(lf->data.forks, &list_free_ignore);
	} else {
		f->data.leafs = list_to_array(lf->data.leafs, &f->num);
	}
	return f;
}

struct shadow_db *index_db(struct db *db, struct listitem *keys)
{
	//Make an intermediate linked list tree
	struct shadow_dbl *sdbl = safe_malloc(sizeof(struct shadow_dbl));
	sdbl->keys = list_to_array(keys, &sdbl->numkeys);

	//Add root fork
	sdbl->root.value = NULL;
	sdbl->root.isfork = sdbl->numkeys>0;
	sdbl->root.data.forks = NULL;

	logmsg(debug, "numkeys: %lu\n", sdbl->numkeys);

	sdbl = db_iterate(db->root, sdbl, &index_db_fun);

	//Convert to array
	struct shadow_db *sdb = safe_malloc(sizeof(struct shadow_db));
	sdb->numkeys = sdbl->numkeys;
	sdb->keys = sdbl->keys;

	//Convert root node
	struct shadow_db_fork *f = convert_listfork_to_array(&sdbl->root);
	sdb->root.value = f->value;
	sdb->root.isfork = f->isfork;
	if(sdb->root.isfork)
		sdb->root.data.forks = f->data.forks;
	else
		sdb->root.data.leafs = f->data.leafs;
	free(f);
	free(sdbl);
	return sdb;
}

void free_db_fork(struct shadow_db_fork *f)
{
	free(f->value);
	if(f->isfork){
		for(uint64_t i = 0; i<f->num; i++)
			free_db_fork(f->data.forks+i);
		free(f->data.forks);
	} else {
		free(f->data.leafs);
	}
}

void free_db_index(struct shadow_db *sdb)
{
	free_db_fork(&sdb->root);
	safe_free(2, sdb->keys, sdb);
}
