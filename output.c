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
#include "rpc_tab.h"
#include "output.h"

static DriverVector *drv;

typedef enum {
  SHOW_NONE,
  SHOW_LOJBAN,                  /* Show just the lojban word */
  SHOW_ENGLISH,                 /* Show just the translation */
  SHOW_TAG_TRANS,               /* Show just the translation of a tag */
  SHOW_LOJBAN_AND_INDICATORS,   /* Show just lojban for most, but both lojban and translation for indicators */
  SHOW_BOTH                     /* Show lojban and translation */
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

      case SUMTI_2:
      case SUMTI_3:
      case SUMTI_4:
      case SUMTI_5:
        if (y->nchildren > 1) {
          y->number = ++*seq;
          y->brackets = BR_CEIL;
        }
        break;

      case SUMTI_5A:
      case SUMTI_5B:
      case NAME_SUMTI_6:
        if (prop_require_brac(x, NO)) {
          y->number = ++*seq;
          y->brackets = BR_CEIL;
        }
        break;

      case SUMTI:
      case SUMTI_1:
      case LAHE_SUMTI_6:
      case NAHE_BO_SUMTI_6:
      case FREE_VOCATIVE:
        if ((y->nchildren > 1) ||
            (prop_require_brac(x, NO))) {
          y->number = ++*seq;
          y->brackets = BR_CEIL;
        }
        break;

      case SUMTI_TAIL_1A:
        if (prop_require_brac(x, NO)) {
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
      case METALINGUISTIC_MAIN_SELBRI:
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
        
      case FREE:
        y->number = ++*seq;
        y->brackets = BR_ROUND;
        break;

      case NUMBER:
      case LERFU_STRING:
      case MEX_OPERATOR:
        if (prop_require_brac(x, NO)) {
          y->number = ++*seq;
          y->brackets = BR_ROUND;
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

  } else if (x->type == N_ZEI) {
    x->data.zei.number = ++*seq;
    x->data.zei.brackets = BR_FLOOR;
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
    static char *prefixes[] = {"", "se", "te", "ve", "xe"};
    strcat(buffer, prefixes[baiconv->conv - 1]);
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

  char *loj

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_goi (TreeNode *x, char *eng)
{
  char *trans;

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

  char *loj

  char *eng
  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_koha (TreeNode *x, char *eng)
{
  char *trans;
  char *cmavo;

  cmavo = cmavo_table[x->data.cmavo.code].cmavo;

  trans = translate(cmavo);
  if (trans) {
    strcpy(eng, trans);
  } else {
    eng[0] = 0;
  }
  if (!strcmp(cmavo, "ke'a")) {
    XRelClauseLink *xrcl = prop_rel_clause_link(x, NO);
    if (xrcl) {
      XAntecedent *xan = prop_antecedent(xrcl->rel, NO);
      if (xan) {
        int bl = xan->node->data.nonterm.number;
        char tbuf[16];
        strcat(eng, " #");
        sprintf(tbuf, "%d", bl);
        strcat(eng, tbuf);
      }
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x

  char *sofar
  ++++++++++++++++++++++++++++++++++++++*/

static void
get_cmavo_text_inside_node_internal(TreeNode *x, char *sofar)
{
  struct nonterm *nt;
  int i, n;
  int selmao;
  char *t;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;
    n = nt->nchildren;
    for (i=0; i<n; i++) {
      c = nt->children[i];
      get_cmavo_text_inside_node_internal(c, sofar);
    }
  } else if (x->type == N_CMAVO) {
    selmao = x->data.cmavo.selmao;
    switch (selmao) {
      case UI:
      case BAhE:
      case DAhO:
      case Y:
      case FUhO:
        break;

      default:
        t = cmavo_table[x->data.cmavo.code].cmavo;
        strcat(sofar, t);
        break;
    }

  } else if (x->type == N_BU) {
    strcat(sofar, x->data.bu.word);
  
  }

  return;

}


/*++++++++++++++++++++++++++++++++++++++
  Retrieve the lojban text for the cmavo inside a node, excluding
  indicator stuff.

  static char * get_cmavo_text_inside_node

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static char *
get_cmavo_text_inside_node(TreeNode *x)
{
  static char buffer[4096];
  buffer[0] = 0;
  get_cmavo_text_inside_node_internal(x, buffer);
  return buffer;
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
      case CC_PEI:
        strcpy(buffer, translate("pei"));
        break;
      case CC_PEINAI:
        strcpy(buffer, translate("peinai"));
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
    strcpy(eng, "{");
    strcat(eng, buffer);
    strcat(eng, "..}");
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
      case CC_PEI:
        strcat(loj, " pei");
        break;
      case CC_PEINAI:
        strcat(loj, " peinai");
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

  /* Letting X be the cmavo to be looked up are, the cases addressed
     are :

     TERM    : X le Y
     NOUN    : le X Y
     SELBRI  : le Z cu X Y
     LINK    : i X bo Y
     CONNECT : Y gi'e X bo Z
     JAI     : le jai X Y

     */

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
      char engbuf[256];
      engbuf[0] = 0;
      trans = translate(cmavo_table[pu->data.cmavo.code].cmavo);
      strcat(engbuf, trans ? trans : "?");
      if (nai) {
        trans = translate(cmavo_table[nai->data.cmavo.code].cmavo);
        strcat(engbuf, " ");
        strcat(engbuf, trans ? trans : "?");
      }
      if (zi) {
        trans = translate(cmavo_table[zi->data.cmavo.code].cmavo);
        strcat(engbuf, " ");
        strcat(engbuf, trans ? trans : "?");
      }
      strcpy(eng, engbuf);
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
  If the lojban text comes from a source that supports conversion (and may
  require different glossing depending on context), apply the appropriate
  adjustments to translate it.

  char *loj The lojban text to translate (not derived from 'basis' since this
  fn is used for several different node types).

  TreeNode *basis The treenode on which the gloss-type and conversion tags may
  be hanging.
  ++++++++++++++++++++++++++++++++++++++*/

static char *
translate_convertible(char *loj, TreeNode *basis)
{
  char buffer[1024];
  char *trans;
  XConversion *conversion;
  XGlosstype *gt;
  int conv;

  conversion = prop_conversion(basis, NO);
  if (conversion) {
    conv = conversion->conv;
  } else {
    conv = 1;
  }
  gt = prop_glosstype(basis, NO);
  if (gt) {
    if (gt->in_selbri) {
      if (gt->is_tertau) {
        trans = adv_translate(loj, conv, TCX_VERB);
      } else {
        trans = adv_translate(loj, conv, TCX_QUAL);
      }
    } else {
      /* In a sumti */
      if (gt->is_tertau) {
        trans = adv_translate(loj, conv, TCX_NOUN);
      } else {
        trans = adv_translate(loj, conv, TCX_QUAL);
      }
    }
    if (trans) {
      return trans;
    } else {
      return NULL;
    }
  } else {
    sprintf(buffer, "%s%1d", loj, conv);
    trans = translate(buffer);
    if (trans) {
      return trans;
    } else {
      trans = translate(loj);
      if (trans) {
        if (conversion) {
          /* If we had to resort to a bare translation and there was
             a SE prefix, something is wrong. */
          strcat(trans, " (CONV?)");
        }
        return trans;
      } else {
        trans = translate_unknown(loj, conv);
        if (trans) {
          return trans;
        } else {
          return NULL;
        }
      }
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++

  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_brivla (TreeNode *x, char *eng)
{
  char *trans;

  trans = translate_convertible(x->data.brivla.word, x);
  if (trans) {
    strcpy(eng, trans);
  } else {
    eng[0] = 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++

  ++++++++++++++++++++++++++++++++++++++*/

static void
translate_abstraction (TreeNode *x, char *eng)
{
  char *trans;
  int code;
  char *cmavo;

  code = x->data.cmavo.code;
  cmavo = cmavo_table[code].cmavo;
  trans = translate_convertible(cmavo, x);
  if (trans) {
    strcpy(eng, trans);
  } else {
    eng[0] = 0;
  }

}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
attempt_translation(char *loj, char *eng)
{
  char *trans;
  trans = translate(loj);
  if (trans) {
    strcpy(eng, trans);
  } else {
    strcpy(eng, "?");
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
get_lojban_word_and_translation (TreeNode *x, char *loj, char *eng)
{

  switch (x->type) {
    
    case N_CMAVO:
      if (prop_elidable(x, NO)) {
        strcpy(loj, make_uppercase(cmavo_table[x->data.cmavo.code].cmavo));
      } else {
        strcpy(loj, cmavo_table[x->data.cmavo.code].cmavo);
      }
      eng[0] = 0;
      switch (x->data.cmavo.selmao) {
        case BAI:
          translate_bai(x, eng);
          break;

        case SE:
          translate_se(x, eng);
          break;

        case GOI:
          translate_goi(x, eng);
          break;

        case KOhA:
          translate_koha(x, eng);
          break;

        case NU:
          translate_abstraction(x, eng);
          break;

        case PU:
        case ZAhO:
        case VA:
        case FAhA:
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
      sprintf(loj, "%s %s %s %s", x->data.zoi.form, x->data.zoi.term, x->data.zoi.text, x->data.zoi.term);
      attempt_translation(x->data.zoi.text, eng);
      break;
      
    case N_ZO:
      {
        char buffer[1024];
        sprintf(loj, "zo %s", x->data.zo.text);
        attempt_translation(x->data.zo.text, buffer);
        sprintf(eng, "\"%s\"", buffer);
      }
      break;

    case N_ZEI:
      {
        /* For the translation, try 2 modes : glue together all the non-zei
           words with + signs and look that up.  If the lookup is successful,
           good.  If it fails, translate it a word at a time with zei
           translated as 'type-of' i.e. treat it pretty much like a tanru */

        char *trans;
        int i, N = x->data.zei.nchildren;

        strcpy(loj, x->data.zei.sep_with_zei);

        trans = translate_convertible(x->data.zei.sep_with_plus, x);

        if (trans && trans[0] != '?') {
          strcpy(eng, trans);
        } else {
          /* A word at a time */
          eng[0] = 0;
        fprintf(stderr, "Got here 3\n");
          for (i=0; i<N; i++) {
            char *temp;
            if (i > 0) strcat(eng, " type-of ");
            temp = build_string_from_node(x->data.zei.children[i]);
            fprintf(stderr, "Comp [%s]\n", temp);
            trans = translate(temp);
            Free(temp);
            if (trans) strcat(eng, trans);
            else       strcat(eng, "?");
          }
        }
        
      }
      break;
      
    case N_LOhU:
      sprintf(loj, "lo'u %s le'u", x->data.lohu.text);
      attempt_translation(x->data.lohu.text, eng);
      break;

    case N_BU:
      sprintf(loj, "%s.bu", x->data.bu.word);
      attempt_translation(loj, eng);
      break;
        
    case N_BRIVLA:
      strcpy(loj, x->data.brivla.word);
      translate_brivla(x, eng);
      break;
      
    case N_CMENE:
      strcpy(loj, x->data.cmene.word);
      strcpy(eng, "[NAME]");
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

  if (what == SHOW_LOJBAN ||
      what == SHOW_LOJBAN_AND_INDICATORS ||
      what == SHOW_BOTH) {
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
            (drv->start_tag)();
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
          case TTT_JAI:
            trans = adv_translate(tag->brivla.x->data.brivla.word, tag->pos, TCX_TAG);
            if (!trans) trans = "?";
            sprintf(tp, "%d", tag->pos);
            (drv->start_tag)();
            (drv->write_tag_text)(tag->brivla.x->data.brivla.word, tp, trans, YES);
            break;
          case TTT_ABSTRACTION:
            {
              int code;
              char *cmavo;
              code = tag->abstraction.nu->data.cmavo.code;
              cmavo = cmavo_table[code].cmavo;
              trans = adv_translate(cmavo, tag->pos, TCX_TAG);
              if (!trans) trans = "?";
              sprintf(tp, "%d", tag->pos);
              (drv->start_tag)();
              (drv->write_tag_text)(cmavo, tp, trans, YES);
            }
          break;

          case TTT_ME:
            {
              char *trans, transbuf[1024];
              (drv->start_tag)();
              trans = adv_translate("me", tag->pos, TCX_TAG);
              sprintf(transbuf, trans, tag->me.sumti->data.nonterm.number);
              sprintf(tp, "%d..", tag->pos);
              (drv->write_tag_text)("me", tp, transbuf, YES);
            }
          break;
          case TTT_NUMBERMOI:
            {
              char *trans, lojbuf[128], transbuf[1024];
              int code;
              int norl_code;
              char *cmavo;
              code = tag->numbermoi.moi->data.cmavo.code;
              cmavo = cmavo_table[code].cmavo;
              norl_code = tag->numbermoi.number_or_lerfu->data.nonterm.number;
              (drv->start_tag)();
              trans = adv_translate(cmavo, tag->pos, TCX_TAG);
              if (trans) {
                sprintf(transbuf, trans, norl_code);
              } else {
                transbuf[0] = 0;
              }
              sprintf(lojbuf, "#%d-%s%d", norl_code, cmavo, tag->pos);
              (drv->write_tag_text)(lojbuf, "", transbuf, YES);
            }
            break;
            
          case TTT_GOhA:
            {
              int code;
              char *cmavo;
              char lojbuf[32];

              code = tag->goha.goha->data.cmavo.code;
              cmavo = cmavo_table[code].cmavo;
              (drv->start_tag)();
              sprintf(lojbuf, "%s%d", cmavo, tag->pos);
              (drv->write_tag_text)(lojbuf, "", "", YES);
            }
            break;
          
          case TTT_NUhA:
            {
              TreeNode *mex_operator = tag->nuha.mex_operator;
              int number = mex_operator->data.nonterm.number;
              char lojbuf[64];
              sprintf(lojbuf, "#%d-%d", number, tag->pos);
              (drv->start_tag)();
              (drv->write_tag_text)(lojbuf, "", "", YES);
            }
            break;

          case TTT_ZEI:
            {
              int number;
              char lojbuf[32];
              number = tag->zei.zei->data.zei.number;
              sprintf(lojbuf, "#%d-%d", number, tag->pos);
              trans = adv_translate(tag->zei.zei->data.zei.sep_with_plus, tag->pos, TCX_TAG);
              if (!trans) trans = "?";
              (drv->start_tag)();
              (drv->write_tag_text)(lojbuf, "", trans, YES);
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
  int i, n;

  loj[0] = 0;
      
  translate_time_offset(x, loj, eng);

  switch (what) {
    case SHOW_LOJBAN:
    case SHOW_LOJBAN_AND_INDICATORS:
    case SHOW_BOTH:
      n = x->data.nonterm.nchildren;
      for (i=0; i<n; i++) {
        output_internal(x->data.nonterm.children[i], SHOW_LOJBAN_AND_INDICATORS);
      }
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
      case SHOW_LOJBAN_AND_INDICATORS:
      case SHOW_BOTH:
        output_internal(c1, SHOW_LOJBAN);
        (drv->translation)(" "); /* Force block out in blocked output
                                    drivers */
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
      (what == SHOW_LOJBAN_AND_INDICATORS) ||
      (what == SHOW_BOTH)) {
    for (i=0; i<n; i++) {
      c = y->children[i];
      output_internal(c, SHOW_LOJBAN_AND_INDICATORS);
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
        /* This is what I think negations on this construct mean ... I
           think it translates as scalar negations of the two phrases.
           Perhaps there should be scalar negation of the connective?
           Except that could be within the stag itself, so I think my
           interpretation is OK.  */
        if (xcon->neg1) {
          if (what == SHOW_TAG_TRANS) {
            (drv->write_tag_text)("", "", "something other than", NO);
          } else {
            (drv->translation)("something other than");
          }
        }
        

      case CNP_GE_JOIK:
        break; /* Don't put anything here */

      case CNP_GI_STAG:
        /* Output the stag as though it occurred at the position of
           the gik in the middle of the sentence.  Doesn't do anything
           yet about negations occurring on the connective - the
           reference grammar isn't even clear what these mean. */
        if (what == SHOW_TAG_TRANS) {
          output_internal(xcon->js, SHOW_TAG_TRANS);
        } else {
          output_internal(xcon->js, SHOW_ENGLISH);
        }

        if (xcon->neg2) {
          if (what == SHOW_TAG_TRANS) {
            (drv->write_tag_text)("", "", "other than", NO);
          } else {
            (drv->translation)("other than");
          }
        }

        break;


      case CNP_GI_JOIK:
        /* Output the joik as though it occurred at the position of
           the gik in the middle of the sentence. */

        if (xcon->neg2) {
          if (what == SHOW_TAG_TRANS) {
            (drv->write_tag_text)("", "", "other than", NO);
          } else {
            (drv->translation)("other than");
          }
        }
        if (what == SHOW_TAG_TRANS) {
          output_internal(xcon->js, SHOW_TAG_TRANS);
        } else {
          output_internal(xcon->js, SHOW_ENGLISH);
        }
    }
  }

}


/*++++++++++++++++++++++++++++++++++++++
  For certain types of non-terminal, gather together the constituent
  children as a concatenated string.  Try to look this up in the
  dictionary.  If it works output that translation, else drill down as
  normal.

  TreeNode *x

  WhatToShow what
  ++++++++++++++++++++++++++++++++++++++*/

static void
output_clustered(TreeNode *x, WhatToShow what)
{
  char *cluster, *trans;
  char localtrans[256];
  char lojbuf[1024], lojbuf2[1024];
  int i, n;
  TreeNode *c;
  struct nonterm *nt;
  XTenseCtx *ctx;

  ctx = prop_tense_ctx(x, NO);

  cluster = get_cmavo_text_inside_node(x);
  strcpy(lojbuf, cluster);
  strcpy(lojbuf2, cluster);

  /* May eventually want context dependence here since many of the
     cases are tag/tense sorts of thing */

  /* Letting X be the cmavo to be looked up are, the cases addressed
     are :

     TERM    : X le Y
     NOUN    : le X Y
     SELBRI  : le Z cu X Y
     LINK    : i X bo Y
     CONNECT : Y gi'e X bo Z
     JAI     : le jai X Y

     */

  if (ctx) {
    switch (ctx->ctx) {
      case TSC_OTHER:
        break;
      case TSC_TERM:
        strcat(lojbuf, "@TERM");
        break;
      case TSC_NOUN:
        strcat(lojbuf, "@NOUN");
        break;
      case TSC_SELBRI:
        strcat(lojbuf, "@SELBRI");
        break;
      case TSC_LINK:
        strcat(lojbuf, "@LINK");
        break;
      case TSC_CONNECT:
        strcat(lojbuf, "@CONNECT");
        break;
      case TSC_JAITAG:
        strcat(lojbuf, "@JAI");
        break;
    }
  }

  trans = translate(lojbuf);
  if (!trans) {
    /* If no form with the @SELBRI etc tacked on, try to translate
       just the plain cluster without the contextual information added */
    trans = translate(lojbuf2);
  }

  if (trans) {
    strcpy(localtrans, trans);
    if ((what == SHOW_LOJBAN) ||
        (what == SHOW_LOJBAN_AND_INDICATORS) ||
        (what == SHOW_BOTH)) {
      nt = &x->data.nonterm;
      n = nt->nchildren;
      for (i=0; i<n; i++) {
        c = nt->children[i];
        output_internal(c, SHOW_LOJBAN_AND_INDICATORS);
      }
    }
    
    if ((what == SHOW_ENGLISH) ||
        (what == SHOW_BOTH)) {
      (drv->translation)(localtrans);
    }

  } else {
    nt = &x->data.nonterm;
    n = nt->nchildren;
    for (i=0; i<n; i++) {
      c = nt->children[i];
      output_internal(c, what);
    }
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
output_internal(TreeNode *x, WhatToShow what)
{
  char loj[1024], eng[1024];
  int i, n;

  if (x->type == N_NONTERM) {
    struct nonterm *y;
    y = &x->data.nonterm;

    if (what == SHOW_BOTH || what == SHOW_LOJBAN || what == SHOW_LOJBAN_AND_INDICATORS) {
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

      if (y->nchildren == 2) { /* not in 'insert elidables' mode */
        output_internal(y->children[0], what);
        switch (what) {
          case SHOW_ENGLISH:
          case SHOW_BOTH:
            (drv->translation)("[is, does]");
            break;
          default:
            break;
        }
        output_internal(y->children[1], what);
      } else {
        int n = y->nchildren, i;
        for (i=0; i<n; i++) {
          output_internal(y->children[i], what);
        }
      }

    } else if (y->type == OBSERVATIVE_SENTENCE) {
      /* Special handling */

#if 0
      /* This doesn't work too well in various cases, e.g. abstractors
         without an x1 before the selbri, and particular in poi/noi
         clauses.  Leave out until this can be looked into. */
      (drv->translation)("(there is)");
#endif
      output_internal(y->children[0], what);

    } else if (y->type == TIME_OFFSET) {

      output_simple_time_offset(x, what);

    } else if ((y->type == SPACE_INT_PROP) ||
               (y->type == INTERVAL_PROPERTY) ||
               (y->type == NUMBER_MOI_TU2)) {
      
      output_clustered(x, what);

    } else if (y->type == SUMTI_TAIL) {

      output_sumti_tail(x, what);

    } else if (((y->type == SUMTI_5A) && (y->nchildren ==2)) ||
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

    if (what == SHOW_BOTH || what == SHOW_LOJBAN || what == SHOW_LOJBAN_AND_INDICATORS) {
      (drv->close_bracket)(y->brackets, y->number);
    }

  } else if (x->type == N_ZEI) {

    if (what == SHOW_BOTH || what == SHOW_LOJBAN || what == SHOW_LOJBAN_AND_INDICATORS) {
      (drv->open_bracket)(x->data.zei.brackets, x->data.zei.number);
    }

    /* FIXME : Need to do lojban word and translation stuff here. */
    get_lojban_word_and_translation(x, loj, eng);
    switch (what) {
      case SHOW_LOJBAN:
      case SHOW_LOJBAN_AND_INDICATORS:
      case SHOW_BOTH:
        (drv->lojban_text)(loj);
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

    if (what == SHOW_BOTH || what == SHOW_LOJBAN || what == SHOW_LOJBAN_AND_INDICATORS) {
      (drv->close_bracket)(x->data.zei.brackets, x->data.zei.number);
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
      case SHOW_LOJBAN_AND_INDICATORS:
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

      case SHOW_LOJBAN_AND_INDICATORS:
        if ((x->type == N_CMAVO) && 
            ((x->data.cmavo.selmao == UI) ||
             (x->data.cmavo.selmao == BAhE) ||
             (x->data.cmavo.selmao == Y) || 
             (x->data.cmavo.selmao == DAhO) ||
             (x->data.cmavo.selmao == FUhO))) {
          if (eng[0]) {
            (drv->translation)(eng);
          }
          (drv->set_eols)(x->eols);
        }          
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
