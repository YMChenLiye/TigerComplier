%{
#include <string.h>
#include "util.h"
#include "absyn.h"
#include "symbol.h" 
#include "y.tab.h"
#include "errormsg.h"

int charPos=1;          //本文件处理到的位置

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}

%x COMMENT_STATE STRING_STATE

%%

" "         { adjust(); continue;}
[\r\t]      { adjust(); continue;}

\n          { adjust(); EM_newline(); continue;}

    /* reserved words */
while       { adjust(); return WHILE; }
for         { adjust(); return FOR; }
to          { adjust(); return TO; }
break       { adjust(); return BREAK; }
let         { adjust(); return LET; }
in          { adjust(); return IN; }
end         { adjust(); return END; }
function    { adjust(); return FUNCTION; }
var         { adjust(); return VAR; }
type        { adjust(); return TYPE; }
array       { adjust(); return ARRAY; }
if          { adjust(); return IF; }
then        { adjust(); return THEN; }
else        { adjust(); return ELSE; }
do          { adjust(); return DO; }
of          { adjust(); return OF; }
nil         { adjust(); return NIL; }

    /* punctuations */
":"         {adjust(); return COLON;}
";"         {adjust(); return SEMICOLON;}
"("         {adjust(); return LPAREN;}
")"         {adjust(); return RPAREN;}
"["         {adjust(); return LBRACK;}
"]"         {adjust(); return RBRACK;}
"{"         {adjust(); return LBRACE;}
"}"         {adjust(); return RBRACE;}
"."         {adjust(); return DOT;}
"+"         {adjust(); return PLUS;}
"-"         {adjust(); return MINUS;}
"*"         {adjust(); return TIMES;}
"/"         {adjust(); return DIVIDE;}
"="         {adjust(); return EQ;}
"<>"        {adjust(); return NEQ;}
"<"         {adjust(); return LT;}
"<="        {adjust(); return LE;}
">"         {adjust(); return GT;}
">="        {adjust(); return GE;}
"&"         {adjust(); return AND;}
"|"         {adjust(); return OR;}
":="        {adjust(); return ASSIGN;}
","         {adjust(); return COMMA;}

    /* Identifiers */
[a-zA-Z][a-zA-Z0-9_]*    { adjust(); yylval.sval=String(yytext); /*printf("id: %s\n", yytext);*/ return ID; }

    /* Integer literal */
[0-9]+                  { adjust(); yylval.ival=atoi(yytext); return INT; }


    /* Comments */
"/*"        { adjust(); BEGIN(COMMENT_STATE); }
"*/"        { adjust(); EM_error(EM_tokPos, "close comment before open it"); yyterminate(); }

<COMMENT_STATE>
{
    .       { adjust();}
    "*/"    { adjust(); BEGIN(INITIAL); }
}

    /* String literal */
"\""        { adjust(); BEGIN(STRING_STATE); yylval.sval=NULL; }

<STRING_STATE>
{
    "\""    { adjust(); BEGIN(INITIAL); return STRING; }
    [^"]*    { adjust(); yylval.sval=String(yytext); /*printf("string: %s\n", yytext);*/ }   
}

.    {adjust(); EM_error(EM_tokPos,"illegal token");}
