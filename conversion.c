/***************************************
  $Header$

  Work out how to gloss certain node types depending on the sequence
  of SE cmavo that precede them.  Work out the context that various
  selbri components occur in, to determine which gloss to use.
  ***************************************/

/* COPYRIGHT */

/*

  Perform handling to do with conversion (se,te etc)

*/

#include <assert.h>
#include "nodes.h"
#include "functions.h"
#include "rpc.tab.h"
#include "cmavotab.h"

/*++++++++++++++++++++++++++++++
  Lookup which place is exchanged for a cmavo of selma'o SE
  ++++++++++++++++++++++++++++++*/

static int
lookup_converted_place(TreeNode *se_cmavo)
{
  char *se;
  int conv;

  assert (se_cmavo->type == N_CMAVO);
  assert (se_cmavo->data.cmavo.selmao == SE);
  se = cmavo_table[se_cmavo->data.cmavo.code].cmavo;
  if (!strcmp(se, "se")) {
    conv = 2;
  } else if (!strcmp(se, "te")) {
    conv = 3;
  } else if (!strcmp(se, "ve")) {
    conv = 4;
  } else if (!strcmp(se, "xe")) {
    conv = 5;
  } else {
    assert(0); /* No other cmavo in selma'o SE hopefully */
  }
  return conv;
}


/*++++++++++++++++++++++++++++++
  Go through the parse tree and mark all SE BAI cmavo with extension
  records on the BAI.  Simple recursive traversal over the whole parse
  tree.
  ++++++++++++++++++++++++++++++*/

static void
conv_tag_se_bai(TreeNode *x)
{
  TreeNode *c, *c1, *c2;
  int nc, i;
  int conv;
  XBaiConversion *ext;
  XDontGloss *edg;
  struct nonterm *nt;

  if (x->type == N_NONTERM) {

    nt = &x->data.nonterm;

    switch (nt->type) {
      case SE_BAI:
        c1 = nt->children[0];
        c2 = nt->children[1];
        conv = lookup_converted_place(c1);
        ext = prop_bai_conversion(c2, YES);
        ext->conv = conv;

 /* Tag the SE cmavo so we don't display a gloss for it later. */
        edg = prop_dont_gloss(c1, YES);

        break;

      case BAI1:
        c1 = nt->children[0];
        ext = prop_bai_conversion(c1, YES);
        ext->conv = 1;
        break;
        
      default:
        /* Traverse down through children */
        nc = nt->nchildren;
        for (i=0; i<nc; i++) {
          c = nt->children[i];
          conv_tag_se_bai(c);
        }
        break;
    }

  } else {

    /* Nothing to do, the se_bai construction can only occur as a
       non-terminal */
  }
}


/*++++++++++++++++++++++++++++++

  Take a tanru_unit_2 node found by the downward scan, and work
  outwards gathering conversion operators.

  TreeNode *tu2 The tu2 node (ancestor of apply_to)

  TreeNode *apply_to The tu_2 node that wants the conversion applying
  to it when the glossing is done later.

  ++++++++++++++++++++++++++++++*/

static void
compute_tu2_conv(TreeNode *tu2, TreeNode *apply_to) {

  /* If the node already has a conversion on it, it must be something
     whose conversion was determined in terms.c processing. */

  if (prop_conversion(apply_to, NO)) {
    return;
  } else {
    /* places[1] is which place of the base word the x1 of the bridi
       corresponds to.  zero means x1 is a jai construction. */
    int places[6];
    int i;
    TreeNode *x = tu2;

    for (i=1; i<=5; i++) {
      places[i] = i;
    }

    /* Barf on JAI constructions for now, fix that up later */

    for (;;) {
      switch (x->data.nonterm.type) {
        
        case SE_TU2:
          {
            int p, t;
            XDontGloss *edg;
            TreeNode *se = child_ref(x, 0);
            p = lookup_converted_place(se);
            t = places[1];
            places[1] = places[p];
            places[p] = t;
            edg = prop_dont_gloss(se, YES);
          }
        x = x->parent;
        break;

        /* This block just need to go up a level and keep scanning */
        case TANRU_UNIT_2:
        case NAHE_TU2:
        case TANRU_UNIT_1:
        case TANRU_UNIT:
        case SELBRI_6:
        case SELBRI_5: 
        case SELBRI_3:
        case KE_SELBRI_3:
        case KE_SELBRI3_TU2:
          x = x->parent;
          break;
          
        case JAI_TU2:
        case JAI_TAG_TU2:
          /* current x1 of the bridi lost into the fai place, no way
             to recover this later (no SE which exchanges fai with a
             normal place!). */
          places[1] = 0;
          x = x->parent;
          break;
          
        case SELBRI_4:
          /* Depends on whether we're immediately below an outermost
             selbri_3 as to whether we close the conversion now or
             propagate. */
          if ((x->parent->        data.nonterm.type == SELBRI_3) &&
              (x->parent->parent->data.nonterm.type == SELBRI_3)) {
            /* close */
            XConversion *ext;
            ext = prop_conversion(apply_to, YES);
            ext->conv = places[1];
            return;
          } else {
            x = x->parent;
          }
          break;
          
        case SELBRI_2:
          /* done in all cases */
          {
            XConversion *ext;
            ext = prop_conversion(apply_to, YES);
            ext->conv = places[1];
            return;
          }
        break;
        default:
          fprintf(stderr, "Unexpected case!!\n");
          assert(0);
          break;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++
  Scan down the tree looking for BRIVLA, abstractions and other forms
  of tanru_unit_2 that may be subject to conversion (SE) operators.
  Work back up the syntax tree, accumulating conversions, until coming
  to something that indicates no more conversions apply to this
  tanru_unit_2.

  ++++++++++++++++++++++++++++++*/

static void
conv_mark_tu2s(TreeNode *x)
{
  int nc, i;
  TreeNode *c;
  struct nonterm *nt;

  nt = &x->data.nonterm;
  
  if (x->type == N_NONTERM) {
    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      conv_mark_tu2s(c);
    }
  } else if (x->type == N_BRIVLA) {
    compute_tu2_conv(x->parent, x);

  } else if ((x->type == N_CMAVO) &&
             (x->data.cmavo.selmao == NU)) {
    TreeNode *y;
    y = x->parent;
    assert(y->type == N_NONTERM);
    while (y->data.nonterm.type != TANRU_UNIT_2) {
      y = y->parent;
      assert(y->type == N_NONTERM);
    }
    compute_tu2_conv(y, x);

  }

}


/*+ State used to track the nearest enclosing ancestor node which
  determines the form +*/
typedef enum {
  GS_NONE,
  GS_MAIN_SELBRI,
  GS_SUMTI,
  GS_JAI,
  GS_MODAL
} GlossState;

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
conv_mark_gloss_types(TreeNode *x, GlossState g)
{
  struct nonterm *nt;
  int nc, i;
  TreeNode *c;
  XGlosstype *gt;
  GlossState ng;

  if (x->type == N_NONTERM) {

#if 0
    fprintf(stderr, "MGT called on nonterm ty=%s gs=%d\n", nonterm_names[x->data.nonterm.type], g);
#endif

    nt = &x->data.nonterm;
    nc = nt->nchildren;

    switch (nt->type) {
      case MAIN_SELBRI:
      case METALINGUISTIC_MAIN_SELBRI:
        ng = GS_MAIN_SELBRI;
        break;
      case JAI_TAG_TU2:
      case JAI_TU2:
        {
          TreeNode *cjai;
          cjai = child_ref(x, 0);
          if (g != GS_NONE) {
            gt = prop_glosstype(cjai, YES);
            if (g == GS_MAIN_SELBRI || g == GS_JAI) {
              gt->in_selbri = 1;
            } else {
              gt->in_selbri = 0;
            }
          }
        }
        ng = GS_JAI;
        break;
      case SUMTI:
        ng = GS_SUMTI;
        break;
      case FREE:
      case OPERAND_3:
      case MEX_OPERATOR:
        ng = GS_NONE; /* Review later what to do with these ones */
        break;
      case COMPLEX_TENSE_MODAL:
        ng = GS_MODAL;
        break;
      default:
        ng = g;
        break;
    }

    for (i=0; i<nc; i++) {
      c = nt->children[i];
      conv_mark_gloss_types(c, ng);
    }

  } else {

#if 0
    if (x->type == N_BRIVLA) {
      fprintf(stderr, "MGT called on brivla %s gs=%d\n", x->data.brivla.word, g);
    }

    if (x->type == N_CMAVO) {
      fprintf(stderr, "MGT called on cmavo %s gs=%d\n", cmavo_table[x->data.cmavo.code].cmavo, g);

    }
#endif

    if ((x->type == N_BRIVLA) ||
        ((x->type == N_CMAVO) &&
         (x->data.cmavo.selmao == NU))) {
      if (g != GS_NONE) {
        gt = prop_glosstype(x, YES);
        /* In something like 'le jai ca cusku' we want to treat
           'cusku' as a 'verb' rather than a 'noun' when glossing */
        if (g == GS_MAIN_SELBRI || g == GS_JAI) {
          gt->in_selbri = 1;
        } else {
          gt->in_selbri = 0;
        }
      }
    }
      
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Scan the entire parse tree for selbri.  Descend into each one to
  work out which tanru_unit_2's inside it correspond to tertau',
  i.e. need to be considered as nouns or verbs (rather than
  adjectives) for glossing.

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
selbri_scan(TreeNode *x, int is_tertau)
{
  struct nonterm *nt;
  int nc, i;
  TreeNode *c;

  nt = &x->data.nonterm;
  nc = nt->nchildren;

  if (x->type == N_NONTERM) {

    nt = &x->data.nonterm;
    nc = nt->nchildren;

    if (is_tertau) {

      switch (nt->type) {
        case SELBRI:
          break;
        case SELBRI_1:
          break;
        case SELBRI_2:
          break;
        case SELBRI_3:
          break;
        case SELBRI_4:
          {
            TreeNode *cs4, *cs5, *cks3, *cs3;
            cs4 = x;
            while (nch(cs4) > 1) {
              cs5 = find_nth_child(cs4, 1, SELBRI_5);
              cks3 = find_nth_child(cs4, 1, KE_SELBRI_3);
              if (cs5) {
                selbri_scan(cs5, 1);
              }
              if (cks3) {
                cs3 = find_nth_child(cks3, 1, SELBRI_3);
                /* This is like a new selbri_3, only the last selbri_4
                   has tertau status, with the prefixes being
                   adjectival */
                selbri_scan(cs3, 0);
              }
              cs4 = find_nth_child(cs4, 1, SELBRI_4);
            }

            /* Mop up final child */
            cs5 = find_nth_child(cs4, 1, SELBRI_5);
            selbri_scan(cs5, 1);

          }
          break;
        case SELBRI_5:
          {
            TreeNode *cs5, *cs6;
            cs5 = x;
            do {
              cs6 = find_nth_child(cs5, 1, SELBRI_6);
              selbri_scan(cs6, 1);
              cs5 = find_nth_child(cs5, 1, SELBRI_5);
            } while (cs5);
          }
          break;
        case SELBRI_6:
          {
            TreeNode *cs6, *tu;
            cs6 = x;
            do {
              tu = find_nth_child(cs6, 1, TANRU_UNIT);
              if (tu) {
                selbri_scan(tu, 1);
              }

              /* There is a clever hack here.  In the NAhE guhek
                 selbri gik selbri_6 case, we don't need to bother
                 about processing the selbri - the selbri_3 within it
                 will get picked up eventually anyway by scanning the
                 whole tree for selbri_3's. */

              cs6 = find_nth_child(cs6, 1, SELBRI_6);
            } while (cs6);
          }
          break;
        case TANRU_UNIT:
          {
            TreeNode *ctu1, *ctu;
            ctu = x;
            do {
              ctu1 = find_nth_child(ctu, 1, TANRU_UNIT_1);
              selbri_scan(ctu1, 1);
              ctu =  find_nth_child(ctu, 1, TANRU_UNIT);
            } while (ctu);
          }
          break;
        case TANRU_UNIT_1:
          {
            TreeNode *ctu2;
            ctu2 = find_nth_child(x, 1, TANRU_UNIT_2);
            selbri_scan(ctu2, 1);
          }
          break;
        case TANRU_UNIT_2:
          {
            TreeNode *c;
            c = child_ref(x, 0);
            if (c->type == N_BRIVLA) {
              XGlosstype *gt;
              gt = prop_glosstype(c, YES);
              gt->is_tertau = 1;
            } else if (c->type == N_NONTERM) {
              switch (c->data.nonterm.type) {
                case SE_TU2:
                case JAI_TU2:
                case NAHE_TU2:
                  {
                    TreeNode *cc;
                    cc = find_nth_child(c, 1, TANRU_UNIT_2);
                    selbri_scan(cc, 1);
                  }
                  break;
                case JAI_TAG_TU2:
                  {
                    TreeNode *cc, *cjai;
                    XGlosstype *gt;

                    cjai = child_ref(c, 0);
                    gt = prop_glosstype(cjai, YES);
                    gt->is_tertau = 1;

                    cc = find_nth_child(c, 1, TANRU_UNIT_2);
                    selbri_scan(cc, 1);
                  }
                  break;

                case ABSTRACTION:
                  {
                    TreeNode *nns;
                    TreeNode *nu;
                    XGlosstype *gt;
                    nns = child_ref(c, 0);
                    while (nns) {
                      nu = find_nth_cmavo_child(nns, 1, NU);
                      gt = prop_glosstype(nu, YES);
                      gt->is_tertau = 1;
                      nns = find_nth_child(nns, 1, NU_NAI_SEQ);
                    }
                  }
                break;

                
                default:
                  break;
              }
            }
          }

        break;

        default:
          break;
      }
    } else {
      /* If it's a selbri_3 that hasn't been scanned yet, need to do
         it.  The ideas is that in a selbri_2, the tertau is the last
         selbri_4 term within the selbri_3 term either at the head, or
         before the first CO.  This selbri_4 is the last term of the
         outermost selbri_3, if it's a tanru with multiple
         components. */

      if (nt->type == SELBRI_3 && !prop_done_s3(x, NO)) {
        /* Find final selbri_4 */
        TreeNode *cs4, *cs3;
        XDoneS3 *xds3;
#if 0
        fprintf(stderr, "Here B\n");
#endif
        cs3 = x;
        cs4 = find_nth_child(cs3, 1, SELBRI_4);
        do {
          xds3 = prop_done_s3(cs3, YES);
          cs3 = find_nth_child(cs3, 1, SELBRI_3);
        } while (cs3);

        /* Actively scan down into selbri4 */
#if 0
        fprintf(stderr, "Here A\n");
#endif
        selbri_scan(cs4, 1);
      }
    }

    /* In all cases, scan downwards to look for other selbri that haven't been tackled yet */
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      selbri_scan(c, 0);
    }

  } else {
    /* Nothing to do */
  }


}


/*++++++++++++++++++++++++++++++
  Do all conversion stages
  ++++++++++++++++++++++++++++++*/

void
do_conversions(TreeNode *top)
{
  conv_tag_se_bai(top);
  conv_mark_tu2s(top);
  conv_mark_gloss_types(top, GS_NONE);
  selbri_scan(top, 0);
}
