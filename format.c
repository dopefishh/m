#include "list.h"
#include "db.h"
#include "file.h"
#include "util.h"
#include "format.h"
#include "log.h"

#include "format/format.tab.h"
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE formatyy_scan_string(char * str);
extern int formatyyparse(void);
extern void formatyy_delete_buffer(YY_BUFFER_STATE buffer);

struct listitem *parse_fmt_atoms(char *fmt)
{
	YY_BUFFER_STATE buffer = formatyy_scan_string(fmt);
	int res = formatyyparse();
	formatyy_delete_buffer(buffer);
	logmsg(debug, "yyparse_res: %d\n", res);

	(void)fmt;
	return NULL;
//	logmsg(debug, "parsing fmt atoms: %s\n", fmt);
//	struct listitem *head = NULL, *current = NULL;
//
//	char *c;
//	while(*fmt != '\0'){
//		//We look until we find a dollar or null byte
//		c = upto(fmt, '$');
//
//		if(c > fmt){
//			struct fmt_atom *atom =
//				safe_malloc(sizeof(struct fmt_atom));
//			atom->which = fmt_lit;
//			atom->atom.lit = strndup(fmt, c - fmt);
//			current = list_append(current, atom);
//			if(head == NULL){
//				head = current;
//			}
//		}
//		if (*c == '$'){
//			c++;
//			//Not ${..} 
//#define fun(s) strncmp(s, c, strlen(s)) == 0
//			if(fun("fallback")){
//				//TODO
//			} else if(fun("tag")){
//			} else if(*c == '{'){
//
//			} else {
//				fmt_die("$ not followed by { or a function");
//			}
////
////			char *end = upto(++c, '}');
////			if(end == '\0')
////				fmt_die("${ not terminated with }");
////			struct fmt_atom *atom =
////				safe_malloc(sizeof(struct fmt_atom));
////			atom->which = fmt_tag;
////			atom->atom.tag = strndup(c+1, end-c - 1);
////			current = list_append(current, atom);
////			if(head == NULL){
////				head = current;
////			}
////			c = end+1;
//		} else {
//		}
//		fmt = c;
//	}
//	return head;
}


void fmt_free(void *vfmt)
{
	free(vfmt);
//	struct fmt_atom *fmt = (struct fmt_atom *)vfmt;
//	switch(fmt->which){
//	case fmt_lit:
//		free(fmt->atom.lit);
//		break;
//	case fmt_fun:
//		//TODO
////		free(fmt->atom.tag);
//		break;
//	}
//	free(fmt);
}

FILE *gof;
struct db_file *gdf;
void format(void *i)
{
	(void)i;
//	struct fmt_atom *item = (struct fmt_atom *)i;
//	char *t;
//	switch(item->which){
//	case fmt_lit:
//		safe_fprintf(gof, "%s", item->atom.lit);
//		break;
//	case fmt_fun:
//		switch(item->atom.fun.type){
//		case fmt_fun_tag:
//			t = file_tag_find(gdf, item->atom.tag);
//			if(t != NULL)
//				safe_fprintf(gof, "%s", t);
//			//TODO
//			break;
//		case fmt_fun_fallback:
//			//TODO
//			break;
//		}
//		break;
//	}
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
