%{

#include "format.h"
#include "format.tab.h"

%}

%%

\$		return DOLLAR;
\{		return OBRACE;
\}		return CBRACE;
[^${}]*	yylval = safe_strdup(yytext); return LITERAL;

%%
