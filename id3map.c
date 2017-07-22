#include <stdlib.h>
#include <stdbool.h>

#include "id3map.h"
#include "list.h"
#include "util.h"
#include "log.h"

struct id3map {
	char id[5];
	char *key;
};

static struct listitem *head = NULL;

void id3map_add_from_string(char *id)
{
	char *key= strchr(id, ':');
	if(key == NULL) {
		logmsg(warn, "Malformed id3mapentry, no ':' found\n");
		return;
	}
	key++[0] = '\0';
	id3map_add(safe_strdup(trim(id)), safe_strdup(trim(key)));
}

void *equalstr;
bool id3map_equal(void *a)
{
	return strcmp(((struct id3map *)a)->id, equalstr) == 0;
}

void id3map_add(char *id, char *key)
{
	if(strlen(id) > 5){
		logmsg(warn, "id3map, id too long\n");
		return;
	}
	//Find dups and remove
	equalstr = id;
	uint32_t toremove;
	void *remove;
	head = list_find(head, &id3map_equal, &toremove);
	head = list_delete(head, toremove, &remove);
	if(remove != NULL){
		;//safe_free(2, remove, ((struct id3map *)remove)->key);
	}

	//Add
	struct id3map *r = safe_malloc(sizeof(struct id3map));
	strncpy(r->id, id, 5);
	r->key = safe_strdup(key);
	head = list_add(head, (void *)r);
}


char *id3map_get(char *id)
{
	logmsg(debug, "Getting id3map entry for %s\n", id);
	equalstr = id;
	struct id3map *r = list_find(head, &id3map_equal, NULL);
	return r == NULL ? id : r->key;
}

void id3map_f(void *a)
{
	safe_free(2, ((struct id3map *)a)->key, a);
}

void id3map_free()
{
	list_free(head, &id3map_f);
	head = NULL;
}
