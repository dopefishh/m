#include "list.h"
#include "db.h"
#include "util.h"
#include "format.h"

char *upto_dollar(char *fmt)
{
	while(*fmt != '\0'){
		if (*fmt == '$'){
			if(*(fmt+1) == '$'){
				fmt++;
			} else {
				return fmt;
			}
		}
	}
	return fmt;
}

struct listitem *parse_fmt_atoms(char *fmt)
{
	struct listitem *head = NULL, *current;

	char *c;
	while(*fmt != '\0'){
		c = upto_dollar(fmt);
		//We are done
		if(c == '\0')
			break;
		//Parsed a dollar
		if (*c == '$'){
			//Finish previous
			struct fmt_atom *a = safe_malloc(sizeof (struct fmt_atom));
			a->which = fmt_lit;
			a->atom.lit = safe_strndup(fmt, fmt - c);
		}
		fmt = c;
	}
}

char *format(struct listitem *fmt, struct db_file *item)
{
	return "";
	(void)fmt;
	(void)item;
}
