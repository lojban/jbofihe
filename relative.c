/***************************************
  $Header$

  Processing for relative clauses.

  ***************************************/

/* COPYRIGHT */

#include <assert.h>

#include "nodes.h"
#include "rpc.tab.h"
#include "functions.h"
#include "cmavotab.h"

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_goi_clause (TreeNode *x)
{
  TreeNode *goi, *term, *termc;
  XDontGloss *xdg;

  goi = find_nth_cmavo_child(x, 1, GOI);
  term = find_nth_child(x, 1, TERM);
  assert (goi);
  assert (term);
  termc = child_ref(term, 0);

  /* What sort of term? */
  switch (termc->data.nonterm.type) {

    case TERM_PLAIN_SUMTI:
      /* This is the standard 'associated with' meaning */
      break;

    case TERM_TAGGED_SUMTI:
    case TAGGED_TERMSET:
      /* In this case we want to elide the gloss for the GOI (in all
         cases of GOI?) */
      xdg = prop_dont_gloss(goi, YES);
      break;

    case TERM_PLACED_SUMTI:
      /* This is bizarre - what ever would it mean? */
      break;

    case TERM_FLOATING_TENSE:
      /* A meaning for this can just about be envisaged :
         
         e.g. le finpe pe ca cu xamgu    would mean
              the fish associated with something current is good

         but is that what the language definition would say?

         */
      break;

    case TERMSET:
      /* ugh - the branches could differ, plus what would it mean
         anyway, there would be multiple terms in each branch. */
      break;

    case TERM_FLOATING_NEGATE:
      /* meaning? */
      break;

    case TERM_OTHER:
      break;

    default:
      break;
  }  
}


/*++++++++++++++++++++++++++++++++++++++
  Processing to decide whether to gloss NOI as 'such that' or as 'which'.

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_noi_clause (TreeNode *x)
{


}

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static void
process_any_rel_clause (TreeNode *x)
{
  TreeNode *rcs, *encl;
  TreeNode *antecedent;

  /* Find enclosing rule */
  rcs = x->parent;
  while (rcs->data.nonterm.type != RELATIVE_CLAUSES) {
    rcs = rcs->parent;
  }

  encl = rcs->parent;

  /* In several cases, it looks like the relative clause can come
     before the antecedent (postcedent? :-) ).  This needs to be
     confirmed. */

  switch (encl->data.nonterm.type) {
    case FRAGMENT:
      antecedent = NULL;
      break;
    case SUMTI:
      antecedent = child_ref(encl, 0); /* The sumti_1 node */
      break;
    case SUMTI_5:
      antecedent = child_ref(encl, 0); /* The sumti_5a or sumti_5b node */
      break;
    case LAHE_SUMTI_6:
      antecedent = find_nth_child(encl, 1, SUMTI);
      break;
    case NAHE_BO_SUMTI_6:
      antecedent = find_nth_child(encl, 1, SUMTI);
      break;
    case NAME_SUMTI_6:
      antecedent = encl;
      break;

    case SUMTI_TAIL:
      antecedent = find_nth_child(encl, 1, SUMTI_6);
      /* If there's a sumti_6 present I think the relative clause
         applies to it, otherwise it's like the case above - does it
         apply to the sumti_tail_1, or to something else??? */
      break;

    case SUMTI_TAIL_1:
      antecedent = child_ref(encl, 0); /* the sumti_tail_1A */
      break;

    case FREE_VOCATIVE:
      /* The relative clause applies to the whole thing (see text
         around example 9.6 in the reference manual.  (Strictly it
         applies to the selbri or CMENE_seq, but ignore that nicety
         for now.) */
      antecedent = encl;
      break;

    default:
      fprintf(stderr, "Shouldn't find relative clause in this context\n");
      antecedent = NULL;
      break;
  }

  /* Set the antecedent on the NOI to where it is, and put a property
     on the antecedent node to warn the bracketer that it needs to
     mark it. */
  if (antecedent) {
    XRequireBrac *xrb;
    XAntecedent *xan;

    xan = prop_antecedent(x, YES);
    xan->node = antecedent;

    xrb = prop_require_brac(antecedent, YES);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_keha(TreeNode *x)
{
  TreeNode *rel;

  /* Need to extend to deal with subscripted ke'a, where you have to
     back-count out of nested relative clauses. */

  rel = x->parent;
  while ((rel->data.nonterm.type != RELATIVE_CLAUSE) &&
         (rel->data.nonterm.type != CHUNKS)) {
    rel = rel->parent;
  }

  if (rel->data.nonterm.type == RELATIVE_CLAUSE) {
    /* Set property on ke'a to point back to containing clause */
    prop_rel_clause_link(x, YES)->rel = rel;

    /* Set property on containing clause to warn that it contains ke'a */
    prop_contains_keha (rel, YES);
  }

}

/*++++++++++++++++++++++++++++++++++++++
  Do processing for relative clauses.  The actions are as follows

  1. For GOI-type relative clauses, see if the term following is of
  the form <sumti> or <tag> <sumti>.  This determines whether to
  bother glossing the GOI cmavo in the text output.

  2. For NOI-type relative clauses, there is more to do.

  2a. Mark the NOI cmavo with a property to point back to the
  antecedent of the relative clause.

  2b. Look for each instance of ke'a in the text.  Look for a
  subscript on it.  Try to backtrack out to the associated relative
  clause, to point the ke'a at its antecedent.

  2c. If ke'a occurs in a relative clause, mark a property on the NOI
  cmavo.  This indicates it should be glossed as 'such that' rather
  than 'which'.  (Maybe this criterion needs to be based on whether
  any terms precede the selbri in the relative clause?  It gets
  awfully complex in some cases, e.g. if the relative clause is a
  <gek_sentence> because the 2 branches may be different in this
  regard.)



  TreeNode *top
  ++++++++++++++++++++++++++++++++++++++*/

void
relative_clause_processing (TreeNode *x)
{
  /* Traverse parse tree */
  
  int nc, i;
  struct nonterm *nt;
  TreeNode *c;

  if (x->type == N_NONTERM) {

    nt = &x->data.nonterm;

    /* Child scan first so that ke'a is processed before NOI */
    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      relative_clause_processing(c);
    }

    if (nt->type == RELATIVE_CLAUSE) {
      process_any_rel_clause(x);
    }

    if (nt->type == TERM_RELATIVE_CLAUSE) {
      process_goi_clause(x);
    } else if (nt->type == FULL_RELATIVE_CLAUSE) {
      process_noi_clause(x);
    } 

  } else if ((x->type == N_CMAVO) &&
             (!strcmp(cmavo_table[x->data.cmavo.code].cmavo, "ke\'a"))) {

    process_keha(x);

  }

}
