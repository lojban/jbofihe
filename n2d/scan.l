/**********************************************************************
  $Header$

  Lexical analyser definition for input files defining an NFA

 *********************************************************************/

/* COPYRIGHT */

%{
#include "n2d.h"
#include "parse.h"

int lineno = 1;
%}

%%

STATE|State|state           { return STATE; }
DEFINE|Define|define        { return DEFINE; }
TOKENS|Tokens|tokens        { return TOKENS; }
PREFIX|Prefix|prefix        { return PREFIX; }
BLOCK|Block|block           { return BLOCK; }
ENDBLOCK|EndBlock           { return ENDBLOCK; }
Endblock|endblock           { return ENDBLOCK; }
[A-Za-z0-9_.]+              { yylval.s = new_string(yytext); return STRING; }
\#.*$                       { /* strip comments */ }
\-\>                        { return ARROW; }
=                           { return EQUAL; }
\|                          { return PIPE; }
\:                          { return COLON; }
\n                          { lineno++; }
[ \t]+                      { /* ignore */ }

