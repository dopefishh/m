#include "list.h"
#include "db.h"
#include "file.h"
#include "util.h"
#include "format.h"
#include "log.h"

#include "format/format.tab.h"
#include "format/format.yy.h"

//Global variable
struct listitem *fmt_list = NULL;

struct listitem *parse_fmt_atoms(char *fmt)
{
	YY_BUFFER_STATE buffer = formatyy_scan_string(fmt);
	int res = formatyyparse();
	formatyy_delete_buffer(buffer);
	formatyylex_destroy();
	if(res != 0)
		die("fmt parsing error: %d\n", res);

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
	if(!item->islit){
		logmsg(debug, "rewrite a function: %s\n", item->atom.fun.name);
		item->islit = true;
		char *funname = item->atom.fun.name;
		struct listitem *funargs = item->atom.fun.args;

		//Rewrite
		if(strcmp(funname, "tag") == 0){
			if(list_length(item->atom.fun.args) != 1){
				die("tag requires 2 arguments\n");
			}
			struct fmt_atom *a = funargs->value;
			while(!a->islit)
				rewrite(a);
			char *tag = file_tag_find(gdf, a->atom.lit);
			if(tag == NULL) {
				struct fmt_atom *fallback = funargs->next->value;
				while(!fallback->islit)
					rewrite(fallback);
				tag = fallback->atom.lit;
			}
			item->atom.lit = safe_strdup(tag);
		} else if(strcmp(funname, "filepath") == 0){
			if(list_length(item->atom.fun.args) != 0){
				die("filepath takes no arguments\n");
			}
			item->atom.lit = safe_strdup(gdf->path);
		} else {
			die("Unknown format function: %s\n", funname);
		}

		free(funname);
		list_free(funargs, &fmt_atom_free);
	} else {
		logmsg(debug, "literal: %s\n", item->atom.lit);
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
