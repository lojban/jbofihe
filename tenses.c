/***************************************
  $Header$

  Processing to do with tense tokens, e.g. working out how to gloss
  them depending on the context where they arise.
  ***************************************/

/* COPYRIGHT */

/*
  
  Processing to do clever things with how tenses are glossed

  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "rpc.tab.h"
#include "nonterm.h"
#include "functions.h"

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
check_tense_context(TreeNode *x)
{
  TreeNode *parent;
  struct nonterm *nt;
  XTenseCtx *tc;

  parent = x->parent;

  for (;;) {
    /* Already know parent is non-terminal!! */
    nt = & parent->data.nonterm;

    /* In all cases, we will need to traverse upwards until we come
       out of <tag> or <stag>, since they don't tell us anything
       useful. */

    switch (nt->type) {
      case TERM_TAGGED_SUMTI:
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_TERM;
        goto done;
        break;

      case TEXT_1C:
      case I_JJ_STAG_BO:
      case GIHEK_STAG_KE:
      case GIHEK_STAG_BO:
      case JOIK_EK_KE:
      case JOIK_EK_STAG_BO:
      case JOIK_STAG_KE:
      case JOIK_JEK_STAG_BO:
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_LINK;
        goto done;
        break;

      case SUMTI_5:
      case SUMTI_TAIL_1:
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_NOUN;
        goto done;
        break;

      case STATEMENT_3:
      case GEK_SENTENCE:
      case MAIN_SELBRI:
        /* It's no use just looking for <selbri>, because that happens
           in a sumti context as well. */
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_SELBRI;
        goto done;
        break;

      case JAI_TAG_TU2:
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_JAITAG;
        goto done;
        break;

      case MEX_OPERATOR:
        /* Via NAhU selbri */
        goto done;
        break;

      case OPERAND_3:
        /* Via NIhE selbri */
        goto done;
        break;

      case COMPLEX_TENSE_MODAL:
        /* Via FIhO selbri */
        goto done;
        break;

      case FREE:
        goto done;
        break;

      case GEK:
        /* This is the case where we have something like 'ca gi X gi Y' which
           can be treated like 'X icabo Y'. */
        tc = prop_tense_ctx(x, YES);
        tc->ctx = TSC_CONNECT;
        goto done;
        break;

      case CHUNKS:
        fprintf(stderr, "Selma'o PU token at line %d column %d lies in unhandled context\n",
                x->start_line, x->start_column);
        abort(); /* oops, we've got all the way out to the top of the parse tree. */
        goto done;
        break;

      default:
        break;
    }

    parent = parent->parent;
  }
  
done:
  return;

}

/*++++++++++++++++++++++++++++++++++++++
  Scan for each node in selma'o PU.  Backtrack up the tree to find out
  the context where it occurs.

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
scan_tenses(TreeNode *x)
{
  struct nonterm *nt;
  int nc, i;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = & x->data.nonterm;

    switch (nt->type) {
      case TIME_OFFSET:
        check_tense_context(x);
        break;
      default:
        break;
    }

    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      scan_tenses(c);
    }
  } else {
    if (x->type == N_CMAVO) {
      switch (x->data.cmavo.selmao) {
        case ZAhO:
        case VA:
        case FAhA:
        case BAI:
        case PU:
          check_tense_context(x);
          break;

        default:
          break;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Top level of tense handling, exported to rest of software

  TreeNode *top
  ++++++++++++++++++++++++++++++++++++++*/

void
tense_processing(TreeNode *top)
{
  scan_tenses(top);

}
