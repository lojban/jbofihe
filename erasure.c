/***************************************
  $Header$

  Handle si, sa, su 
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "nodes.h"
#include "rpc_tab.h"
#include "functions.h"

/*++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++*/

static void
handle_si(TreeNode *toks)
{
  TreeNode *x, *y, *nt, *z;
  TreeNode *ntt, *t;
  TreeNode *a;
  int require, si_got;

  for (x = toks->next;
       x != toks;
       x = nt) {

    nt = x->next;

    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == SI) {

      y = x->prev;

      if (y == toks) {
        fprintf(stderr, "Unmatched SI at start of input, line %d column %d\n",
                x->start_line, x->start_column);
        continue;
      }

      require = 1;

      switch (y->type) {
        case N_ZOI:
          require = 4;
          break;
        case N_ZO:
          require = 2;
          break;
        case N_LOhU:
          require = 3;
          break;
        case N_BRIVLA:
        case N_CMAVO:
        case N_CMENE:
          require = 1;
          break;
        case N_BROKEN_ERASURE:
          require = y->data.erasure.defects;
          break;
        default:
          fprintf(stderr, "Unrecognized token type %d before SI\n", y->type);
          fflush(stderr);
          assert(0);
      }

      z = x->next;
      si_got = 1;

      while ((si_got < require) &&
             (z != toks)) {
        if ((z->type == N_CMAVO) &&
            (z->data.cmavo.selmao == SI)) {
          si_got ++;
          z = z->next;
        } else {
          break;
        }
      }

      if (si_got < require) {
        y->type = N_BROKEN_ERASURE;
        y->data.erasure.defects = require - si_got;
      } else {
        delete_node(y);
      }

      for (t = x; t != z; t = ntt) {
        ntt = t->next;
        delete_node(t);
      }

      nt = z;

    }        
  }
}

/*++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++*/

static void
handle_sa(TreeNode *toks)
{

}

/*++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++*/

static void
handle_su(TreeNode *toks)
{

}

  
/*++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++*/

void
do_erasures(TreeNode *toks)
{
  handle_si(toks);
  handle_sa(toks);
  handle_su(toks);
}

