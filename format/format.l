%{

#include "format.h"
#include "format.tab.h"

%}

%%

\$		return DOLLAR;
\{		return OBRACE;
\}		return CBRACE;
,		return COMMA;
[^${}]*	formatyylval = safe_strdup(yytext); return LITERAL;

%%
