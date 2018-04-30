#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdint.h>

struct listitem {
	void *value;
	struct listitem *next;
};

struct listitem *list_prepend(struct listitem *, void *);
struct listitem *list_append(struct listitem *head, void *value);
void *list_find(struct listitem *, bool(*)(void *), uint32_t *);
struct listitem *list_delete(struct listitem *, uint32_t, void **);
void list_iterate(struct listitem *, void (*)(void *));
void list_free(struct listitem *, void(*)(void *));
void list_free_ignore(void *);

#endif
