/***-*- Fundamental -*-************************************
  $Header$

  Functions for gathering text into a list and preprocessing the list.
  ***************************************/

/* COPYRIGHT */

%{
#include "cm.h"
static int pdepth = 0;
%}

C	[bcdfgjklmnprstvxz]
CNR	[bcdfgjklmnpstvxz]
V	[aeiou]
L	({C}|{V}|y|\')
LC  {L},?

/* The definition of cmavo is pretty liberal, allowing (hopefully)
   the minimum punctuation and arbitrary extensions, e.g. ca'ai'au. */
VV  {V}{V}?
VVY {VV}|y

CCC {C}{VVY}(\'{VVY})*
VCC {VVY}(\'{VVY})*

CMT ({CCC}|\.{VCC})*
CM1 {CCC}{CMT}
CM2 \.?{VCC}{CMT}
CM  {CM1}|{CM2}

/* These patterns are compelled to be BRIVLA - they are the special
   cases where r, n or y is inserted to prevent a cmavo breaking off. */
BM1 {C}{V}\'?{V}r{CNR}{L}+{V}
BM2 {C}{V}\'?{V}nr{L}+{V}
BM3 {C}{V}{C}y{C}{L}+{V}
BM  {BM1}|{BM2}|{BM3}

%x PAREN

/* The main rule for brivla is a pretty liberal definition - it should match
   fu'ivla too. Rely on the cmavo rule coming earlier so that sequences of
   cmavo glued together don't match this rule. */

%%

{BM}                { gather_brivla(yytext); }
{CM}                { gather_cmavo(yytext); }
{C}{L}+{V}          { gather_brivla(yytext); }
(\.?{LC}*{C})\.?    { gather_cmene(yytext); }
{L}+                { gather_fallthru(yytext); }
[ \t]+              { /* discard */ }
\r?\n               { /* discard */ }
\r?\n(\r?\n)+       { gather_newline(); }
[([]                { pdepth++; BEGIN PAREN; gather_paren(yytext); }

<PAREN>[[(]         { ++pdepth; gather_paren(yytext); }
<PAREN>[^][()]+     { gather_paren(yytext); }
<PAREN>[])]         { --pdepth; gather_paren(yytext); if (pdepth==0) { BEGIN INITIAL;} }

[a-zA-Z]+           { gather_fallthru(yytext); }
.                   { gather_fallthru(yytext); }

