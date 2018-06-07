#include "util.h"
#include "search.h"
#include "db.h"
#include "log.h"
#include "list.h"
#include "format.h"

static struct listitem *head = NULL;

static int tag_cmp(const void *k, const void *v)
{
	return strcmp((char *)k, ((struct db_tag *)v)->key);
}

struct query *gq;
void *search(struct db_file *f, void *st)
{
	if(f->tags == NULL){
		logmsg(debug, "skip non music file\n");
		return st;
	}
	//For each search key
	struct listitem *c = head;
	while(c != NULL){
		//See if the tag is there
		void *res = bsearch(c->value, f->tags, f->ntags, sizeof(struct db_tag), tag_cmp);
		if(res != NULL){
			logmsg(debug, "search for %s=%s in %s, found %s\n", c->value, gq->query, f->path, ((struct db_tag *)res)->value);
			if(strcmp(((struct db_tag *)res)->value, gq->query) == 0){
				st = (void *)list_prepend(st, f);
				break;
			}
		}
		c = c->next;
	}
	return st;
}

void search_db(struct db * db)
{
	gq = parse_query(command.fields.search_opts.query);
	logmsg(warn, "Searching for %s\n", command.fields.search_opts.query);

	//Naive searching:
	struct listitem *result = iterate_db(db->root, NULL, &search);

	//Print
	for(struct listitem *i = result; i != NULL; i = i->next){
		struct db_file *f = (struct db_file *)i->value;
		fformat(stdout, command.fmt, f);
	}

	//Free
	list_free(result, list_free_ignore);

	free(gq);
	(void)db;
}

struct query *parse_query(char *qstring)
{
	struct query *q = safe_malloc(sizeof (struct query));
	q->query = qstring;
	return q;
}

char *query_to_string(struct query *q)
{
	return safe_strdup(q->query);
}

void search_key_add(char *k)
{
	head = list_prepend(head, (void *)k);
}

void search_key_free()
{
	list_free(head, free);
}
