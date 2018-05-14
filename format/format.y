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
			{ $$ = NULL; }
		| atoms atom
			{
				printf("atoms: %p\n", $1);
				struct listitem *tail = list_append($1, $2);
				$$ = $1 == NULL ? tail : $1;
				fmt_list = $$;
				printf("tail: %p\n", tail);
				printf("lenatoms: %llu\n", list_length($$));
			}
		;
atom	: DOLLAR LITERAL OBRACE args CBRACE
			{
				printf("fun\n");
				struct fmt_atom *current = safe_malloc(sizeof (struct fmt_atom));
				current->islit = false;
				current->atom.fun.name = (char *)$2;
				current->atom.fun.args = $4;
				$$ = current;
			}
		| LITERAL
			{
				printf("lit\n");
				struct fmt_atom *current = safe_malloc(sizeof (struct fmt_atom));
				current->islit = true;
				current->atom.lit = (char *)$1;
				$$ = current;
			}
		;
args	:
			{ $$ = NULL; }
		| nargs
			{ $$ = $1; }
nargs	: atom
			{ $$ = list_append(NULL, $1); }
		| nargs COMMA atom
			{
				list_append($1, $3);
				$$ = $1;
			}
