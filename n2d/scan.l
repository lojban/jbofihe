/**********************************************************************
  $Header$

  Lexical analyser definition for input files defining an NFA

 *********************************************************************/

/* Copyright (C) Richard P. Curnow  2000-2001 */
/* LICENCE */

%{
#include "n2d.h"
#include "parse.h"

int lineno = 1;
%}

%%

STATE|State|state           { return STATE; }
ABBREV|Abbrev|abbrev        { return ABBREV; }
DEFINE|Define|define        { return DEFINE; }
TOKENS|Tokens|tokens        { return TOKENS; }
PREFIX|Prefix|prefix        { return PREFIX; }
BLOCK|Block|block           { return BLOCK; }
ENDBLOCK|EndBlock           { return ENDBLOCK; }
Endblock|endblock           { return ENDBLOCK; }
RESULT|Result|result        { return RESULT; }
DEFRESULT|DefResult         { return DEFRESULT; }
Defresult|defresult         { return DEFRESULT; }
SYMBOL|Symbol|symbol        { return SYMBOL; }
SYMRESULT|SymResult         { return SYMRESULT; }
Symresult|symresult         { return SYMRESULT; }
[A-Za-z0-9_.]+              { yylval.s = new_string(yytext); return STRING; }
\#.*$                       { /* strip comments */ }
\-\>                        { return ARROW; }
=                           { return EQUAL; }
\|                          { return PIPE; /* OR */ }
\&                          { return AND; }
\~                          { return NOT; }
\!                          { return NOT; }
\^                          { return XOR; }
\*                          { return STAR; }
\?                          { return QUERY; }
\:                          { return COLON; }
\;                          { return SEMICOLON; }
\(                          { return LPAREN; }
\)                          { return RPAREN; }
\,                          { return COMMA; }
\n                          { lineno++; }
[ \t]+                      { /* ignore */ }

