/***************************************
  $Header$

  Scan syntax tree for suspicious constructions that are hard to
  detect during the parse phase.
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

/* ================================================== */

void
error_scan(TreeNode *x)
{
  struct nonterm *nt;
  TreeNode *c;
  int i, nc;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;
    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      error_scan(c);
    }

    if (nt->type == FRAGMENT) {
      TreeNode *c1;
      c1 = child_ref(x, 0);
      if ((c1->type == N_NONTERM) &&
          (c1->data.nonterm.type == TERMS)) {
        fprintf(stderr, "Warning: Sentence may be missing selbri at line %d column %d?\n",
                x->start_line, x->start_column);
      }
    }


  } else {
    

  }

}

/* ================================================== */
