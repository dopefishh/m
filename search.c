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

struct listitem *search(struct db_entry *db, struct query *q, struct listitem *rhd)
{
	//In the directories we recurse
	for(uint64_t i = 0; i<db->ndir; i++){
		rhd = search(&(db->dirs[i]), q, rhd);
	}
	//In the files
	for(uint64_t file = 0; file<db->nfile; file++){
		struct db_file *f = &(db->files[file]);
		logmsg(debug, "path: %s\n", f->path);
		if(f->tags == NULL){
			logmsg(debug, "skip non music file\n");
			continue;
		}
		//For each search key
		struct listitem *c = head;
		while(c != NULL){
			//Find if the tag is there
			void *res = bsearch(c->value, f->tags, f->ntags,
				sizeof(struct db_tag), tag_cmp);
			if(res != NULL){
				if(strcmp(((struct db_tag*)res)->value,
						q->query) == 0){
					rhd = list_prepend(rhd, f);
					break;
				}
			}
			c = c->next;
		}
	}
	return rhd;
}

void search_db(struct db * db)
{
	struct query *q = parse_query(command.fields.search_opts.query);
	logmsg(debug, "Searching for %s\n", command.fields.search_opts.query);

	//Search
	struct listitem *result = search(db->root, q, NULL);

	//Print
	for(struct listitem *i = result; i != NULL; i = i->next){
		struct db_file *f = (struct db_file *)i->value;
		fformat(stdout, command.fmt, f);
	}

	//Free
	list_free(result, list_free_ignore);

	free(q);
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
