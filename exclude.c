#include <stdlib.h>
#include <stdio.h>
#include <fnmatch.h>

#include "list.h"
#include "log.h"

static struct listitem *head = NULL;

void exclude_add(char *pat)
{
	head = list_add(head, (void *)pat);
}

void exclude_free()
{
	list_free(head, &free);
}

bool exclude(char *v)
{
	struct listitem *r = head;
	while(r != NULL){
		switch(fnmatch((const char *)r->value, v, 0)){
		case 0:
			return true;
		default:
			perror("fnmatch");
		case FNM_NOMATCH:
			break;
		}
		r = r->next;
	}
	return false;
}
