/***************************************
  $Header$

  Do processing to do with forethought and afterthought connectives,
  e.g. grouping NA, SE and NAI onto the bare connective to work out a
  combined truth function that may be glossed as a whole.
  ***************************************/

/* COPYRIGHT */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"
#include "nonterm.h"
#include "functions.h"
#include "rpc_tab.h"
#include "cmavotab.h"

static char *truth_ex[] = {
  "FFFF", "FFFT", "FFTF", "FFTT",
  "FTFF", "FTFT", "FTTF", "FTTT",
  "TFFF", "TFFT", "TFTF", "TFTT",
  "TTFF", "TTFT", "TTTF", "TTTT",
  "QQQQ"
};

/*+ Index as follows : 8*(index in (a,e,o,u) [0..3]) + (4 if
  se-converted) + (2 if first term negated) + (1 if second term
  negated) +*/
static int main_table[] = {
  0xe,  0xd,  0xb,  0x7,  0xe,  0xd,  0xb,  0x7,
  0x8,  0x4,  0x2,  0x1,  0x8,  0x4,  0x2,  0x1,
  0x9,  0x6,  0x6,  0x9,  0x9,  0x6,  0x6,  0x9,
  0xc,  0xc,  0x3,  0x3,  0xa,  0x5,  0xa,  0x5,
  0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10
};

typedef enum {
  TR_OTHER = -1,
  TR_A     = 0,
  TR_E     = 1,
  TR_O     = 2,
  TR_U     = 3,
  TR_Q     = 4
} TruthFunction;


/*++++++++++++++++++++++++++++++++++++++
  

  static int generate_truth_index

  int neg1

  int conv

  int neg2

  TruthFunction func
  ++++++++++++++++++++++++++++++++++++++*/

static int
generate_truth_index(int neg1, int conv, int neg2, TruthFunction func)
{
  int index, result;

  assert(func != TR_OTHER);

  index = (8*(int)func) + 4*(conv ? 1:0) + 2*(neg1 ? 1:0) + (neg2 ? 1:0);
  result = main_table[index];
  

#if 0
  fprintf(stderr, "func=%d conv=%d neg1=%d neg2=%d index=%d result=%d (%s)\n",
          (int)func, conv, neg1, neg2, index, result, truth_ex[result]);
#endif

  return result;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x

  TruthFunction func
  ++++++++++++++++++++++++++++++++++++++*/


static void
process_afterthought(TreeNode *x, TruthFunction func)
{
  TreeNode *na, *se, *nai;
  int neg1, neg2, conv;
  int index;
  XConnective *xcon;

  na  = find_nth_cmavo_child(x, 1, NA);
  se  = find_nth_cmavo_child(x, 1, SE);
  nai = find_nth_cmavo_child(x, 1, NAI);

  neg1 = na  ? 1 : 0;
  conv = se  ? 1 : 0;
  neg2 = nai ? 1 : 0;

  index = generate_truth_index(neg1, conv, neg2, func);

  /* Bind property to x */
  xcon = prop_connective(x, YES);
  xcon->pos = CNP_AFTER;

  /* Can be pointer to this fixed table below */
  xcon->pattern = truth_ex[index];

  return;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_jek(TreeNode *x)
{
  TreeNode *key;
  char *text;
  TruthFunction func;

  key = find_nth_cmavo_child(x, 1, JA);
  assert(key);
  
  text = cmavo_table[key->data.cmavo.code].cmavo;
  if (!strcmp(text, "ja")) {
    func = TR_A;
  } else if (!strcmp(text, "je")) {
    func = TR_E;
  } else if (!strcmp(text, "jo")) {
    func = TR_O;
  } else if (!strcmp(text, "ju")) {
    func = TR_U;
  } else if (!strcmp(text, "je'i")) {
    func = TR_Q;
  } else {
    func = TR_OTHER;
  }

  if (func != TR_OTHER) {
    process_afterthought(x, func);
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_ek(TreeNode *x)
{
  TreeNode *key;
  char *text;
  TruthFunction func;

  key = find_nth_cmavo_child(x, 1, A);
  assert(key);
  
  text = cmavo_table[key->data.cmavo.code].cmavo;
  if (!strcmp(text, "a")) {
    func = TR_A;
  } else if (!strcmp(text, "e")) {
    func = TR_E;
  } else if (!strcmp(text, "o")) {
    func = TR_O;
  } else if (!strcmp(text, "u")) {
    func = TR_U;
  } else if (!strcmp(text, "ji")) {
    func = TR_Q;
  } else {
    func = TR_OTHER;
  }

  if (func != TR_OTHER) {
    process_afterthought(x, func);
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_gihek(TreeNode *x)
{
  TreeNode *key;
  char *text;
  TruthFunction func;

  key = find_nth_cmavo_child(x, 1, GIhA);
  assert(key);
  
  text = cmavo_table[key->data.cmavo.code].cmavo;
  if (!strcmp(text, "gi'a")) {
    func = TR_A;
  } else if (!strcmp(text, "gi'e")) {
    func = TR_E;
  } else if (!strcmp(text, "gi'o")) {
    func = TR_O;
  } else if (!strcmp(text, "gi'u")) {
    func = TR_U;
  } else if (!strcmp(text, "gi'i")) {
    func = TR_Q;
  } else {
    func = TR_OTHER;
  }

  if (func != TR_OTHER) {
    process_afterthought(x, func);
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x

  TruthFunction func
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_forethought(TreeNode *x, TruthFunction func)
{
  TreeNode *se, *nai1, *nai2, *gik;
  int neg1, neg2, conv;
  int index;
  XConnective *xcon;

  gik = find_nth_child(x->parent, 1, GIK);
  assert(gik);

  se   = find_nth_cmavo_child(x,   1, SE);
  nai1 = find_nth_cmavo_child(x,   1, NAI);
  nai2 = find_nth_cmavo_child(gik, 1, NAI);

  neg1 = nai1 ? 1 : 0;
  conv = se   ? 1 : 0;
  neg2 = nai2 ? 1 : 0;

  index = generate_truth_index(neg1, conv, neg2, func);

  /* Bind property to x (=gek/guhek) */
  xcon = prop_connective(x, YES);
  xcon->pos = CNP_GE;

  /* Can be pointer to this fixed table below */
  xcon->pattern = truth_ex[index];

  /* Bind property to gik */
  xcon = prop_connective(gik, YES);
  xcon->pos = CNP_GI;

  /* Can be pointer to this fixed table below */
  xcon->pattern = truth_ex[index];

  return;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_guhek(TreeNode *x)
{
  TreeNode *key;
  char *text;
  TruthFunction func;

  key = find_nth_cmavo_child(x, 1, GUhA);
  assert(key);
  
  text = cmavo_table[key->data.cmavo.code].cmavo;
  if (!strcmp(text, "gu'a")) {
    func = TR_A;
  } else if (!strcmp(text, "gu'e")) {
    func = TR_E;
  } else if (!strcmp(text, "gu'o")) {
    func = TR_O;
  } else if (!strcmp(text, "gu'u")) {
    func = TR_U;
  } else if (!strcmp(text, "gu'i")) {
    func = TR_Q;
  } else {
    func = TR_OTHER;
  }

  if (func != TR_OTHER) {
    process_forethought(x, func);
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *gek

  TreeNode *gi
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_joik_gi(TreeNode *gek, TreeNode *gi)
{
  TreeNode *nai2, *mid_gik, *joik;
  int neg1, neg2;
  XConnective *xcon;

  mid_gik = find_nth_child(gek->parent, 1, GIK);
  assert(mid_gik);
  joik = find_nth_child(gek, 1, JOIK);
  assert(joik);

  nai2 = find_nth_cmavo_child(mid_gik, 1, NAI);

  neg1 = 0; /* inconsistently with everything else, you can't negate
               the first phrase */
  neg2 = nai2 ? 1 : 0;

  /* Bind property to gek */
  xcon = prop_connective(gek, YES);
  xcon->pos = CNP_GE_JOIK;
  xcon->js = joik;
  xcon->neg1 = neg1;
  xcon->neg2 = neg2;

  /* Bind property to mid_gik */
  xcon = prop_connective(mid_gik, YES);
  xcon->pos = CNP_GI_JOIK;
  xcon->js = joik;
  xcon->neg1 = neg1;
  xcon->neg2 = neg2;

  return;
}

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *gek

  TreeNode *gik
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_stag_gik(TreeNode *gek, TreeNode *gik)
{
  TreeNode *nai1, *nai2, *mid_gik, *stag;
  int neg1, neg2;
  XConnective *xcon;

  mid_gik = find_nth_child(gek->parent, 1, GIK);
  assert(mid_gik);
  stag = find_nth_child(gek, 1, STAG);
  assert(stag);

  nai1 = find_nth_cmavo_child(gik,     1, NAI);
  nai2 = find_nth_cmavo_child(mid_gik, 1, NAI);

  neg1 = nai1 ? 1 : 0;
  neg2 = nai2 ? 1 : 0;

  /* Bind property to gek */
  xcon = prop_connective(gek, YES);
  xcon->pos = CNP_GE_STAG;
  xcon->js = stag;
  xcon->neg1 = neg1;
  xcon->neg2 = neg2;

  /* Bind property to mid_gik */
  xcon = prop_connective(mid_gik, YES);
  xcon->pos = CNP_GI_STAG;
  xcon->js = stag;
  xcon->neg1 = neg1;
  xcon->neg2 = neg2;

  return;
}

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_gek(TreeNode *x)
{
  TreeNode *key;
  char *text;
  TruthFunction func;

  key = find_nth_cmavo_child(x, 1, GA);

  if (!key) {
    TreeNode *gi, *gik;
    gik = find_nth_child(x, 1, GIK);
    gi = find_nth_cmavo_child(x, 1, GI);
    assert (gik || gi);
    if (gi) {
      process_joik_gi(x, gi);
    } else {
      process_stag_gik(x, gik);
    }
  } else {
  
    text = cmavo_table[key->data.cmavo.code].cmavo;

    if (!strcmp(text, "ga")) {
      func = TR_A;
    } else if (!strcmp(text, "ge")) {
      func = TR_E;
    } else if (!strcmp(text, "go")) {
      func = TR_O;
    } else if (!strcmp(text, "gu")) {
      func = TR_U;
    } else if (!strcmp(text, "ge'i")) {
      func = TR_Q;
    } else {
      func = TR_OTHER;
    }
    
    if (func != TR_OTHER) {
      process_forethought(x, func);
    }
    
  }
}

/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static void
traverse_tree(TreeNode *x)
{
  int nc, i;
  struct nonterm *nt;
  TreeNode *c;
  
  if (x->type == N_NONTERM) {
    nt = &x->data.nonterm;

    /* Process specific types of node */
    switch (nt->type) {
      case GIHEK:
        process_gihek(x);
        break;

      case EK:
        process_ek(x);
        break;

      case JEK:
      case JEK_OPT_KE:
      case JEK_OPT_KEBO:
        process_jek(x);
        break;

      case GUHEK:
        process_guhek(x);
        break;

      case GEK:
        process_gek(x);
        break;

      default:
        break;
    }

    nc = nt->nchildren;
    for (i=0; i<nc; i++) {
      c = nt->children[i];
      traverse_tree(c);
    }
  }


}

/*++++++++++++++++++++++++++++++++++++++
  The outermost routine for connectives processing, visible from outside

  TreeNode *top
  ++++++++++++++++++++++++++++++++++++++*/

void
connectives_processing(TreeNode *top)
{
  traverse_tree(top);

}
