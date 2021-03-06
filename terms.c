/***************************************
  $Header$

  Processing to work out which x-place of a particular selbri any term
  in the text occupies.
  ***************************************/

/* COPYRIGHT */

/*{{{ #Includes  */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "rpc2x_act.tab.h"
#include "nodes.h"
#include "cmavotab.h"
/*}}}*/

#define MAX_TERMS_IN_VECTOR 20

/* Maximum argument position.  Even the most horrific abstraction or
   lujvo would be pushed to get this many args!!! */

#define MAX_POS 20

/*+ Value used to denote 'fai' as a place.  So fa,fe,fi,fo,fu are
  1..5, f[aeiou] xi xa is 6, ... f[aeiou] xi soso is 99, fai or fai xi
  pa is 101 etc +*/
#define FAI_OFFSET 101

/* Type definitions *//*{{{*/

typedef enum {
  TRM_POS, /* positional, i.e. just bare sumti */
  TRM_FA,  /* a term with a FA in front */
  TRM_FAhI,/* a term with a fa'i in front */
  TRM_TAG  /* a term with a modal tag in front */
} TermType;

typedef struct TermVectorEntry{
  TermType type;
  int      pos; /* For the FA case, which position the sumti has */
  TreeNode *node; /* The treenode corresponding to this term */
} TermVectorEntry;

typedef struct TermVector{
  int n_nodes;
  TermVectorEntry nodes[MAX_TERMS_IN_VECTOR];
} TermVector;

/*+ Enumeration tags for the LinkConvEntry type. +*/
typedef enum {
  LC_LINKS, /* TermVector of sumti links */
  LC_SE, /* SE conversion */
  LC_JAI, /* JAI conversion */
  LC_TAG /* JAI tag conversion */
} LCType;

typedef struct {
  int conv;
  TreeNode *senode;
} SeInfo;

typedef struct {
  LCType type;
  union {
    TermVector *links;
    SeInfo se;
    struct {
      TreeNode *tag; /* The tag */
      TreeNode *inner_tu2; /* The modified tanru_unit_2 */
    } jai_tag;
  } data;
} LinkConvEntry;

/*+ Structure containing an array of linked sumti blocks and
  conversions.  This is accumulated whilst drilling down into 'KE
  selbri3 KE'E BE links' constructions, which could recurse a lot. +*/
typedef struct {
  int n;
  LinkConvEntry e[MAX_TERMS_IN_VECTOR];
} LinkConv;
/*}}}*/
/* Forward prototypes *//*{{{*/
static void process_bridi_tail(TreeNode *bt, TermVector *pre, TermVector *post);
static void process_selbri_args(TreeNode *s, TermVector *pre, TermVector *post, LinkConv *lc);
static void process_selbri_3_args(TreeNode *s3, TermVector *pre, TermVector *post, LinkConv *lc);
/*}}}*/

static void tv_init(TermVector *tv)/*{{{*/
{
  tv->n_nodes = 0;
}
/*}}}*/
static void tv_catenate(TermVector *s1, TermVector *s2, TermVector *r)/*{{{*/
{
  int tn;
  int i, n1, n2;
  tn = s1->n_nodes + s2->n_nodes;

  /* If this is exceeded, we'll need to reassess the design.  Really
     ought to make the arrays dynamic but that incurs hassle of
     releasing them. */
  assert(tn <= MAX_TERMS_IN_VECTOR);

  n1 = s1->n_nodes;
  n2 = s2->n_nodes;

  r->n_nodes = tn;

  for (i=0; i<n1; i++) {
    r->nodes[i] = s1->nodes[i];
  }

  for (i=0; i<n2; i++) {
    r->nodes[n1+i] = s2->nodes[i];
  }
}
/*}}}*/
static void tv_reverse(TermVector *dest, TermVector *src)/*{{{*/
{
  /* Reverse the order of a Termector. */
  int n, i;

  assert(dest != src); /* Not designed to cope with this case */

  n = dest->n_nodes = src->n_nodes;

  for (i=0; i<n; i++) {
    dest->nodes[i] = src->nodes[n-1-i];
  }
}/*}}}*/
static void lc_init(LinkConv *lc)/*{{{*/
{
  /* Initialise a linkconv list to empty */
  lc->n = 0;
}
/*}}}*/
static void lc_append_se(LinkConv *lc, int conv, TreeNode *senode)/*{{{*/
{
  /* Append a SE conversion to a linkconv chain */
  assert(lc->n < MAX_TERMS_IN_VECTOR);

  lc->e[lc->n].data.se.conv = conv;
  lc->e[lc->n].data.se.senode = senode;
  lc->e[lc->n].type = LC_SE;
  ++(lc->n);
}
/*}}}*/
static void lc_append_jai_tag(LinkConv *lc, TreeNode *tag, TreeNode *inner_tu2)/*{{{*/
{
  assert(lc->n < MAX_TERMS_IN_VECTOR);

  lc->e[lc->n].data.jai_tag.tag = tag;
  lc->e[lc->n].data.jai_tag.inner_tu2 = inner_tu2;
  lc->e[lc->n].type = LC_TAG;
  ++(lc->n);
  
}
/*}}}*/
static void lc_append_jai(LinkConv *lc)/*{{{*/
{
  assert(lc->n < MAX_TERMS_IN_VECTOR);

  lc->e[lc->n].type = LC_JAI;
  ++(lc->n);
  
}
/*}}}*/
static void lc_append_links(LinkConv *lc, TermVector *v)/*{{{*/
{
  /* Append a set of linked sumti to a linkconv chain.  Note, a dynamic COPY is
     made of 'v'. */
  assert(lc->n < MAX_TERMS_IN_VECTOR);
  lc->e[lc->n].data.links = new(TermVector);

  /* Deep structure copy */
  *(lc->e[lc->n].data.links) = *v;
  
  lc->e[lc->n].type = LC_LINKS;
  ++(lc->n);
}
/*}}}*/
static void lc_copy(const LinkConv *src, LinkConv *dest)/*{{{*/
{
/* Copy a link/conv vector.  A semi-deep structure copy is made, i.e. the
  pointers to the vectors of linked sumti get aliased.  The intention is to
  produce a local copy onto which extra terms can be appended, rather than to
  make a completely general copy.  */
  
  *dest = *src;
}
/*}}}*/
static int recover_se_conv(TreeNode *x)/*{{{*/
{
  /* Turn se, te etc into 2..5 */
  int se_code;
  char *se_str;
  TreeNode *se;

  se = strip_attitudinal(x);
  assert (se->data.cmavo.selmao == SE);

  se_code = se->data.cmavo.code;
  se_str = cmavo_table[se_code].cmavo;

  /* Unfortunately, faxixa, faxize etc to access the 6th places
     onwards are just too much to handle for now!! */
  
  if (!strcmp(se_str, "se")) {
    return 2;
  } else if (!strcmp(se_str, "te")) {
    return 3;
  } else if (!strcmp(se_str, "ve")) {
    return 4;
  } else if (!strcmp(se_str, "xe")) {
    return 5;
  } else {
    abort();
  }
}
/*}}}*/
static int recover_fa_conv(TreeNode *x)/*{{{*/
{
/* Turn fa, fe... into 1 .. 5

  EXTEND TO COPE WITH SUBSCRIPTED VALUES!!!

  static int recover_fa_conv Return the value in the range 1 .. 5
  corresponding to the FA cmavo.

  TreeNode *x The parse node, must be a cmavo of selma'o FA.  */
  int fa_code;
  char *fa_str;

  x = strip_attitudinal(x);
  assert (x->data.cmavo.selmao == FA);

  fa_code = x->data.cmavo.code;
  fa_str = cmavo_table[fa_code].cmavo;

  /* Unfortunately, faxixa, faxize etc to access the 6th places
     onwards are just too much to handle for now!! */
  
  if (!strcmp(fa_str, "fi'a")) {
    return 0;
  } else if (!strcmp(fa_str, "fa")) {
    return 1;
  } else if (!strcmp(fa_str, "fe")) {
    return 2;
  } else if (!strcmp(fa_str, "fi")) {
    return 3;
  } else if (!strcmp(fa_str, "fo")) {
    return 4;
  } else if (!strcmp(fa_str, "fu")) {
    return 5;
  } else if (!strcmp(fa_str, "fai")) {
    return FAI_OFFSET;
  } else {
    abort();
  }
}
/*}}}*/
static void tv_build(TermVector *r, TreeNode *x)/*{{{*/
{
/* Build a TermVector from a terms node in the parse tree.  Any terms_1
  or terms_2 with CEhE or PEhE inside is ignored - to defer the problem of what
  to do about afterthought termsets to somewhere else.  */

  TermVector vv;
  TreeNode *xx, *t1, *t2, *t, *tc, *tcc;
  struct nonterm *ntx, *ntt;
  int nc, ntc;

  type_check(x, TERMS);

  /* The main hassle is that the <terms> rule is left recursive, so we
     build the vector backwards and reverse it at the end. */
  tv_init(&vv);

  xx = x;
  do {
    ntx = &xx->data.nonterm;
    nc = ntx->nchildren;

    if (nc == 1) { /* last term in the series */
      t1 = ntx->children[0];
    } else if (nc == 2) {
      t1 = ntx->children[1];
      xx = ntx->children[0]; /* next term in the series */
    } else {
      abort();
    }

    type_check(t1, TERMS_1);

    /* No point assertion checking whether the children are
       nonterminals, they must be from the grammar */
    ntt = &t1->data.nonterm;
    ntc = ntt->nchildren;

    if (ntc > 1) {
      /* aaaargh!! termset land */
      fprintf(stderr, "No place tagging for termset at line %d\n", t1->start_line);
      continue;
    }

    t2 = ntt->children[0];
    type_check(t2, TERMS_2);
    ntt = &t2->data.nonterm;
    ntc = ntt->nchildren;

    if (ntc > 1) {
      /* aaaargh!! termset land */
      fprintf(stderr, "No place tagging for termset at line %d\n", t2->start_line);
      continue;
    }

    t = ntt->children[0];
    assert(t->data.nonterm.type == TERM);
    
    /* Now, what sort of term is it? Have to drill down one layer further */
    tc = t->data.nonterm.children[0];

    switch (tc->data.nonterm.type) {
      case TERM_PLAIN_SUMTI:
        vv.nodes[vv.n_nodes].type = TRM_POS;
        vv.nodes[vv.n_nodes].node = t;
        vv.n_nodes++;
        break;
      case TERM_PLACED_SUMTI:
        {
          int pos;
          tcc = child_ref(tc, 0);
          pos = recover_fa_conv(tcc);
          if (pos == 0) {
            vv.nodes[vv.n_nodes].type = TRM_FAhI;
            vv.nodes[vv.n_nodes].node = t;
            vv.nodes[vv.n_nodes].pos = 0;
          } else {            
            vv.nodes[vv.n_nodes].type = TRM_FA;
            vv.nodes[vv.n_nodes].node = t;
            vv.nodes[vv.n_nodes].pos = pos;
          }
          vv.n_nodes++;
        }
        break;
      case TERM_TAGGED_SUMTI:
      case TAGGED_TERMSET:
        vv.nodes[vv.n_nodes].type = TRM_TAG;
        vv.nodes[vv.n_nodes].node = t;
        vv.n_nodes++;
        break;
      case TERMSET:
        fprintf(stderr, "No place tagging for termset at line %d\n", t->start_line);
        break;
      case TERM_FLOATING_TENSE:
      case TERM_FLOATING_NEGATE:
      case TERM_OTHER:
      default:
        /* None of these are interesting for place tagging */
        break;
    }

  } while (nc == 2); /* Keep looping whilst were in the left recursive <terms> rule */

  /* Reverse the order to put the terms in natural order */
  tv_reverse(r, &vv);

}
/*}}}*/
static void process_subsentence(TreeNode *ss, TermVector *pre, TermVector *post)/*{{{*/
{
  struct nonterm *nt, *ntc;
  int nc;
  TreeNode *sss;
  TreeNode *c;
  TermVector head_terms, new_pre;
  TreeNode *terms, *btail;

  sss = ss;
  do {
    type_check(ss, SUBSENTENCE);

    nt = &sss->data.nonterm;
    nc = nt->nchildren;

    if (nc == 2) {
      /* Pick the 'subsentence' following the 'prenex' out of the 2 child version */
      sss = child_ref(sss, 1);
    } else {
      /* Pick 'sentence' out of the 1 child version */
      sss = child_ref(sss, 0);
    }
  } while (nc == 2);

  type_check(sss, SENTENCE);
  nt = &sss->data.nonterm;

  if (nt->nchildren == 1) {
    c = nt->children[0];
    if (c->type == N_NONTERM) {
      ntc = & c->data.nonterm;
      if ((ntc->type == NO_CU_SENTENCE) || (ntc->type == OBSERVATIVE_SENTENCE)) {
        sss = c;
        nt = ntc;
      }
    }
  }

  /* Now looking at the '[terms [CU #]] bridi_tail' node. */
  terms = find_nth_child(sss, 1, TERMS);
  btail = find_nth_child(sss, 1, BRIDI_TAIL);
  assert(btail); /* Every sentence has one of these! */

  if (terms) {
    tv_build(&head_terms, terms);
    tv_catenate(pre, &head_terms, &new_pre);
    process_bridi_tail(btail, &new_pre, post);
  } else {
    process_bridi_tail(btail, pre, post);
  }
}
/*}}}*/

/* Type definitions (tags, places etc) *//*{{{*/

typedef enum {
  PT_ORD, /* one of the x1 .. x5 of the BRIVLA etc */
  PT_TAG, /* e.g. the x1 of 'jai bau cusku' */
  PT_JAI  /* e.g. the x1 of 'jai rinka' */
} PlaceType;

typedef struct {
  TreeNode *tag;
  TreeNode *inner_tu2;
} TagPlace;

typedef struct {
  int pad;
} JaiPlace;

typedef struct {
  PlaceType type;
  int valid;
  int taken;
  int pos;
  TagPlace tag;
  JaiPlace jai;
} Place;
/*}}}*/

static void fixup_term_place(TreeNode *x, Place *pl, XTermTag *tt)/*{{{*/
{
  /* Given a term treenode and a single place descriptor, chain the place
     information to the node's property list for later display. */
  XTermTags *ts, *nts;

  type_check(x, TERM);
  ts = prop_term_tags(x, NO);
  if (ts) {
    while(ts->next) ts = ts->next; /* Get to end of list */

    nts = new(XTermTags);
    ts->next = nts;
    nts->next = NULL;
    ts = nts;
  } else {
    ts = prop_term_tags(x, YES); /* Create it */
    ts->next = NULL;
  }

  ts->tag = *tt; /* deep copy, tt is in automatic storage */

  ts->tag.pos = pl->pos;

  switch (pl->type) {
    case PT_ORD:
      break;

    case PT_TAG:
      ts->tag.jaitag.tag = pl->tag.tag;
      ts->tag.jaitag.inner_tu2 = pl->tag.inner_tu2;
      ts->tag.type = TTT_JAITAG;
      break;

    case PT_JAI:
      ts->tag.type = TTT_JAI;
      break;
  }
  
}/*}}}*/
static void assign_terms_to_places(TermVector *t, Place *place, Place *fai, int abase, XTermTag *tt)/*{{{*/
{
  int i, n;
  int base;
  int pos;

  base = abase;

  /* Assign FA places */
  n = t->n_nodes;
  for (i=0; i<n; i++) {
    if (t->nodes[i].type == TRM_FA) {
      pos = t->nodes[i].pos;
      if (pos < FAI_OFFSET) {
        if (place[pos].valid) {
          fixup_term_place(t->nodes[i].node, &place[pos], tt);
          place[pos].taken = 1;
        } else {
          fprintf(stderr, "Invalid place\n");
        }
      } else {
        pos -= FAI_OFFSET;
        pos++;
        if (fai[pos].valid) {
          fixup_term_place(t->nodes[i].node, &fai[pos], tt);
          fai[pos].taken = 1;
        } else {
          fprintf(stderr, "Invalid place\n");
        }          
      }
    }
  }

  /* Now fix up unmarked places */
  for (i=0; i<n; i++) {
    switch (t->nodes[i].type) {
      case TRM_POS :
        /* Look for first unassigned place */
        while (place[base].taken) ++base;
        if (base > MAX_POS) {
          fprintf(stderr, "Too many places assigned\n");
        } else {
          fixup_term_place(t->nodes[i].node, &place[base], tt);
          place[base].taken = 1;
          ++base;
        }
        break;

      case TRM_FA :
        pos = t->nodes[i].pos;
        if (pos < FAI_OFFSET) {
          base = pos + 1;
        }

        break;

      default:
        break;

    }
  }

}
/*}}}*/
static void assign_conversion(LinkConv *lc, TreeNode *convertible)/*{{{*/
{
  Place place[MAX_POS];
  XConversion *ext;

  int i, n;

  /* Init the place and vectors */
  for (i=0; i<MAX_POS; i++) {
    place[i].valid = 1;
    place[i].taken = 0;
    place[i].type = PT_ORD;
    place[i].pos = i;
  }

  /* Work back through the linked sumti */
  n = lc->n;
  for (i=n-1; i>=0; i--) {
    switch (lc->e[i].type) {
      case LC_SE:
        /* Just swap 2 positions in the place[] array */
        {
          Place temp;
          SeInfo *j;
          XDontGloss *dg;

          j = &lc->e[i].data.se;
          temp = place[1];
          place[1] = place[j->conv];
          place[j->conv] = temp;
          dg = prop_dont_gloss(j->senode, YES); /* prevent the node being glossed */
        }        
        break;

      case LC_LINKS:
        /* Binding of linked sumti does not affect the glossing of the
           brivla or whatever, just drop through to next loop
           iteration */
        break;

      case LC_TAG:
        /* Break out at this point */
        goto loop_done;
        break;

      case LC_JAI:
        fprintf(stderr, "Don't know what to do with <jai> for gloss conversion\n");
        break;
    }
  }

loop_done:
  
  ext = prop_conversion(convertible, YES);
  ext->conv = place[1].pos;

}
/*}}}*/
static void assign_places(TermVector *pre, TermVector *post, LinkConv *lc, XTermTag *tt)/*{{{*/
{
  /* Work out which terms have which places in the bridi, and tag them
     accordingly */

  /* Variable declarations */

  /*+ Array for the ordinary places in the bridi.  e.g. if you get a
    term with 'fe' in front of it, look in place[2] to find what to do
    with it. +*/
  Place place[MAX_POS];

  /*+ Array for the 'fai' places.  e.g. if a term has 'fai xi re' in
    front, look in fai[2] to find what to do. +*/

  Place fai[MAX_POS];
  int i, n;

  /* Init the place and fai vectors */
  for (i=0; i<MAX_POS; i++) {
    place[i].valid = 1;
    place[i].taken = 0;
    place[i].type = PT_ORD;
    place[i].pos = i;
    
    fai[i].valid = 0;
    fai[i].taken = 0;
  }

  /* Work back through the linked sumti */
  n = lc->n;
  for (i=n-1; i>=0; i--) {
    switch (lc->e[i].type) {
      case LC_SE:
        /* Just swap 2 positions in the place[] array */
        {
          Place temp;
          SeInfo *j;
          j = &lc->e[i].data.se;
          temp = place[1];
          place[1] = place[j->conv];
          place[j->conv] = temp;
        }        
        break;
      case LC_TAG:
        {
          int j;
          /* Shift fai place array up (i.e. need subscripts if more
             than one fai place is in scope). */
          for (j=1; ; j++) {
            if (!fai[j].valid) break;
          }
          for (; j>1; j--) {
            fai[j] = fai[j-1];
          }
          fai[1] = place[1];
          place[1].type = PT_TAG;
          place[1].valid = 1;
          place[1].tag.tag = lc->e[i].data.jai_tag.tag;
          place[1].tag.inner_tu2 = lc->e[i].data.jai_tag.inner_tu2;
        }
      break;

      case LC_JAI:
        {
          int j;
          /* Shift fai place array up (i.e. need subscripts if more
             than one fai place is in scope). */
          for (j=1; ; j++) {
            if (!fai[j].valid) break;
          }
          for (; j>1; j--) {
            fai[j] = fai[j-1];
          }
          fai[1] = place[1];
          place[1].type = PT_ORD;
          place[1].valid = 1;
        }
        break;

      case LC_LINKS:
        assign_terms_to_places(lc->e[i].data.links, place, fai, 2, tt);
        break;
    }
  }

  /* Do the pre and post terms */
  assign_terms_to_places(pre, place, fai, 1, tt);
  assign_terms_to_places(post, place, fai, 2, tt);

}
/*}}}*/
static void process_tanru_unit_2_args(TreeNode *tu2, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
  /* Handle argument processing at the level of a tanru_unit_2.  This is
     where the clever tag assignment stuff is done! */

  TreeNode *c1;

  type_check(tu2, TANRU_UNIT_2);
  c1 = maybe_strip_attitudinal(child_ref(tu2, 0));

  if (c1->type == N_CMAVO) {/*{{{*/
    switch (c1->data.cmavo.selmao) {
      case GOhA:/*{{{*/
        {
          XTermTag tt;

          tt.type = TTT_GOhA;
          tt.goha.goha = c1;
          assign_places(pre, post, lc, &tt);
          assign_conversion(lc, c1);
        }
        break;/*}}}*/
      case ME:/*{{{*/
        {
          XTermTag tt;
          XRequireBrac *xrb;
          TreeNode *me_node;

          tt.type = TTT_ME;
          tt.me.sumti = find_nth_child(tu2, 1, SUMTI);
          me_node = find_nth_cmavo_child(tu2, 1, ME);
          assert(tt.me.sumti);
          xrb = prop_require_brac (tt.me.sumti, YES);
          assign_places(pre, post, lc, &tt);
          /* Conversion can't occur on ME, there is only an x1 place */
        }
      break;/*}}}*/
      case NUhA:/*{{{*/
        {
          XTermTag tt;
          XRequireBrac *xrb;

          tt.type = TTT_NUhA;
          tt.nuha.mex_operator = find_nth_child(tu2, 1, MEX_OPERATOR);
          assign_places(pre, post, lc, &tt);
          assign_conversion(lc, c1);
          xrb = prop_require_brac (tt.nuha.mex_operator, YES);
        }
        break;/*}}}*/
        
    }
/*}}}*/
  } else if (c1->type == N_BRIVLA) {/*{{{*/

    XTermTag tt;

    tt.type = TTT_BRIVLA;
    tt.brivla.x = c1;
    assign_places(pre, post, lc, &tt);
    assign_conversion(lc, c1);/*}}}*/
  } else if (c1->type == N_NONTERM) {
    switch (c1->data.nonterm.type) {
      case NUMBER_MOI_TU2:/*{{{*/
        {
          TreeNode *norl, *moi;
          XRequireBrac *xrb;
          XTermTag tt;
          /* Remember : The NUMBER_MOI marker that's in the bison file is NOT
             built into the parse tree!! */
          norl = child_ref(c1, 0);
          moi = find_nth_cmavo_child(c1, 1, MOI);
          tt.type = TTT_NUMBERMOI;
          tt.numbermoi.number_or_lerfu = norl;
          tt.numbermoi.moi = moi;
          assign_places(pre, post, lc, &tt);
          if ((pre->n_nodes > 0) ||
              (post->n_nodes > 0)) {
            xrb = prop_require_brac (norl, YES);
          }
        }
        break;
/*}}}*/
      case KE_SELBRI3_TU2:/*{{{*/
        {
          TreeNode *cs3;

          cs3 = find_nth_child(c1, 1, SELBRI_3);
          assert(cs3);
          process_selbri_3_args(cs3, pre, post, lc);
        }

        break;/*}}}*/
      case SE_TU2:/*{{{*/
        {
          LinkConv newlc;
          int conv;
          TreeNode *se_child, *tu2_child;

          lc_copy(lc, &newlc);
          se_child = child_ref(c1, 0);
          tu2_child = find_nth_child(c1, 1, TANRU_UNIT_2);
          assert(tu2_child);

          conv = recover_se_conv(se_child);
          lc_append_se(&newlc, conv, se_child);

          process_tanru_unit_2_args(tu2_child, pre, post, &newlc);

        }

        break;/*}}}*/
      case JAI_TAG_TU2:/*{{{*/
        {
          LinkConv newlc;
          TreeNode *ctag, *tu2_child;
          XRequireBrac *xrb;

          lc_copy(lc, &newlc);
          ctag = find_nth_child(c1, 1, TAG);
          tu2_child = find_nth_child(c1, 1, TANRU_UNIT_2);
          assert(ctag);
          assert(tu2_child);
          lc_append_jai_tag(&newlc, ctag, tu2_child);
          xrb = prop_require_brac(tu2_child, YES);
          process_tanru_unit_2_args(tu2_child, pre, post, &newlc);
        }

        break;
/*}}}*/
      case JAI_TU2:/*{{{*/
        {
          LinkConv newlc;
          TreeNode *tu2_child;

          lc_copy(lc, &newlc);
          tu2_child = find_nth_child(c1, 1, TANRU_UNIT_2);
          assert(tu2_child);
          lc_append_jai(&newlc);
          process_tanru_unit_2_args(tu2_child, pre, post, &newlc);
        }
        break;
/*}}}*/
      case NAHE_TU2:/*{{{*/
        {
          TreeNode *tu2_child;
          tu2_child = find_nth_child(c1, 1, TANRU_UNIT_2);
          assert(tu2_child);
          process_tanru_unit_2_args(tu2_child, pre, post, lc);
        }
        break;
/*}}}*/
      case ABSTRACTION:/*{{{*/
        {
          TreeNode *nns, *c2, *nu, *nai;
          XTermTag tt;

          nns = child_ref(c1, 0);
          type_check(nns, NU_NAI_SEQ);
          c2 = maybe_strip_attitudinal(child_ref(nns, 0));
          if ((c2->type == N_CMAVO) &&
              (c2->data.cmavo.selmao == NU)) {
            /* A simple NU <subsentence> thing */
            nu = c2;
            /* Maybe null - not sure what to do with it as I haven't
               seen any examples */
            nai = find_nth_child(nns, 1, NAI);
            if (nai) {
              fprintf(stderr, "Don't know how to handle negated abstractor yet at line %d column %d - ignoring negation\n",
                      c1->start_line, c1->start_column);
            }

            tt.type = TTT_ABSTRACTION;
            tt.abstraction.nu = nu;
            assign_places(pre, post, lc, &tt);
            assign_conversion(lc, nu);
            
          } else {
            fprintf(stderr, "Can't handle connected abstractors wrt places yet at line %d column %d\n",
                    c1->start_line, c1->start_column);
          }
        }
        break;
/*}}}*/
      default:
        break;

    }
  } else if (c1->type == N_ZEI) {/*{{{*/

    XTermTag tt;

    tt.type = TTT_ZEI;
    tt.zei.zei = c1;
    assign_places(pre, post, lc, &tt);
    assign_conversion(lc, c1);
    /*}}}*/
  } else {
    abort();
  }
  

}
/*}}}*/
static void process_tanru_unit_1_args(TreeNode *tu1, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
/* Handle argument processing at the level of a tanru_unit_1.  */

  XTermVector *xtv; /* The linked sumti vector of the tu1 if any */
  XDoneTU1 *xdtu1; /* Property whose existence on the TU1 tree node
                      shows we have processed its linked sumti */
  TreeNode *tu2;

  xtv = prop_term_vector(tu1, NO);

  tu2 = child_ref(tu1, 0);

  if (xtv) {
    LinkConv newlc;
    lc_copy(lc, &newlc);
    lc_append_links(&newlc, xtv->vec);
    process_tanru_unit_2_args(tu2, pre, post, &newlc);
  } else {
    process_tanru_unit_2_args(tu2, pre, post, lc);
  }

  /* Put marker on the TU1 node so that we know its linked sumti have
     now been processed.  Later on we scan the entire parse tree for
     TU1 nodes that don't have this property already set - these are
     ones that are not part of tertau */

  xdtu1 = prop_done_tu1(tu1, YES);

}
/*}}}*/
static void process_selbri_6_args(TreeNode *s6, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
  /* Drill down into a selbri_6 etc */

  TreeNode *tu, *tu1, *cs6, *cs;
  
  /* For the cases with BO, I think it's only the very final term
     that's relevant.  For the guhek cases, I think these are like
     connectives, in selbri_4/5, it's like gek_sentence?? */

  type_check(s6, SELBRI_6);

  if (nch(s6) == 1) {
    tu = child_ref(s6, 0); /* Always first child */

    if (nch(tu) > 1) {
      fprintf(stderr, "tanru_unit at line %d column %d contains CEI, can't handle this yet\n",
              tu->start_line, tu->start_column);
    } else {
      tu1 = child_ref(tu, 0);
      process_tanru_unit_1_args(tu1, pre, post, lc);
    }
    
  } else {
    tu = find_nth_child(s6, 1, TANRU_UNIT);
    if (tu) {
      /* BO form */
      cs6 = find_nth_child(s6, 1, SELBRI_6);
      process_selbri_6_args(cs6, pre, post, lc);
    } else {
      cs = find_nth_child(s6, 1, SELBRI);
      cs6 = find_nth_child(s6, 1, SELBRI_6);
      process_selbri_args(cs, pre, post, lc);
      process_selbri_6_args(cs6, pre, post, lc);
    }
  }
}
/*}}}*/
static void process_selbri_5_args(TreeNode *s5, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
  /* Drill down into a selbri_5 etc */

  TreeNode *s6, *cs5;

  cs5 = s5;
  do {
    s6 = find_nth_child(cs5, 1, SELBRI_6);
    process_selbri_6_args(s6, pre, post, lc);
    cs5 = find_nth_child(cs5, 1, SELBRI_5);
  } while (cs5);

}
/*}}}*/
static void process_selbri_3_args(TreeNode *s3, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
  /* Drill down into a selbri_3 to try to recover a single tanru_unit_2 which is
     the tertau.  Apply the supplied args to that - i.e. work out the place
     structure, then go and mark all the referenced terms accordingly.  */

  TreeNode *s4, *s5, *ks3, *cs3; /* The selbri chain */

  type_check(s3, SELBRI_3);

  /* The tertau is always the last entry in the sequence of selbri_4's.
     Fortunately, this is immediately available via left recursion */
  s4 = find_nth_child(s3, 1, SELBRI_4);

  /* If the selbri_4 is non-trivial (i.e. has connectives in), we have
     to descend each branch, more or less as though there was a gihek
     at the sentence level.  Fortunately from this function onwards,
     the same termvectors apply to every child, so at least we're over
     that. */

  if (nch(s4) > 1) {

    do {
      s5 = find_nth_child(s4, 1, SELBRI_5);
      if (s5) {
        process_selbri_5_args(s5, pre, post, lc);
      } else {
        ks3 = find_nth_child(s4, 1, KE_SELBRI_3);
        assert(ks3);
        cs3 = find_nth_child(ks3, 1, SELBRI_3);
        assert(cs3);
        process_selbri_3_args(cs3, pre, post, lc);
      }

      s4 = find_nth_child(s4, 1, SELBRI_4);

    } while (s4);

  } else {
    s5 = find_nth_child(s4, 1, SELBRI_5);
    process_selbri_5_args(s5, pre, post, lc);
  }

}
/*}}}*/
static void process_selbri_args(TreeNode *s, TermVector *pre, TermVector *post, LinkConv *lc)/*{{{*/
{
  /* Drill down into a selbri etc */

  TreeNode *cs, *s1, *s2, *s3; /* The selbri chain */
  TermVector empty_tv;

  tv_init(&empty_tv);

  type_check(s, SELBRI);

  cs = s;
  do {
    s1 = find_nth_child(cs, 1, SELBRI_1);

    /* Try to get selbri_2, or go round again if there's a NA */
    s2 = find_nth_child(s1, 1, SELBRI_2);
    if (!s2) {
      cs = find_nth_child(s1, 1, SELBRI);
      assert(cs);
    }
  } while (!s2);

  /* OK, have selbri_2, the fun starts here if there's a CO. */

  s3 = child_ref(s2, 0); 
  if (nch(s2) > 1) {
    /* There is a CO following. Only the head terms apply to this
       first selbri_3, its other arguments are either linked sumti or
       are unspecified. */
    process_selbri_3_args(s3, pre, &empty_tv, lc);

    while (nch(s2) > 1) {
      s2 = find_nth_child(s2, 1, SELBRI_2);
    }

    s3 = child_ref(s2, 0);
    process_selbri_3_args(s3, &empty_tv, post, lc);

  } else {
    /* All arguments apply to the selbri_3 */
    process_selbri_3_args(s3, pre, post, lc);
  }
}/*}}}*/
static void process_main_selbri(TreeNode *ms, TermVector *pre, TermVector *post)/*{{{*/
{
  /* Handle each main selbri after it has been located.  */

  TermVector *cpre, *cpost;
  XTermVectors *xtv;
  TreeNode *s;
  LinkConv lc;

  type_check(ms, MAIN_SELBRI);

  cpre = new(TermVector);
  cpost = new(TermVector);
  *cpre = *pre;
  *cpost = *post;
  /* Do we really need to store this property???!!! We do all the
     processing on these vectors below! */
  xtv = prop_term_vectors(ms, YES);
  xtv->pre = cpre;
  xtv->post = cpost;

  lc_init(&lc);

  /*

    Roll up the sleeves ..., this is where the real grunge starts.
    
   */

  s = child_ref(ms, 0); /* selbri */

  process_selbri_args(s, pre, post, &lc);

}/*}}}*/
static void process_bridi_tail_3(TreeNode *bt3, TermVector *pre, TermVector *post)/*{{{*/
{
  TreeNode *c1, *gsc, *tt, *ttc;
  TermVector tail_terms, new_post;

  type_check(bt3, BRIDI_TAIL_3);

  c1 = child_ref(bt3, 0);
  if (c1->data.nonterm.type == MAIN_SELBRI) {
    /* Do main_selbri processing --- make a property record for the
       pre and post vectors and tag it onto that node */
    TreeNode *ms;
    ms = c1;

    tt = find_nth_child(bt3, 1, TAIL_TERMS);
    if (tt) {
      ttc = find_nth_child(tt, 1, TERMS);
      if (ttc) {
        tv_build(&tail_terms, ttc);
        tv_catenate(&tail_terms, post, &new_post);
        process_main_selbri(ms, pre, &new_post);
      } else {
        process_main_selbri(ms, pre, post);
      }
    } else {
      process_main_selbri(ms, pre, post);
    }
  } else {
    /* Gek sentence */
    TreeNode *gs, *ss1, *ss2, *tt, *ttt;
    gs = c1;
    type_check(gs, GEK_SENTENCE);

    /* Do gek_sentence processing.

       If first child is not a 'gek', the sentence is a 'tag KE ..'
       format one.  Drill down through that until we come to something
       of the form 'gek subsentence...' .*/
    while (gsc = child_ref(gs, 0),
           ((gsc->type != N_NONTERM) ||
            (gsc->data.nonterm.type != GEK))) {
      gs = find_nth_child(gs, 1, GEK_SENTENCE);
    }

    /* OK , got it. Extract the two subsentences */
    ss1 = find_nth_child(gs, 1, SUBSENTENCE);
    ss2 = find_nth_child(gs, 2, SUBSENTENCE);
    assert(ss1);
    assert(ss2);
    tt = find_nth_child(gs, 1, TAIL_TERMS);
    if (tt) {
      ttt = find_nth_child(tt, 1, TERMS);
      if (ttt) {
        TermVector new_post, tail_terms;

        tv_build(&tail_terms, ttt);
        tv_catenate(&tail_terms, post, &new_post);
        process_subsentence(ss1, pre, &new_post);
        process_subsentence(ss2, pre, &new_post);
      } else {
        process_subsentence(ss1, pre, post);
        process_subsentence(ss2, pre, post);
      }
    } else {
      process_subsentence(ss1, pre, post);
      process_subsentence(ss2, pre, post);
    }

  }
    
  return;
}/*}}}*/
static void process_bridi_tail_2(TreeNode *bt2, TermVector *pre, TermVector *post)/*{{{*/
{

  TreeNode *bt3, *bt2a, *bt2b, *tt, *ttt; /* The children */
  struct nonterm *nt;
  int nc;

  /* The structure of a bridi_tail_2 is, for this fn's purposes,

     bridi_tail_3
   | bridi_tail_2 [CONNECTIVE bridi_tail_2 [tail_terms]]

     */

  nt = & bt2->data.nonterm;
  nc = nt->nchildren;

  if (nc == 1) {
    /* Just ananlyse the child */
    bt3 = nt->children[0];
    process_bridi_tail_3(bt3, pre, post);
  } else {
    TermVector new_post, tail_terms;

    bt2a = find_nth_child(bt2, 1, BRIDI_TAIL_2);
    assert(bt2a);
    bt2b = find_nth_child(bt2, 2, BRIDI_TAIL_2);
    assert(bt2b);

    tt = find_nth_child(bt2, 1, TAIL_TERMS);
    if (tt) {
      ttt = find_nth_child(tt, 1, TERMS);
      if (ttt) {
        tv_build(&tail_terms, ttt);
        tv_catenate(&tail_terms, post, &new_post);
        process_bridi_tail_2(bt2a, pre, &new_post);
        process_bridi_tail_2(bt2b, pre, &new_post);
      } else {
        process_bridi_tail_2(bt2a, pre, post);
        process_bridi_tail_2(bt2b, pre, post);
      }
    } else {
      process_bridi_tail_2(bt2a, pre, post);
      process_bridi_tail_2(bt2b, pre, post);
    }
  }

  return;

}/*}}}*/
static void process_bridi_tail_1(TreeNode *bt1, TermVector *pre, TermVector *post)/*{{{*/
{

  TreeNode *bt2, *bt1c, *tt, *ttt; /* The children */
  struct nonterm *nt;
  int nc;

  /* The structure of a bridi_tail_1 is, for this fn's purposes,

     bridi_tail_2 
   | bridi_tail_1 [CONNECTIVE bridi_tail_2 [tail_terms]]

     */

  nt = & bt1->data.nonterm;
  nc = nt->nchildren;

  if (nc == 1) {
    /* Just ananlyse the child */
    bt2 = nt->children[0];
    process_bridi_tail_2(bt2, pre, post);
  } else {
    TermVector new_post, tail_terms;

    bt1c = find_nth_child(bt1, 1, BRIDI_TAIL_1);
    assert(bt1c);
    bt2 = find_nth_child(bt1, 1, BRIDI_TAIL_2);
    assert(bt2);

    tt = find_nth_child(bt1, 1, TAIL_TERMS);
    if (tt) {
      ttt = find_nth_child(tt, 1, TERMS);
      if (ttt) {
        tv_build(&tail_terms, ttt);
        tv_catenate(&tail_terms, post, &new_post);
        process_bridi_tail_1(bt1c, pre, &new_post);
        process_bridi_tail_2(bt2, pre, &new_post);
      } else {
        process_bridi_tail_1(bt1c, pre, post);
        process_bridi_tail_2(bt2, pre, post);
      }
    } else {
      process_bridi_tail_1(bt1c, pre, post);
      process_bridi_tail_2(bt2, pre, post);
    }
  }

  return;

}/*}}}*/
static void process_bridi_tail(TreeNode *bt, TermVector *pre, TermVector *post)/*{{{*/
{
/*
  Analyse a bridi_tail down to each vector of tail_terms and find each
  main_selbri within it.  Ultimately tag each main_selbri with the
  vectors of pre- and post- terms that apply to it.

  TreeNode *bt The bridi_tail to analyse

  TermVector *pre The vector of all sumti-like terms in before the
  'CU'.  Note, for gek-sentences, this is the concatenation of all
  such pre-term sequences.

  TermVector *post The vector of tail_terms accumulated from outer
  bridi_tail constructions.  (Initially this will be empty).

  */

  TreeNode *bt1, *btc, *tt, *ttt; /* The children */
  struct nonterm *nt;
  int nc;

  /* The structure of a bridi_tail is, for this fn's purposes,

     bridi_tail_1 [CONNECTIVE bridi_tail [tail_terms]]

     */

  nt = & bt->data.nonterm;
  nc = nt->nchildren;
  

  if (nc == 1) {
    /* Just ananlyse the child */
    bt1 = nt->children[0];
    process_bridi_tail_1(bt1, pre, post);
  } else {
    TermVector new_post, tail_terms;

    btc = find_nth_child(bt, 1, BRIDI_TAIL);
    assert(btc);

    bt1 = find_nth_child(bt, 1, BRIDI_TAIL_1);
    assert(bt1);
    
    tt = find_nth_child(bt, 1, TAIL_TERMS);
    if (tt) {
      ttt = find_nth_child(tt, 1, TERMS);
      if (ttt) {
        tv_build(&tail_terms, ttt);
        tv_catenate(&tail_terms, post, &new_post);
        process_bridi_tail_1(bt1, pre, &new_post);
        process_bridi_tail(btc, pre, &new_post);
      } else {
        process_bridi_tail_1(bt1, pre, &new_post);
        process_bridi_tail(btc, pre, post);
      }
    } else {
      process_bridi_tail_1(bt1, pre, &new_post);
      process_bridi_tail(btc, pre, post);
    }
  }
}/*}}}*/
static void process_statement_3(TreeNode *x)/*{{{*/
{
  struct nonterm *nt, *nts, *ntc;
  TreeNode *sent, *terms, *btail, *c;
  TermVector pre_terms, tail_terms;

  assert(x->type == N_NONTERM && x->data.nonterm.type == STATEMENT_3);

  nt = & x->data.nonterm;

  if (nt->nchildren == 1) {
    /* Must be the form where 'sentence' is the only child.  All the
       forms with TUhE and text_1 have at least 2 child nodes */

    sent = nt->children[0]; /* This is the 'sentence' inside */

    /* If the sentence is 'no_cu_sentence' or 'observative_sentence', drill down */

    assert(sent->type == N_NONTERM);

    nts = &sent->data.nonterm;
    if (nts->nchildren == 1) {
      c = nts->children[0];
      if (c->type == N_NONTERM) {
        ntc = & c->data.nonterm;
        if ((ntc->type == NO_CU_SENTENCE) || (ntc->type == OBSERVATIVE_SENTENCE)) {
          sent = c;
          nts = ntc;
        }
      }
    }

    /* Now looking at the '[terms [CU #]] bridi_tail' node. */
    terms = find_nth_child(sent, 1, TERMS);
    btail = find_nth_child(sent, 1, BRIDI_TAIL);
    assert(btail); /* Every sentence has one of these! */

    tv_init(&tail_terms);
    if (terms) {
      tv_build(&pre_terms, terms);
    } else {
      tv_init(&pre_terms);
    }

    process_bridi_tail(btail, &pre_terms, &tail_terms);

#if 0
    if (terms) {
      /* Test code, for now */
      TermVector tv;
      int i, n;
      TreeNode *c;

      tv_build(&tv, terms);
      n = tv.n_nodes;
      for (i=0; i<n; i++) {
        c = tv.nodes[i].node;
        fprintf(stderr, "Term at line %d col %d is ", c->start_line, c->start_column);
        switch (tv.nodes[i].type) {
          case TRM_POS:
            fprintf(stderr, "Ordinary positional sumti\n");
            break;
          case TRM_FA:
            fprintf(stderr, "Positioned sumti, place=%d\n", tv.nodes[i].pos);
            break;
          case TRM_TAG:
            fprintf(stderr, "Tagged sumti\n");
            break;
        }
      }
    }
#endif
  }
}/*}}}*/
static void process_abstraction(TreeNode *x)/*{{{*/
{
  TreeNode *ss;
  TermVector pre, post;

  tv_init(&pre);
  tv_init(&post);
  ss = child_ref(x, 1);
  type_check(ss, SUBSENTENCE);
  process_subsentence(ss, &pre, &post);
}/*}}}*/
static void process_relative_clause(TreeNode *x)/*{{{*/
{
  TreeNode *ss, *fc;
  TermVector pre, post;

  tv_init(&pre);
  tv_init(&post);
  fc = child_ref(x, 0);
  if (fc->data.nonterm.type == FULL_RELATIVE_CLAUSE) {
    ss = find_nth_child(fc, 1, SUBSENTENCE);
    if (ss) {
      process_subsentence(ss, &pre, &post);
    }
  }
}/*}}}*/
static void process_metalinguistic(TreeNode *x)/*{{{*/
{
  /* Handle metalinguistic constructions (SEI...).  This is slightly
    different to the other cases, in that we have to resolve the terms and selbri
    ourselves.  */

  TermVector pre, post;
  TreeNode *terms, *mmselbri, *selbri;
  LinkConv lc;

  terms = find_nth_child(x, 1, TERMS);
  mmselbri = find_nth_child(x, 1, METALINGUISTIC_MAIN_SELBRI);
  assert(mmselbri);
  selbri = child_ref(mmselbri, 0);
  tv_init(&pre);
  tv_init(&post);
  
  if (terms) {
    tv_build(&pre, terms);
  }

  lc_init(&lc);
  process_selbri_args(selbri, &pre, &post, &lc);

}/*}}}*/
static void scan_for_sentence_parents(TreeNode *x)/*{{{*/
{
  /* Seek recursively downwards looking for treenodes of type
    STATEMENT_3, ABSTRACTION.  We're really interested in SENTENCE, but that
    occurs within ge subsentence->sentence gi subsentence->sentence, and we
    have to track term strings into gek_sentences because you can have terms in
    front plus tail terms behind, that are part of both clauses.

    Relative clauses and NU abstractions (the 2 other places where sentences
    can occur) will have to be looked at afterwards.  */

  int nc, i;
  struct nonterm *nt;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;

    if (nt->type == STATEMENT_3) {
      process_statement_3(x);
    } else if (nt->type == ABSTRACTION) {
      process_abstraction(x);
    } else if (nt->type == RELATIVE_CLAUSE) {
      process_relative_clause(x);
    } else if (nt->type == METALINGUISTIC) {
      process_metalinguistic(x);
    }

    /* Traverse down even for statement_3 (there may be abstractors,
       TO..TOI etc buried inside) */
    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      scan_for_sentence_parents(c);
    }

  }
}/*}}}*/
static void process_sumti_5b(TreeNode *x)/*{{{*/
{
  /* Scan down into the selbri inside a 'quantifier selbri' sumti, to mark up the
     tertau for conversions.  */

  TreeNode *c;
  LinkConv lc;

  TermVector empty_tv;
  tv_init(&empty_tv);

  c = find_nth_child(x, 1, SELBRI);
  assert(c);
  lc_init(&lc);
  process_selbri_args(c, &empty_tv, &empty_tv, &lc);
}/*}}}*/
static void process_sumti_tail_1a(TreeNode *x)/*{{{*/
{
  /* Scan down into the selbri inside a sumti_tail construction.  */ 

  TreeNode *c;
  LinkConv lc;

  TermVector empty_tv;
  tv_init(&empty_tv);

  c = find_nth_child(x, 1, SELBRI);
  assert(c);
  lc_init(&lc);
  process_selbri_args(c, &empty_tv, &empty_tv, &lc);
}/*}}}*/
static void scan_for_selbri_in_sumti(TreeNode *x)/*{{{*/
{
  /* Process selbri in the context of sumti.  Allows conversions to be
    handled with full generality.  */

  int nc, i;
  struct nonterm *nt;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;

    if (nt->type == SUMTI_5B) {
      process_sumti_5b(x);
    } else if (nt->type == SUMTI_TAIL_1A) {
      process_sumti_tail_1a(x);
    }

    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      scan_for_selbri_in_sumti(c);
    }
  }
}/*}}}*/
static void check_tu1_for_links(TreeNode *tu1)/*{{{*/
{
/* Check whether there are linked sumti and add property */

  TreeNode *la, *tm, *lk, *tc, *tcc;
  TermVector tv;
  XTermVector *xtv;

  type_check(tu1, TANRU_UNIT_1);

  if (tu1->data.nonterm.nchildren > 1) {
    /* There is a linkargs field */
    tv_init(&tv);
    la = tu1->data.nonterm.children[1]; /* linkargs */
    type_check(la, LINKARGS);
    do {
      tm = find_nth_child(la, 1, TERM);
      lk = find_nth_child(la, 1, LINKS);
      assert(tm); /* Always a term present */
      /* But not always an lk */
      
      /* Now, what sort of term is it? Have to drill down one layer further */
      tc = child_ref(tm, 0);

      switch (tc->data.nonterm.type) {
        case TERM_PLAIN_SUMTI:
          tv.nodes[tv.n_nodes].type = TRM_POS;
          tv.nodes[tv.n_nodes].node = tm;
          tv.n_nodes++;
          break;
        case TERM_PLACED_SUMTI:
          {
            int pos;
            tcc = child_ref(tc, 0);
            pos = recover_fa_conv(tcc);
            if (pos == 0) {
              tv.nodes[tv.n_nodes].type = TRM_FAhI;
              tv.nodes[tv.n_nodes].node = tm;
              tv.nodes[tv.n_nodes].pos = 0;
            } else {
              tv.nodes[tv.n_nodes].type = TRM_FA;
              tv.nodes[tv.n_nodes].node = tm;
              tv.nodes[tv.n_nodes].pos = pos;
            }
            tv.n_nodes++;
          }
          break;
        case TERM_TAGGED_SUMTI:
        case TAGGED_TERMSET:
          tv.nodes[tv.n_nodes].type = TRM_TAG;
          tv.nodes[tv.n_nodes].node = tm;
          tv.n_nodes++;
          break;
        case TERMSET:
          fprintf(stderr, "Termset at line %d ignored, place tagging will be faulty\n", tm->start_line);
          break;
        case TERM_FLOATING_TENSE:
        case TERM_FLOATING_NEGATE:
        case TERM_OTHER:
        default:
          /* None of these are interesting for place tagging */
          break;
      }
      
      la = lk;
    } while (la);

    /* Attach property to node */
    xtv = prop_term_vector(tu1, YES);
    xtv->vec = new(TermVector);
    *(xtv->vec) = tv;
  }
}/*}}}*/
static void scan_tu1_phase1(TreeNode *x)/*{{{*/
{
  /* Find any tanru_unit_1 with linked sumti on it, and build a termvector
     property to attach to it.  */

  int nc, i;
  struct nonterm *nt;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;

    if (nt->type == TANRU_UNIT_1) {
      check_tu1_for_links(x);
    }

    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      scan_tu1_phase1(c);
    }
  }
}
/*}}}*/
static void scan_tu1_phase2(TreeNode *x)/*{{{*/
{
  int nc, i;
  struct nonterm *nt;
  TreeNode *c;

  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;

    if (nt->type == TANRU_UNIT_1) {
      XTermVector *xtv;
      XDoneTU1 *xdtu1;

      xtv = prop_term_vector(x, NO);
      xdtu1 = prop_done_tu1(x, NO);
      if (xtv && !xdtu1) {
        LinkConv lc;
        TreeNode *tu2;
        TermVector pre, post;
        
        tu2 = child_ref(x, 0);
        
        lc_init(&lc);
        lc_append_links(&lc, xtv->vec);
        tv_init(&pre);
        tv_init(&post);
        
        process_tanru_unit_2_args(tu2, &pre, &post, &lc);
        xdtu1 = prop_done_tu1(x, YES);
      }
    }

    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      scan_tu1_phase2(c);
    }
  }

}
/*}}}*/
void terms_processing(TreeNode *top)/*{{{*/
{
  scan_tu1_phase1(top);
  scan_for_sentence_parents(top);
  scan_for_selbri_in_sumti(top);
  scan_tu1_phase2(top);
}
/*}}}*/
