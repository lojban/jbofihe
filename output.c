/***************************************
  $Header$

  Generate glossed output, calling the appropriate backend (latex,
  text, html etc).
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "nodes.h"
#include "functions.h"
#include "cmavotab.h"
#include "rpc.tab.h"
#include "output.h"

static DriverVector *drv;

typedef enum {
  SHOW_NONE,
  SHOW_LOJBAN,
  SHOW_ENGLISH,
  SHOW_TAG_TRANS,
  SHOW_BOTH
} WhatToShow;

/*+ Forward prototype +*/
static void output_internal(TreeNode *x, WhatToShow what);

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
add_bracketing_internal(TreeNode *x, int *seq)
{
  struct nonterm *y;
  int i, n;

  if (x->type == N_NONTERM) {
    y = &x->data.nonterm;
    y->number = 0;
    y->brackets = BR_NONE;

    switch (y->type) {
      case TERM:
        y->number = ++*seq;
        y->brackets = BR_ROUND;
        break;
      case SUMTI:
      case SUMTI_1:
      case SUMTI_2:
      case SUMTI_3:
      case SUMTI_4:
      case SUMTI_5:
      case SUMTI_6:
        if (y->nchildren > 1) {
          y->number = ++*seq;
          y->brackets = BR_CEIL;
        }
        break;

      case SELBRI_1:
      case SELBRI_2:
      case SELBRI_3:
      case SELBRI_4:
      case SELBRI_5:
      case SELBRI_6:
        if (y->nchildren > 1) {
          y->number = ++*seq;
          y->brackets = BR_FLOOR;
        }
        break;

      case MAIN_SELBRI:
        y->number = ++*seq;
        y->brackets = BR_TRIANGLE;
        break;

      case SENTENCE:
      case FRAGMENT:
        y->number = ++*seq;
        y->brackets = BR_SQUARE;
        break;

      case RELATIVE_CLAUSES:
        /* Drill to relative_clause_seq inside it */
        if (y->children[0]->data.nonterm.nchildren > 1) {
          y->number = ++*seq;
          y->brackets = BR_ROUND;
        }
        break;

      case RELATIVE_CLAUSE:
        y->number = ++*seq;
        y->brackets = BR_ANGLE;
        break;

      case ABSTRACTION:
        y->number = ++*seq;
        y->brackets = BR_ANGLE;
        break;

      case TANRU_UNIT_1:
        if (y->nchildren > 1) {
          y->number = ++*seq;
          y->brackets = BR_BRACE;
        }
        break;
        
      default:
        y->number = 0;
        y->brackets = BR_NONE;
    }

    n = y->nchildren;
    for (i=0; i<n; i++) {
      add_bracketing_internal(y->children[i], seq);
    }

  }

}

/*++++++++++++++++++++++++++++++
  Go through parse tree and mark specific non-terminals with
  bracketing type and sequence number
  ++++++++++++++++++++++++++++++*/

void
add_bracketing_tags(TreeNode *top)
{
  int seq = 0;

  add_bracketing_internal(top, &seq);

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x

  char *loj

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_bai (TreeNode *x, char *eng)
{
  XBaiConversion *baiconv;
  XTenseCtx *xtc;
  char buffer[1024], *trans;

  eng[0] = 0;
  buffer[0] = 0;
  baiconv = prop_bai_conversion(x, NO);
  xtc = prop_tense_ctx(x, NO);

  if (baiconv) {
    static char *prefixes[] = {"se", "te", "ve", "xe"};
    strcat(buffer, prefixes[baiconv->conv - 2]);
  }
  strcat(buffer, cmavo_table[x->data.cmavo.code].cmavo);

  if (xtc) {
    switch (xtc->ctx) {
      case TSC_OTHER:
        break;
      case TSC_SELBRI:
        break;
      case TSC_TERM:
        break;
      case TSC_NOUN:
        break;
      case TSC_LINK:
        break;
      case TSC_CONNECT:
        break;
      case TSC_JAITAG:
        strcat(buffer,"@JAI");
        break;
    }
  }

  trans = translate(buffer);
  if (trans) {
    strcpy(eng, trans);
  } else {
    trans = translate(cmavo_table[x->data.cmavo.code].cmavo);
    if (trans) {
      strcpy(eng, trans);
      strcat(eng, " (CONV?)");
    } else {
      strcpy(eng, "?");
    }
  }

}


/*++++++++++++++++++++++++++++++++++++++

  TreeNode *x

  char *loj

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_se (TreeNode *x, char *eng)
{
  char *trans;

#if 0
  fprintf(stderr, "check dg %08lx\n", x);
#endif
  if (prop_dont_gloss(x, NO)) {
    eng[0] = 0;
  } else {
    trans = translate(cmavo_table[x->data.cmavo.code].cmavo);
    if (trans) {
      strcpy(eng, trans);
    } else {
      eng[0] = 0;
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++

  TreeNode *x

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_other_cmavo (TreeNode *x, char *eng)
{
  char *trans;
  
  trans = translate(cmavo_table[x->data.cmavo.code].cmavo);
  if (trans) {
    strcpy(eng, trans);
  } else {
    eng[0] = 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x

  char *loj

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_indicator (TreeNode *x, char *loj, char *eng)
{
  char *trans;
  int negated;
  char buffer[256];
  XCaiIndicator *xci;

  if (prop_neg_indicator(x, NO)) {
    negated = 1;
  } else {
    negated = 0;
  }

  xci = prop_cai_indicator(x, NO);
  if (xci) {
    switch (xci->code) {
      case CC_CAI:
        strcpy(buffer, translate("cai"));
        break;
      case CC_SAI:
        strcpy(buffer, translate("sai"));
        break;
      case CC_RUhE:
        strcpy(buffer, translate("ru'e"));
        break;
      case CC_CUhI:
        strcpy(buffer, translate("cu'i"));
        break;
      case CC_RUhENAI:
        strcpy(buffer, translate("ru'enai"));
        break;
      case CC_SAINAI:
        strcpy(buffer, translate("sainai"));
        break;
      case CC_CAINAI:
        strcpy(buffer, translate("cainai"));
        break;
    }
    strcat(buffer, " ");
  } else {
    buffer[0] = 0;
  }

  strcpy(loj, cmavo_table[x->data.cmavo.code].cmavo);
  if (negated) {
    strcat(loj, "nai");
  }
  trans = translate(loj);
  if (trans) {
    strcat(buffer, trans);
    strcpy(eng, buffer);
  } else {
    eng[0] = 0;
  }

  if (xci) {
    switch (xci->code) {
      case CC_CAI:
        strcat(loj, " cai");
        break;
      case CC_SAI:
        strcat(loj, " sai");
        break;
      case CC_RUhE:
        strcat(loj, " ru'e");
        break;
      case CC_CUhI:
        strcat(loj, " cu'i");
        break;
      case CC_RUhENAI:
        strcat(loj, " ru'enai");
        break;
      case CC_SAINAI:
        strcat(loj, " sainai");
        break;
      case CC_CAINAI:
        strcat(loj, " cainai");
        break;
    }
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  static char * translate_tense_in_context

  char *text

  enum tense_contexts ctx
  ++++++++++++++++++++++++++++++++++++++*/

static char *
translate_tense_in_context(char *text, enum tense_contexts ctx)
{
  char buffer[128];
  char *trans;

  strcpy(buffer, text);

  switch (ctx) {
    case TSC_OTHER:
      break;
    case TSC_TERM:
      strcat(buffer, "@TERM");
      break;
    case TSC_NOUN:
      strcat(buffer, "@NOUN");
      break;
    case TSC_SELBRI:
      strcat(buffer, "@SELBRI");
      break;
    case TSC_LINK:
      strcat(buffer, "@LINK");
      break;
    case TSC_CONNECT:
      strcat(buffer, "@CONNECT");
      break;
    case TSC_JAITAG:
      strcat(buffer, "@JAI");
      break;
  }
  trans = translate(buffer);
  if (trans) {
    return trans;
  } else {
    trans = translate(text);
    return trans; /* tough if null */
  }
}


/*++++++++++++++++++++++++++++++++++++++

  TreeNode *x

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_tense (TreeNode *x, char *eng)
{
  XTenseCtx *ctx;
  char buffer[128], *trans;

  eng[0] = 0;
  ctx = prop_tense_ctx(x, NO);
  if (ctx) {
    strcpy(buffer, cmavo_table[x->data.cmavo.code].cmavo);
    trans = translate_tense_in_context(buffer, ctx->ctx);
    if (trans) {
      strcpy(eng, trans);
    } else {
      fprintf(stderr, "No advanced translation found for %s\n", buffer);
      translate_other_cmavo(x, eng);
    }
  } else {
    fprintf(stderr, "No context found for tense cmavo at line %d column %d\n",
            x->start_line, x->start_column);
    translate_other_cmavo(x, eng);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Process nonterminals of type time_offset.

  TreeNode *x

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_time_offset (TreeNode *x, char *loj, char *eng)
{
  XTenseCtx *ctx;
  char *trans;
  TreeNode *pu, *nai, *zi;

  pu = find_nth_cmavo_child(x, 1, PU);
  nai = find_nth_cmavo_child(x, 1, NAI);
  zi = find_nth_cmavo_child(x, 1, ZI);

  assert(pu);

  eng[0] = 0;
  ctx = prop_tense_ctx(x, NO);

  /* Try for the whole thing */
  strcpy(loj, cmavo_table[pu->data.cmavo.code].cmavo);
  if (nai) {
    strcat(loj, cmavo_table[nai->data.cmavo.code].cmavo);
  }
  if (zi) {
    strcat(loj, cmavo_table[zi->data.cmavo.code].cmavo);
  }

  if (ctx) {
    trans = translate_tense_in_context(loj, ctx->ctx);
    if (trans) {
      strcpy(eng, trans);
    } else {
      fprintf(stderr, "No advanced translation found for %s\n", loj);
      translate_other_cmavo(x, eng);
    }
  } else {
    fprintf(stderr, "No context found for tense cmavo at line %d column %d\n",
            x->start_line, x->start_column);
    trans = translate(loj);
  }

}

/*++++++++++++++++++++++++++++++++++++++

  TreeNode *x

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_jai (TreeNode *x, char *eng)
{
  XGlosstype *xgt;
  char buffer[128], *trans;

  strcpy(buffer, cmavo_table[x->data.cmavo.code].cmavo);
  eng[0] = 0;
  xgt = prop_glosstype(x, NO);
  if (xgt) {
    if (xgt->in_selbri) {
      if (xgt->is_tertau) {
        strcat(buffer, "_v");
      } else {
        strcat(buffer, "_a");
      }
    } else {
      if (xgt->is_tertau) {
        strcat(buffer, "_n");
      } else {
        strcat(buffer, "_a");
      }
    }
  } else {
    strcat(buffer, "_a");
  }

  trans = translate(buffer);
  if (trans) {
    strcpy(eng, trans);
  } else {
    fprintf(stderr, "No advanced translation found for %s\n", buffer);
    translate_other_cmavo(x, eng);
  }
}



/*++++++++++++++++++++++++++++++++++++++

  TreeNode *x

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_brivla (TreeNode *x, char *eng)
{
  char buffer[1024];
  char *trans;
  XConversion *conversion;
  XGlosstype *gt;
  int conv;

  eng[0] = 0;
  conversion = prop_conversion(x, NO);
  if (conversion) {
    conv = conversion->conv;
  } else {
    conv = 1;
  }
  gt = prop_glosstype(x, NO);
  if (gt) {
    if (gt->in_selbri) {
      if (gt->is_tertau) {
        trans = adv_translate(x->data.brivla.word, conv, TCX_VERB);
      } else {
        trans = adv_translate(x->data.brivla.word, conv, TCX_QUAL);
      }
    } else {
      /* In a sumti */
      if (gt->is_tertau) {
        trans = adv_translate(x->data.brivla.word, conv, TCX_NOUN);
      } else {
        trans = adv_translate(x->data.brivla.word, conv, TCX_QUAL);
      }
    }
    if (trans) {
      strcpy(eng, trans);
    } else {
      eng[0] = 0;
    }
  } else {
    sprintf(buffer, "%s%1d", x->data.brivla.word, conv);
    trans = translate(buffer);
    if (trans) {
      strcpy(eng, trans);
    } else {
      trans = translate(x->data.brivla.word);
      if (trans) {
        strcpy(eng, trans);
        if (conversion) {
          /* If we had to resort to a bare translation and there was
             a SE prefix, something is wrong. */
          strcat(eng, " (CONV?)");
        }
      } else {
        trans = translate_unknown(x->data.brivla.word);
        if (trans) {
          strcpy(eng, trans);
        } else {
          eng[0] = 0;
        }
      }
    }
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
get_lojban_word_and_translation (TreeNode *x, char *loj, char *eng)
{

  switch (x->type) {
    
    case N_CMAVO:
      strcpy(loj, cmavo_table[x->data.cmavo.code].cmavo);
      eng[0] = 0;
      switch (x->data.cmavo.selmao) {
        case BAI:
          translate_bai(x, eng);
          break;

        case SE:
          translate_se(x, eng);
          break;

        case PU:
        case ZAhO:
        case VA:
          translate_tense(x, eng);
          break;
          
        case UI:
        case CAI:
          translate_indicator(x, loj, eng);
          break;

        case JAI:
          translate_jai(x, eng);
          break;

        default:
          translate_other_cmavo(x, eng);
      }

      break;

    case N_ZOI:
      sprintf(loj, "zoi %s %s %s", x->data.zoi.term, x->data.zoi.text, x->data.zoi.term);
      eng[0] = 0;
      break;
      
    case N_ZO:
      sprintf(loj, "zo %s", x->data.zo.text);
      eng[0] = 0;
      break;
      
    case N_LOhU:
      sprintf(loj, "lo'u %s le'u", x->data.lohu.text);
      eng[0] = 0;
      break;

    case N_BU:
      sprintf(loj, "%s bu", x->data.bu.word);
      eng[0] = 0;
      break;
        
    case N_BRIVLA:
      strcpy(loj, x->data.brivla.word);
      translate_brivla(x, eng);
      break;
      
    case N_CMENE:
      strcpy(loj, x->data.cmene.word);
      eng[0] = 0;
      break;
      
    default:
      break;
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static void
output_term(TreeNode *x, WhatToShow what)
{
  XTermTags *xtt;
  char *trans;
  char tp[16];
  int n, i;
  struct nonterm *y;
  y = &x->data.nonterm;

  if (what == SHOW_LOJBAN || what == SHOW_BOTH) {
    xtt = prop_term_tags(x, NO);
    if (xtt) {
      XTermTag *tag;
      (drv->start_tags)();
      do {
        tag = &(xtt->tag);
        switch (tag->type) {
          case TTT_BRIVLA:
            trans = adv_translate(tag->brivla.x->data.brivla.word, tag->pos, TCX_TAG);
            if (!trans) trans = "?";
            sprintf(tp, "%d", tag->pos);
            (drv->write_tag_text)(tag->brivla.x->data.brivla.word, tp, trans, YES);
            break;
          case TTT_JAITAG:
            (drv->start_tag)();
            output_internal(tag->jaitag.tag, SHOW_TAG_TRANS);
            trans = adv_translate(tag->brivla.x->data.brivla.word, tag->pos, TCX_VERB);
            sprintf(tp, "%d", tag->pos);
            if (trans) {
              (drv->write_tag_text) ("", "", trans, NO);
            }
            break;
          default:
            break;
        }
        xtt = xtt->next;
      } while (xtt);
      
      (drv->end_tags)();
    }
  }
  n = y->nchildren;
  for (i=0; i<n; i++) {
    output_internal(y->children[i], what);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static void
output_simple_time_offset(TreeNode *x, WhatToShow what)
{
  char loj[1024], eng[1024];

  loj[0] = 0;
      
  translate_time_offset(x, loj, eng);
  switch (what) {
    case SHOW_LOJBAN:
    case SHOW_BOTH:
      (drv->lojban_text)(loj);
      break;
    default:
      break;
  }
  switch (what) {
    case SHOW_ENGLISH:
    case SHOW_BOTH:
      /* Translation */
      if (eng[0]) {
        (drv->translation)(eng);
      }
      (drv->set_eols)(x->eols);
      break;

    case SHOW_TAG_TRANS:
      if (eng[0]) {
        (drv->write_tag_text)("", "", eng, NO);
      }
      break;      

    default:
      break;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Special output processing for <sumti_tail>.  See if there's a
  <sumti_6> at the start.  If so, do a special gloss to turn it into a
  genetive form.

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
output_sumti_tail(TreeNode *x, WhatToShow what)
{
  int n, i;
  struct nonterm *y;
  TreeNode *c1;

  y = &x->data.nonterm;

  c1 = y->children[0];
  n = y->nchildren;
  if (c1->data.nonterm.type == SUMTI_6) {
    switch (what) {
      case SHOW_LOJBAN:
      case SHOW_BOTH:
        output_internal(c1, SHOW_LOJBAN);
        break;
      default:
        break;
    }
    for (i=1; i<n; i++) {
      output_internal(y->children[i], what);
    }
    switch (what) {
      case SHOW_ENGLISH:
      case SHOW_BOTH:
        (drv->translation)("[of]");
        output_internal(c1, SHOW_ENGLISH);
        break;
      default:
        break;
    }
  } else {
    for (i=0; i<n; i++) {
      output_internal(y->children[i], what);
    }
  }
}      


/*++++++++++++++++++++++++++++++++++++++
  Handle the various types of jek, ek and gihek non-terminal.  In
  connect.c, we have worked out the truth function being expressed and
  stored it on a property.

  TreeNode *x

  WhatToShow what
  ++++++++++++++++++++++++++++++++++++++*/

static void
output_fore_or_afterthought(TreeNode *x, WhatToShow what)
{
  XConnective *xcon;
  struct nonterm *y;
  int n, i;
  TreeNode *c;
  char buffer[64];
  char *trans;

  xcon = prop_connective(x, NO);

  y = &x->data.nonterm;

  n = y->nchildren;
  if ((what == SHOW_LOJBAN) ||
      (what == SHOW_BOTH)) {
    for (i=0; i<n; i++) {
      c = y->children[i];
      output_internal(c, SHOW_LOJBAN);
      /* Extend to do attitudinals later */
    }
  }

  /* Now output meaning */
  if ((what == SHOW_ENGLISH) ||
      (what == SHOW_TAG_TRANS) ||
      (what == SHOW_BOTH)) {
    assert(xcon);
    switch (xcon->pos) {
      case CNP_AFTER:
      case CNP_GE:
      case CNP_GI:

        strcpy(buffer, xcon->pattern);
        switch (xcon->pos) {
          case CNP_AFTER:
            strcat(buffer, "@AFTER"); break;
          case CNP_GE:
            strcat(buffer, "@GE"); break;
          case CNP_GI:
            strcat(buffer, "@GI"); break;
          default:
            abort();
        }
        trans = translate(buffer);
        if (trans) {
          if (what == SHOW_TAG_TRANS) {
            (drv->write_tag_text)("", "", trans, NO);
          } else {
            (drv->translation)(trans);
          }
        }

      case CNP_GE_STAG:
      case CNP_GE_JOIK:
        break; /* Don't put anything here */

      case CNP_GI_STAG:
      case CNP_GI_JOIK:
        /* Output the stag or joik as though it occurred at the
           position of the gik in the middle of the sentence.  Need to
           think further about how to handle NAI and any kind of
           context dependent stuff.  The problem is that stag can be
           infinitely recursive, so this is kind of hard to do. */

        if (what == SHOW_TAG_TRANS) {
          output_internal(xcon->js, SHOW_TAG_TRANS);
        } else {
          output_internal(xcon->js, SHOW_ENGLISH);
        }
    }
  }

}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
output_internal(TreeNode *x, WhatToShow what)
{
  struct nonterm *y;
  char loj[1024], eng[1024];
  int i, n;

  if (x->type == N_NONTERM) {
    y = &x->data.nonterm;

    if (what == SHOW_BOTH || what == SHOW_LOJBAN) {
      (drv->open_bracket)(y->brackets, y->number);
    }

    if (y->type == SELBRI_3 &&
        y->nchildren == 2) {
      /* Special handling */

      output_internal(y->children[0], what);
      switch (what) {
        case SHOW_ENGLISH:
        case SHOW_BOTH:
          (drv->translation)("[type-of]");
          break;
        default:
          break;
      }
          
      output_internal(y->children[1], what);

    } else if (y->type == TERM) {

      output_term(x, what);
      
    } else if (y->type == NO_CU_SENTENCE) {
      /* Special handling */

      output_internal(y->children[0], what);
#if 1
      switch (what) {
        case SHOW_ENGLISH:
        case SHOW_BOTH:
          (drv->translation)("[is, does]");
          break;
        default:
          break;
      }
          
#endif
      output_internal(y->children[1], what);


    } else if (y->type == OBSERVATIVE_SENTENCE) {
      /* Special handling */

#if 0
      /* This doesn't work too well in various cases, e.g. abstractors
         without an x1 before the selbri, and particular in poi/noi
         clauses.  Leave out until this can be looked into. */
      (drv->translation)("(there is)");
#endif
      output_internal(y->children[0], what);

    } else if ((y->type == TIME_OFFSET) && is_simple_nonterm(x)) {

      output_simple_time_offset(x, what);

    } else if (y->type == SUMTI_TAIL) {

      output_sumti_tail(x, what);

    } else if (((y->type == SUMTI_5) &&
                (y->children[0]->data.nonterm.type == QUANTIFIER) &&
                (y->children[1]->data.nonterm.type == SUMTI_6)) ||
               ((y->type == SUMTI_TAIL_1) &&
                (y->children[0]->data.nonterm.type == QUANTIFIER) &&
                (y->children[1]->data.nonterm.type == SUMTI))) {
      int i, n;
      
      output_internal(y->children[0], what);

      if (what == SHOW_ENGLISH || what == SHOW_BOTH) {
        (drv->translation)("(of)");
      }

      n = y->nchildren;

      for (i=1; i<n; i++) {
        output_internal(y->children[i], what);
      }

    } else if ((y->type == JEK) ||
               (y->type == JEK_OPT_KE) ||
               (y->type == JEK_OPT_KEBO) ||
               (y->type == EK) ||
               (y->type == GIHEK) ||
               (y->type == GEK) ||
               (y->type == GIK) ||
               (y->type == GUHEK)) {
      
      output_fore_or_afterthought(x, what);

    } else {

      n = y->nchildren;
      for (i=0; i<n; i++) {
        output_internal(y->children[i], what);
      }
      
    }

    if (what == SHOW_BOTH || what == SHOW_LOJBAN) {
      (drv->close_bracket)(y->brackets, y->number);
    }

  } else {
    /* Terminal token */
    char lojbuf[1024];

    lojbuf[0] = 0;

    get_lojban_word_and_translation(x, loj, eng);
    if (strchr("aeiou", loj[0])) {
      strcat(lojbuf, ".");
    }
    strcat(lojbuf, loj);
    if (x->type == N_CMENE) {
      strcat(lojbuf, ".");
    }

    switch (what) {
      case SHOW_LOJBAN:
      case SHOW_BOTH:
        (drv->lojban_text)(lojbuf);
        break;
      default:
        break;
    }
    
    /* Translation */
    switch (what) {
      case SHOW_ENGLISH:
      case SHOW_BOTH:
        if (eng[0]) {
          (drv->translation)(eng);
        }
        (drv->set_eols)(x->eols);
        break;

      case SHOW_TAG_TRANS:
        if (eng[0]) {
          (drv->write_tag_text)("", "", eng, NO);
        }
        (drv->set_eols)(x->eols);
        break;
      default:
        break;
    }


  }

}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

void
do_output(TreeNode *top, DriverVector *driver)
{
  drv = driver;

  drv->initialise();
  drv->prologue();

  output_internal(top, SHOW_BOTH);

  drv->epilogue();
}
