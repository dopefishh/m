#include "util.h"
#include "search.h"
#include "db.h"
#include "log.h"

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

struct resultset *search(struct db *db, struct query *q)
{
	char *qs = query_to_string(q);
	logmsg(debug, "Searching for %s\n", qs);
	free(qs);
	return NULL;
	(void)q;
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