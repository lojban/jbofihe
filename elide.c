
/***************************************
  $Header$

  Monitor passage of elidable tokens into the parser, and determine whether one
  or more of them could be discarded with no change in meaning of the text.
  ***************************************/

/* COPYRIGHT */

#define YYSTYPE TreeNode *

#include <stdio.h>
#include "nodes.h"
#include "rpc_tab.h"
#include "cmavotab.h"

/* Pointers grabbed from registration function */
static const short *yypact = NULL;
static const short *yytranslate = NULL;
static const short *yycheck = NULL;
static const short *yytable = NULL;

/* Need to see these to intercept the values set by the lexer and to force the
 * values seen by the parser. */
extern YYSTYPE yylval;
extern YYLTYPE yylloc;

typedef struct {
  int value;
  YYLTYPE yylloc;
  TreeNode *yylval;
} TokenType;

/* tok is the most recent token given to the parser.  If it is elidable, the
 * 'eli' flag is set and tok1 is the next token. */
static TokenType tok, tok1;

static int eli = 0;

/*++++++++++++++++++++++++++++++++++++++
  Called by modified yyparse to register the addresses of the parser tables
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_register(const short *pact, const short *translate, const short *check, const short *table)
{
  yypact = pact;
  yytranslate = translate;
  yycheck = check;
  yytable = table;
}

/*++++++++++++++++++++++++++++++++++++++
  Return 1 if the cmavo is elidable
  ++++++++++++++++++++++++++++++++++++++*/

static int
is_elidable(TokenType *tok)
{
  if (tok->yylval->type != N_CMAVO) return 0;
  
  switch (cmavo_table[tok->yylval->data.cmavo.code].code) {
    case CM_BEhO:
    case CM_BOI:
    case CM_CU:
    case CM_DOhU:
    case CM_FEhU:
    case CM_GEhU:
    case CM_KEI:
    case CM_KEhE:
    case CM_KU:
    case CM_KUhE:
    case CM_KUhO:
    case CM_LIhU:
    case CM_LOhO:
    case CM_LUhU:
    case CM_MEhU:
    case CM_NUhU:
    case CM_SEhU:
    case CM_TEhU:
    case CM_TOI:
    case CM_TUhU:
    case CM_VAU:
    case CM_VEhO:
      return 1;
      break;
    default:
      return 0;
      break;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Trace shifts by the parser.  When an elidable is shifted, we can cease
  checking for whether anything coming after it could have been shifted in
  a state where a reduction took place
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_shift(int yychar)
{

}

/*++++++++++++++++++++++++++++++++++++++
  Trace reductions by the parser.  This is the clever bit where we check
  whether the following cmavo could shift in the state where we are reducing.
  If this doesn't happen in any state before the elidable is shifted, then the
  elidable was unnecessary.
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_reduce(int yystate, int yyn)
{


}

/*++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++*/

static void
fill_tok(TokenType *t)
{
  t->value = yylex1();
  t->yylloc = yylloc;
  t->yylval = yylval;
}

/*++++++++++++++++++++++++++++++++++++++
  Provide the yylex function
  ++++++++++++++++++++++++++++++++++++++*/

extern int yylex1(void);

int
yylex(void)
{
  
  return yylex1(); /* Short-circuit until this module is coded up properly */
  
  if (eli) { 
    /* two tokens are already buffered, tok already returned, need to return
     * tok1 this time, shift down and maybe refill */
    tok = tok1;
  } else {
    fill_tok(&tok);
  }

  if (is_elidable(&tok)) {
    eli = 1;
    fill_tok(&tok1);
  } else {
    eli = 0;
  }

  yylval = tok.yylval;
  yylloc = tok.yylloc;
  return tok.value;
}

/*++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++*/


