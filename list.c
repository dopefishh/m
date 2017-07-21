#include <stdlib.h>
#include <stdint.h>

#include "log.h"
#include "list.h"
#include "util.h"

struct listitem *list_add(struct listitem *head, void *value)
{
	struct listitem *r;
	if(head == NULL){
		head = safe_malloc(sizeof(struct listitem));
		r = head;
	} else {
		r = head;
		while(r->next != NULL)
			r = r->next;
		r = (r->next = safe_malloc(sizeof(struct listitem)));
	}
	r->value = value;
	r->next = NULL;
	return head;
}

void *list_find(struct listitem *head, void *ndl, bool(*pred)(void *, void *), uint32_t *index)
{
	struct listitem *r = head;
	*index = 0;
	while(r != NULL){
		if(pred(r->value, ndl))
			return r->value;
		r = r->next;
		(*index)++;
	}
	*index = -1;
	return NULL;
}

struct listitem *list_delete(struct listitem *head, uint32_t index)
{
	struct listitem *r = head, *p = NULL;
	uint32_t in = 0;
	while(r != NULL && in < index){
		p = r;
		r = r->next;
		in++;
	}
	(void)p;
	//TODO delete, and allow the user to free it
	return NULL;
}

void list_free(struct listitem *head, void(*myfree)(void *))
{
	struct listitem *t;
	while(head != NULL){
		t = head->next;
		myfree(head->value);
		free(head);
		head = t;
	}
}
