#include "util.h"
#include "search.h"
#include "db.h"
#include "log.h"
#include "list.h"

static struct listitem *head = NULL;

/*
struct db_entry {
	char *dir;
	uint64_t nfile;
	uint64_t ndir;
	struct db_file *files;
	struct db_entry *dirs;
};

struct db_file {
	char *path;
	time_t mtime;
	size_t size;
	struct db_tags *tags;
};

struct db_tags {
	uint64_t ntags;
	char **keys;
	char **values;
};
*/

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
		struct db_tags *t = f->tags;
		if(f->tags == NULL){
			logmsg(debug, "skip non music file\n");
			continue;
		}
		//In their tags
		for(uint64_t tag = 0; tag<t->ntags; tag++){
			//For each search key
			struct listitem *c = head;
			while(c != NULL){
				if(strcmp(t->keys[tag], c->value) == 0){
					if(strcmp(t->values[tag], q->query) == 0){
						rhd = list_prepend(rhd, f);
						break;
					}
				}
				c = c->next;
			}

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
		struct db_file *f = (struct db_file *)(i->value);
		printf("%s\n", f->path);
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
