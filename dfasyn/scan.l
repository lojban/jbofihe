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

%x PASSTHRU
%x STR

%%

STATE|State|state           { return STATE; }
ABBREV|Abbrev|abbrev        { return ABBREV; }
DEFINE|Define|define        { return DEFINE; }
TOKENS|Tokens|tokens        { return TOKENS; }
PREFIX|Prefix|prefix        { return PREFIX; }
BLOCK|Block|block           { return BLOCK; }
ENDBLOCK|EndBlock           { return ENDBLOCK; }
Endblock|endblock           { return ENDBLOCK; }
TYPE|Type|type              { return TYPE; }
RESULT|Result|result        { return RESULT; }
EARLYRESULT                 { return EARLYRESULT; }
EarlyResult                 { return EARLYRESULT; }
Earlyresult                 { return EARLYRESULT; }
earlyresult                 { return EARLYRESULT; }
ATTRIBUTE|ATTR              { return ATTR; }
Attribute|Attr              { return ATTR; }
attribute|attr              { return ATTR; }
DEFATTR|DefAttr             { return DEFATTR; }
Defattr|defattr             { return DEFATTR; }
DEFRESULT|DefResult         { return DEFRESULT; }
Defresult|defresult         { return DEFRESULT; }
SYMBOL|Symbol|symbol        { return SYMBOL; }
SYMRESULT|SymResult         { return SYMRESULT; }
Symresult|symresult         { return SYMRESULT; }
EARLYSYMRESULT              { return EARLYSYMRESULT; }
EarlySymResult              { return EARLYSYMRESULT; }
EarlySymresult              { return EARLYSYMRESULT; }
Earlysymresult              { return EARLYSYMRESULT; }
earlysymresult              { return EARLYSYMRESULT; }
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
^\%\{[ \t]*\n               { BEGIN PASSTHRU; }
\"                          { BEGIN STR; }

<PASSTHRU>^\%\}[ \t]*\n     { BEGIN INITIAL; }
<PASSTHRU>\n                { fputs(yytext, yyout); lineno++; }
<PASSTHRU>.+                { fputs(yytext, yyout); }

<STR>\"                     { BEGIN INITIAL; }
<STR>[^"]*                  { yylval.s = new_string(yytext); return STRING; }



