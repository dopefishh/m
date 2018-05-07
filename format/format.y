%{
#include "format.h"

int formatyydebug=0;

void formatyyerror(const char *str)
{
	die(str);
}

int formatyywrap()
{
	return 1;
}

%}

%token DOLLAR OBRACE CBRACE LITERAL

%%

atoms	:
		| atoms atom
		;

atom	: fun
		| literal
		;

fun		: DOLLAR LITERAL OBRACE atoms CBRACE
	 		{
				printf("fun: '%s'\n", $2);
			}
		;

literal	: LITERAL
