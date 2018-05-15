%option noinput
%option nounput
%{

#include "format.h"
#include "format.tab.h"

%}

%%

,[ \t]*    return COMMA;
\$         return DOLLAR;
\{         return OBRACE;
\}         return CBRACE;
(\\[,${}\\]|[^,${}])+   formatyylval = safe_strdup(yytext); return LITERAL;

%%
