#include "list.h"
#include "db.h"
#include "file.h"
#include "util.h"
#include "format.h"
#include "log.h"

#include "format/format.tab.h"

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE formatyy_scan_string(char * str);
extern int formatyyparse(void);
extern void formatyy_delete_buffer(YY_BUFFER_STATE buffer);

//Global variable
struct listitem *fmt_list = NULL;

struct listitem *parse_fmt_atoms(char *fmt)
{
	YY_BUFFER_STATE buffer = formatyy_scan_string(fmt);
	int res = formatyyparse();
	formatyy_delete_buffer(buffer);
	if(res != 0) {
		die("fmt parsing error: %d\n", res);
	}

	return fmt_list;
}

void fmt_atom_free(void *vfmt)
{
	struct fmt_atom *fmt = (struct fmt_atom *)vfmt;
	if(fmt->islit){
		free(fmt->atom.lit);
	} else {
		free(fmt->atom.fun.name);
		list_free(fmt->atom.fun.args, &fmt_atom_free);
	}
	free(fmt);
}

void fmt_free(struct listitem * fmt)
{
	list_free(fmt, &fmt_atom_free);
}

FILE *gof;
struct db_file *gdf;
void rewrite(void *i)
{
	struct fmt_atom *item = (struct fmt_atom *)i;
	//Recursive rewriting
	//Done rewriting
	if(!item->islit){
		item->islit = true;
		char *funname = item->atom.fun.name;
		struct listitem *funargs = item->atom.fun.args;

		//Rewrite
		if(strcmp(item->atom.fun.name, "tag") == 0){
			if(list_length(item->atom.fun.args) != 1){
				die("tag requires 1 arguments\n");
			}
			struct fmt_atom *a = (struct fmt_atom *)item->atom.fun.args->value;
			while(!a->islit)
				rewrite(a);
			logmsg(debug, "find %s tag\n", a->atom.lit);
			char *tag = file_tag_find(gdf, a->atom.lit);
			if(tag == NULL) {
				logmsg(warn, "Couldn't find tag %s\n", a->atom.lit);
				tag = "";
			}
			item->atom.lit = safe_strdup(tag);
		} else if(strcmp(item->atom.fun.name, "i") == 0){
			if(list_length(item->atom.fun.args) != 1){
				die("i requires 1 arguments\n");
			}
			struct fmt_atom *a = (struct fmt_atom *)item->atom.fun.args->value;
			while(!a->islit)
				rewrite(a);
			item->atom.lit = safe_strdup(a->atom.lit);
		} else if(strcmp(item->atom.fun.name, "k") == 0){
			if(list_length(item->atom.fun.args) != 2){
				die("k requires 2 arguments\n");
			}
			logmsg(debug, "k called with %llu args\n", list_length(item->atom.fun.args));
			struct fmt_atom *a2 = item->atom.fun.args->next->value;
			while(!a2->islit)
				rewrite(a2);
			item->atom.lit = safe_strdup(a2->atom.lit);
		} else {
			die("Unknown format function: %s\n", funname);
		}

		free(funname);
		list_free(funargs, &fmt_atom_free);
	}
}

void print(void *i)
{
	struct fmt_atom *item = (struct fmt_atom *)i;
	if(item->islit){
		safe_fprintf(gof, "%s", item->atom.lit);
	} else {
		die("Huh, rewriting didn't succeed?\n");
	}
}

void fformat(FILE *f, struct listitem *l, struct db_file *df)
{
	gof = f;
	gdf = df;
	list_iterate(l, rewrite);
	list_iterate(l, print);
	safe_fprintf(gof, "\n");
	gof = NULL;
	gdf = NULL;
}
