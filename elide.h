/***************************************
  $Header$

  Header file for token handling between elider and lex2
  
  ***************************************/

#ifndef ELIDE_H
#define ELIDE_H

#include "nodes.h"
#include "rpc_tab.h"

typedef struct {
  int value;
  YYLTYPE yylloc;
  TreeNode *yylval;
} TokenType;

/* API provided from lex2 to the elider.
  This is fatter than the normal yylex API, because the semantic value and
  line/col number info are passed by reference rather than as global vars. */

void yylex1(TokenType *res);

#endif /* ELIDE_H */


