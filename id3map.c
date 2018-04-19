#include <stdlib.h>
#include <stdbool.h>

#include "id3map.h"
#include "list.h"
#include "util.h"
#include "log.h"

#define id3err(s) logmsg(warn, "id3error: " s "\n")

struct id3map {
	char id[5];
	char *key;
	char *txxx;
};

static struct listitem *head = NULL;

void id3map_add_from_string(char *id)
{
	char *txxx = NULL;
	char *key = strchr(id, ':');
	if(key == NULL) {
		id3err("no ':' found");
		return;
	}
	key++[0] = '\0';
	char *key1 = strchr(key, ':');
	//TXXX:key:value
	if(key1 != NULL) {
		if(strncmp(id, "TXXX", 4) != 0){
			id3err("two : found but tag is not TXXX");
			return;
		}
		key1++[0] = '\0';
		txxx = key1;
		key = key1;
	}
	id3map_add(trim(id), trim(key), txxx);
}

void *equalid;
char *equaltxxx;
bool id3map_equal(void *a)
{
	struct id3map *m = (struct id3map *)a;
	if(m->txxx == NULL){
		return strcmp(m->id, equalid) == 0;
	} else {
		return equaltxxx != NULL && strcmp(m->txxx, equaltxxx) == 0;
	}
}

void id3map_add(char *id, char *key, char *txxx)
{
	if(strlen(id) > 5){
		id3err("tag id may only be 4 chars");
		return;
	}
	//Add
	struct id3map *r = safe_malloc(sizeof(struct id3map));
	strncpy(r->id, id, 5);
	r->key = safe_strdup(key);
	r->txxx = txxx == NULL ? NULL : safe_strdup(txxx);
	head = list_prepend(head, (void *)r);
}


char *id3map_get(char *id, char *txxx)
{
	logmsg(debug, "Getting id3map entry for %s\n", id);
	equalid = id;
	equaltxxx = txxx;
	struct id3map *r = list_find(head, &id3map_equal, NULL);
	return r == NULL ? id : r->key;
}

void id3map_f(void *a)
{
	struct id3map *m = (struct id3map *)a;
	safe_free(3, m->key, m->txxx, m);
}

void id3map_free()
{
	list_free(head, &id3map_f);
	head = NULL;
}
