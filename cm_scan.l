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
V	[aeiou]
L	({C}|{V}|y|\')

VCM	({V}|({V}{V})|({V}\'{V}))
CCM	({C}({V}|({V}{V})|({V}\'{V})))

%x PAREN

%%

((\.?{VCM})|{CCM})((\.{VCM})|{CCM})*	{ gather_cmavo(yytext); }

({C}{V}(\'?{V})?{C}y?{C}{L}*{V})	{ gather_brivla(yytext); }
({C}{C}{L}*{V})				{ gather_brivla(yytext); }

(\.?{L}*{C})				{ gather_cmene(yytext); }

{L}+					{ gather_fallthru(yytext); }

[ \t]+					{ /* discard */ }
\n					{ /* discard */ }

\n\n+					{ gather_newline(); }

[([]					{ pdepth++; BEGIN PAREN; gather_paren(yytext); }

<PAREN>[[(]				{ ++pdepth; gather_paren(yytext); }

<PAREN>[^][()]+				{ gather_paren(yytext); }

<PAREN>[])]				{ --pdepth; gather_paren(yytext); if (pdepth==0) { BEGIN INITIAL;} }

[a-zA-Z]+				{ gather_fallthru(yytext); }
.					{ gather_fallthru(yytext); }
