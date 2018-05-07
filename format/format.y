%{
#include "format.h"

int yydebug=0;

void yyerror(const char *str)
{
	die(str);
}

int yywrap()
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
