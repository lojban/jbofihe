/* -*-Fundamental-*-

  $Header$

  Flex input file for removing comments from C, bison etc.

  A special case, the comment [* error *] (replace [ and ] by /
  obviously!) is left in, so that the action.perl file can avoid adding
  actions for rules where we use YYERROR ourselves and tag the rule with
  this comment.  We might extend this mechanism.

*/

/* COPYRIGHT */

%{
#ifndef yywrap
#define yywrap() 1
#endif

static int depth = 0;
%}

%x COMMENT

%%

<INITIAL>\/\*	{BEGIN COMMENT; depth = 1;}
<INITIAL>\/\*\ error\ \*\/ { ECHO; }
<INITIAL>\/\*\ ET\ [A-Zh]+\ \*\/ { ECHO; }
<COMMENT>\/\*   {depth++;}
<COMMENT>\*\/   {depth--; if (depth==0) {BEGIN INITIAL;}}
<COMMENT>.      { /* discard */ }
<COMMENT>\n     { /* discard */ }
<INITIAL>.      { ECHO; }
<INITIAL>\n     { ECHO; }

%%

int main() {
	yylex();
	return 0;
}
