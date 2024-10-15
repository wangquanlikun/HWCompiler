%{
#include "y.tab.h"
%}
%%
[\n]           { return '$'; }
[ \f\r\t\v]    {}
[\+\-\*\/\(\)] { return yytext[0]; }
[0-9]+         { return NUM; }
\$             { return 0; }
.              { printf("unexpected character: %c\n", yytext[0]); exit(0); }
%%