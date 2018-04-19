#include <stdlib.h>
#include <stdint.h>

#include "log.h"
#include "list.h"
#include "util.h"

struct listitem *list_append(struct listitem *head, void *value)
{
	if(head == NULL)
		return list_prepend(NULL, value);
	struct listitem *r = head, *p;
	do {
		p = r;
	} while((r = r->next) != NULL);
	p->next = list_prepend(NULL, value);
	return head;
}

struct listitem *list_prepend(struct listitem *head, void *value)
{
	struct listitem *r = safe_malloc(sizeof(struct listitem));
	r->next = head;
	r->value = value;
	return r;
}

void *list_find(struct listitem *head, bool(*pred)(void *), uint32_t *index)
{
	struct listitem *r = head;
	uint32_t in = 0;
	while(r != NULL){
		if(pred(r->value)){
			if(index != NULL)
				*index = in;
			return r->value;
		}
		r = r->next;
		in++;
	}
	if(index != NULL)
		*index = -1;
	return NULL;
}

struct listitem *list_delete(struct listitem *head, uint32_t index, void **item)
{
	struct listitem *r = head, *p = NULL;
	*item = NULL;
	uint32_t in = 0;
	while(r != NULL && in < index){
		p = r;
		r = r->next;
		in++;
	}
	if(head == NULL){
		return NULL;
	}
	*item = r->value;
	if(r == head)
		head = r->next;
	else
		p->next = r->next;
	free(r);
	return head;
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

void list_free_ignore(void *p)
{
	(void)p;
}
