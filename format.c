#include <errno.h>
#include <stdarg.h>
#include <math.h>

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
void rewrite(void *i);

void nargfun(char *funname, struct listitem *funargs, int nargs, ...)
{
	va_list valist;
	va_start(valist, nargs);

	for (int i = 0; i < nargs; i++) {
		if(funargs == NULL)
			die("Not enough arguments to %s\n", funname);
		struct fmt_atom *a = funargs->value;
		char **lit = va_arg(valist, char **);

		while(!a->islit)
			rewrite(a);

		*lit = a->atom.lit;
		funargs = funargs->next;
	}

	va_end(valist);
}

int64_t intarg(char *a)
{
	errno = 0;
	int64_t p = strtoll(a, NULL, 10);
	if(errno != 0)
		die("Unable to parse int: %s\n", a);
	return p;
}

int64_t min(int64_t a, int64_t b)
{
	return a > b ? b : a;
}


FILE *gof;
struct db_file *gdf;
void rewrite(void *i)
{
	struct fmt_atom *item = (struct fmt_atom *)i;
	if(!item->islit){
		item->islit = true;
		char *funname = item->atom.fun.name;
		struct listitem *funargs = item->atom.fun.args;

		//Rewrite
		if(strcmp(funname, "tag") == 0){
			char *tag = NULL, *fallback = NULL;
			nargfun("tag", funargs, 2, &tag, &fallback);

			tag = file_tag_find(gdf, tag);
			if(tag == NULL) {
				tag = fallback;
			}
			item->atom.lit = safe_strdup(tag);
		} else if(strcmp(funname, "filepath") == 0){
			nargfun("filepath", funargs, 0);
			item->atom.lit = safe_strdup(gdf->path);
		} else if(strcmp(funname, "if") == 0){
			char *cond, *ethen, *eelse;
			nargfun("if", funargs, 3, &cond, &ethen, &eelse);
			item->atom.lit = safe_strdup(cond[0] == '\0' ? ethen : eelse);
		} else if(strcmp(funname, "rpad") == 0){
			char *str, *padchar, *width;
			nargfun("rpad", funargs, 3, &str, &padchar, &width);
			int64_t padwidth = intarg(width);
			if(padwidth < 0)
				die("Padding width must be positive");

			item->atom.lit = safe_malloc(min(strlen(str),padwidth)+1);
			strcpy(item->atom.lit, str);

			int64_t i = 0, padc = 0, padmod = strlen(padchar);
			for(; i<padwidth; i++){
				item->atom.lit[i] = str[i] != '\0'
					? str[i]
					: padchar[padc++%padmod];
			}
			item->atom.lit[i] = '\0';
		} else if(strcmp(funname, "lpad") == 0){
			char *str, *padchar, *width;
			nargfun("lpad", funargs, 3, &str, &padchar, &width);
			int64_t padwidth = intarg(width);
			if(padwidth < 0)
				die("Padding width must be positive");
			item->atom.lit = safe_malloc(min(strlen(str),padwidth)+1);

			uint64_t i = 0, padc = 0, padmod = strlen(padchar);
			for(; i<padwidth-strlen(str); i++)
				item->atom.lit[i] = padchar[padc++%padmod];
			strcpy(item->atom.lit + i, str);
		} else if(strcmp(funname, "plus") == 0){
			char *a, *b;
			nargfun("plus", funargs, 2, &a, &b);
			int64_t r = intarg(a) + intarg(b);
			item->atom.lit = safe_malloc(abs(log10(r)+2));
			sprintf(item->atom.lit, "%ld", r);
		} else if(strcmp(funname, "min") == 0){
			char *a, *b;
			nargfun("min", funargs, 2, &a, &b);
			int64_t r = intarg(a) - intarg(b);
			item->atom.lit = safe_malloc(abs(log10(r)+2));
			sprintf(item->atom.lit, "%ld", r);
		} else if(strcmp(funname, "abs") == 0){
			char *a;
			nargfun("abs", funargs, 1, &a);
			int64_t r = abs(intarg(a));
			item->atom.lit = safe_malloc(abs(log10(r)+2));
			sprintf(item->atom.lit, "%ld", r);
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
