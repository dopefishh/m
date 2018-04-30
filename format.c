#include "list.h"
#include "db.h"
#include "file.h"
#include "util.h"
#include "format.h"
#include "log.h"

#define fmt_die(s) {                          \
	logmsg(warn, "fmt_parsing: " s "\n"); \
	list_free(head, fmt_free);            \
	return NULL;                          \
	}

char *upto(char *fmt, char upto)
{
	while(*fmt != '\0'){
		if (*fmt == '\\' && *(fmt+1) == upto){
			fmt++;
		}
		if (*fmt == upto){
			return fmt;
		}
		fmt++;
	}
	return fmt;
}

struct listitem *parse_fmt_atoms(char *fmt)
{
	logmsg(debug, "parsing fmt atoms: %s\n", fmt);
	struct listitem *head = NULL, *current = NULL;

	char *c;
	while(*fmt != '\0'){
		//We look until we find a dollar or null byte
		c = upto(fmt, '$');

		if(c > fmt){
			struct fmt_atom *atom =
				safe_malloc(sizeof(struct fmt_atom));
			atom->which = fmt_lit;
			atom->atom.lit = strndup(fmt, c - fmt);
			current = list_append(current, atom);
			if(head == NULL){
				head = current;
			}
		}
		if (*c == '$'){
			if(*(c+1) != '{')
				fmt_die("$ not followed by {");
			char *end = upto(++c, '}');
			if(end == '\0')
				fmt_die("${ not terminated with }");
			struct fmt_atom *atom =
				safe_malloc(sizeof(struct fmt_atom));
			atom->which = fmt_tag;
			atom->atom.tag = strndup(c+1, end-c - 1);
			current = list_append(current, atom);
			if(head == NULL){
				head = current;
			}
			c = end+1;
		} else {
		}
		fmt = c;
	}
	return head;
}


void fmt_free(void *vfmt)
{
	struct fmt_atom *fmt = (struct fmt_atom *)vfmt;
	switch(fmt->which){
	case fmt_lit:
		free(fmt->atom.lit);
		break;
	case fmt_tag:
		free(fmt->atom.tag);
		break;
	}
	free(fmt);
}

FILE *gof;
struct db_file *gdf;
void format(void *i)
{
	struct fmt_atom *item = (struct fmt_atom *)i;
	char *t;
	switch(item->which){
	case fmt_lit:
		safe_fprintf(gof, "%s", item->atom.lit);
		break;
	case fmt_tag:
		t = file_tag_find(gdf, item->atom.tag);
		if(t != NULL)
			safe_fprintf(gof, "%s", t);
		break;
	}
}

void fformat(FILE *f, struct listitem *l, struct db_file *df)
{
	gof = f;
	gdf = df;
	list_iterate(l, format);
	safe_fprintf(f, "\n");
	gof = NULL;
	gdf = NULL;
}
