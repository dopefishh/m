%{
#include "format.h"

extern struct listitem *fmt_list;

int formatyydebug=1;

void formatyyerror(const char *str)
{
	logmsg(warn, "fmt error: %s\n", str);
}

int formatyywrap()
{
	return 1;
}

%}

%token DOLLAR OBRACE CBRACE LITERAL COMMA

%%

atoms	:
			{
				$$ = NULL;
			}
		| atoms atom
			{
				struct listitem *tail = list_append($1, $2);
				if($1 == NULL)
					$$ = tail;
				else
					$$ = $1;
				fmt_list = $1;
			}
		;
atom	: DOLLAR LITERAL OBRACE args CBRACE
			{
				printf("fun: %s\n", (char *)$1);
				struct fmt_atom *current = safe_malloc(sizeof (struct fmt_atom));
				current->islit = false;
				current->atom.fun.name = safe_strdup((char *)$2);
				current->atom.fun.args = $4;
				$$ = current;
			}
		| LITERAL
			{
				printf("lit: %s\n", (char *)$1);
				struct fmt_atom *current = safe_malloc(sizeof (struct fmt_atom));
				current->islit = true;
				current->atom.lit = safe_strdup((char *)$1);
				$$ = current;
			}
		;
args	:
			{ $$ = NULL; }
		| nargs
			{ $$ = $1; }
nargs	: atom
			{
				$$ = list_append(NULL, $1);
			}
		| nargs COMMA atom
			{
				list_append($1, list_append(NULL, $3));
				$$ = $1;
			}
