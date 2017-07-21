#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

struct listitem {
	void *value;
	struct listitem *next;
};

struct listitem *list_add(struct listitem *, void *);

void *list_find(struct listitem *, void *, bool(*)(void *, void *));

void list_free(struct listitem *, void(*)(void *));

#endif
