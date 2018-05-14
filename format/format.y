%{
#include "format.h"

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
		| atoms atom
		;

atom	: fun
		| LITERAL
			{
				printf("lit: %s\n", $1);
			}
		;

fun		: DOLLAR OBRACE LITERAL CBRACE
			{
				printf("tag: '%s'\n", $3);
			}
		| DOLLAR LITERAL OBRACE args CBRACE
			{
				printf("fun: '%s'\n", $2);
			}
		;
args	:
		| nargs
nargs	: LITERAL
			{
				printf("arg: '%s'\n", $1);
			}
		| nargs COMMA LITERAL
			{
				printf("arg: '%s'\n", $3);
			}
