%{

#include "format.h"
#include "format.tab.h"

%}

%%

\$		return DOLLAR;
\{		return OBRACE;
\}		return CBRACE;
[^${}]*	formatyylval = safe_strdup(yytext); return LITERAL;

%%
