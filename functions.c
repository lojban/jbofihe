/***************************************
  $Header$

  General functions.
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "functions.h"
#include "nonterm.h"
#include "rpc_tab.h"

/* command line option */
extern int insert_elidables;

/*++++++++++++++++++++++++++++++
  Go through children in a node and chain them onto that parent.
  Also, set the start line/column of the nonterminal to that of the first child.
  ++++++++++++++++++++++++++++++*/

static void
mark_children(TreeNode *parent)
{
  int i;
  struct nonterm *nt;
  TreeNode *c;

  assert(parent->type == N_NONTERM);
  nt = & parent->data.nonterm;
  for (i=0; i<nt->nchildren; i++) {
    c = nt->children[i];
    c->parent = parent;
  }
  if (nt->nchildren > 0) {
    parent->start_line = nt->children[0]->start_line;
    parent->start_column = nt->children[0]->start_column;
  } else {
    parent->start_line = 0;
    parent->start_column = 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Compress child array to remove NULLs.  This situation should only arise if
  one of the children is the result of matching an error token, and has no
  proper contents (e.g. paragraph).  Without this, the mark_children function
  will blow up.
  ++++++++++++++++++++++++++++++++++++++*/

static void
compress_children(TreeNode *parent)
{
  int i, j;
  struct nonterm *nt;

  assert(parent->type == N_NONTERM);
  nt = & parent->data.nonterm;

  j = 0;
  for (i=0; i<nt->nchildren; i++) {
    if (nt->children[j] = nt->children[i]) j++;
  }
  nt->nchildren = j;
  return;
}

/*++++++++++++++++++++++++++++++++++++++
  Return a new dynamically allocated token / node.

  TreeNode * new_node Return the new node.
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
new_node(void)
{
  TreeNode *result = new(TreeNode);
  result->ext = NULL;
  result->bahe = NULL;
  result->ui_next = result->ui_prev = (TreeNode *) &result->ui_next;
  result->eols = 0;
  return result;
}

/*++++++++++++++++++++++++++++++
  Create new non-terminal nodes
  ++++++++++++++++++++++++++++++*/

TreeNode *
new_elidable(int code, int selmao)
{
  TreeNode *result;
  if (insert_elidables) {
    result = new_node();
    result->type = N_CMAVO;
    result->start_line = result->start_column = 0;
    result->data.cmavo.code = code;
    result->data.cmavo.selmao = selmao;
    prop_elidable(result, YES);
  } else {
    result = NULL;
  }
  return result;
}

/*++++++++++++++++++++++++++++++
  Create new non-terminal nodes
  ++++++++++++++++++++++++++++++*/

TreeNode *
new_node_0(NonTerm nt)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 0;
  result->data.nonterm.children = NULL;
  result->data.nonterm.brackets = BR_NONE;

  /* only arises for empty TEXT anyway */
  result->start_line = result->start_column = 0;

  return result;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode * new_node_1

  NonTerm nt

  TreeNode *c1
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
new_node_1(NonTerm nt, TreeNode *c1)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 1;
  result->data.nonterm.children = new_array(TreeNode *, 1);
  result->data.nonterm.children[0] = c1;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode * new_node_2

  NonTerm nt

  TreeNode *c1

  TreeNode *c2
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
new_node_2(NonTerm nt, TreeNode *c1, TreeNode *c2)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 2;
  result->data.nonterm.children = new_array(TreeNode *, 2);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}


/*++++++++++++++++++++++++++++++++++++++
  

  TreeNode * new_node_3

  NonTerm nt

  TreeNode *c1

  TreeNode *c2

  TreeNode *c3
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
new_node_3(NonTerm nt, TreeNode *c1, TreeNode *c2, TreeNode *c3)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 3;
  result->data.nonterm.children = new_array(TreeNode *, 3);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

TreeNode *
new_node_4(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 4;
  result->data.nonterm.children = new_array(TreeNode *, 4);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

TreeNode *
new_node_5(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4, 
           TreeNode *c5)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 5;
  result->data.nonterm.children = new_array(TreeNode *, 5);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_6(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4, 
           TreeNode *c5, TreeNode *c6)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 6;
  result->data.nonterm.children = new_array(TreeNode *, 6);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_7(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 7;
  result->data.nonterm.children = new_array(TreeNode *, 7);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_8(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 8;
  result->data.nonterm.children = new_array(TreeNode *, 8);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  result->data.nonterm.children[7] = c8;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_9(NonTerm nt,
           TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
           TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
           TreeNode *c9)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 9;
  result->data.nonterm.children = new_array(TreeNode *, 9);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  result->data.nonterm.children[7] = c8;
  result->data.nonterm.children[8] = c9;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_10(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 10;
  result->data.nonterm.children = new_array(TreeNode *, 10);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  result->data.nonterm.children[7] = c8;
  result->data.nonterm.children[8] = c9;
  result->data.nonterm.children[9] = c10;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_11(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10, TreeNode *c11)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 11;
  result->data.nonterm.children = new_array(TreeNode *, 11);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  result->data.nonterm.children[7] = c8;
  result->data.nonterm.children[8] = c9;
  result->data.nonterm.children[9] = c10;
  result->data.nonterm.children[10] = c11;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

TreeNode *
new_node_12(NonTerm nt,
            TreeNode *c1, TreeNode *c2, TreeNode *c3, TreeNode *c4,
            TreeNode *c5, TreeNode *c6, TreeNode *c7, TreeNode *c8,
            TreeNode *c9, TreeNode *c10, TreeNode *c11, TreeNode *c12)
{
  TreeNode *result;
  result = new_node();
  result->type = N_NONTERM;
  result->data.nonterm.type = nt;
  result->data.nonterm.nchildren = 12;
  result->data.nonterm.children = new_array(TreeNode *, 12);
  result->data.nonterm.children[0] = c1;
  result->data.nonterm.children[1] = c2;
  result->data.nonterm.children[2] = c3;
  result->data.nonterm.children[3] = c4;
  result->data.nonterm.children[4] = c5;
  result->data.nonterm.children[5] = c6;
  result->data.nonterm.children[6] = c7;
  result->data.nonterm.children[7] = c8;
  result->data.nonterm.children[8] = c9;
  result->data.nonterm.children[9] = c10;
  result->data.nonterm.children[10] = c11;
  result->data.nonterm.children[11] = c12;
  compress_children(result);
  mark_children(result);
  result->data.nonterm.brackets = BR_NONE;
  return result;
}

/*++++++++++++++++++++++++++++++
  Check whether a node is non-terminal of a particular type
  ++++++++++++++++++++++++++++++*/

void
type_check(TreeNode *x, NonTerm type)
{
  assert((x->type == N_NONTERM) &&
         (x->data.nonterm.type == type));
}


/*++++++++++++++++++++++++++++++++++++++
  Return number of children a nonterminal has

  static int nch The number

  TreeNode *x The nonterminal.
  ++++++++++++++++++++++++++++++++++++++*/

int
nch(TreeNode *x)
{
  assert(x->type == N_NONTERM);
  return x->data.nonterm.nchildren;
}

/*++++++++++++++++++++++++++++++++++++++
  Return a reference to a particular child within a non-terminal,
  checking that the index is in bounds.

  static TreeNode * child_ref Returned reference

  TreeNode *x The nonterminal.

  int i The index (0..n-1)
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
child_ref(TreeNode *x, int i)
{
  struct nonterm *nt;
  assert(x->type == N_NONTERM);
  assert(i>=0);
  nt = & x->data.nonterm;
  assert(i < nt->nchildren);
  return nt->children[i];
}


/*++++++++++++++++++++++++++++++
  Find the nth node (counting from 1) of a particular type amongst the
  children of node x.  Return null if there aren't any.  Trap if x is
  terminal.
  ++++++++++++++++++++++++++++++*/

TreeNode *
find_nth_child(TreeNode *x, int n, NonTerm type)
{
  struct nonterm *nt;
  TreeNode *c;
  int i, nc, togo;

  assert(x->type == N_NONTERM);
  nt = & x->data.nonterm;

  togo = n;
  nc = nt->nchildren;
  for (i=0; i<nc; i++) {
    c = nt->children[i];
    if ((c->type == N_NONTERM) &&
        (c->data.nonterm.type == type)) {
      --togo;
      if (togo == 0) {
        return c;
      }
    }
  }

  return NULL;

}

/*++++++++++++++++++++++++++++++
  Find the nth node (counting from 1) of a particular type amongst the
  children of node x.  Return null if there aren't any.  Trap if x is
  terminal.  If one of the immediate children is a nonterm of type
  AUGMENTED, drill down into that.  This allows the child we are
  looking for to have attitudinals attached to it.

  TreeNode * find_nth_cmavo_child The returned child, NULL if not found

  TreeNode *x The parent child whose children are to be searched

  int n The index of the child to find (starting from 1 for the first)

  int selmao The selmao of the child being sought.
  ++++++++++++++++++++++++++++++*/

TreeNode *
find_nth_cmavo_child(TreeNode *x, int n, int selmao)
{
  struct nonterm *nt, *ntc;
  TreeNode *c, *cc;
  int i, j, nc, ncc, togo;

  assert(x->type == N_NONTERM);
  nt = & x->data.nonterm;

  togo = n;
  nc = nt->nchildren;
  for (i=0; i<nc; i++) {
    c = nt->children[i];
    if ((c->type == N_CMAVO) &&
        (c->data.cmavo.selmao == selmao)) {
      --togo;
      if (togo == 0) {
        return c;
      }
    } else if ((c->type == N_NONTERM) &&
               (c->data.nonterm.type == AUGMENTED)) {
      ntc = & c->data.nonterm;
      ncc = ntc->nchildren;
      for (j=0; j<ncc; j++) {
        cc = ntc->children[j];
        if ((cc->type == N_CMAVO) &&
            (cc->data.cmavo.selmao == selmao)) {
          --togo;
          if (togo == 0) {
            return cc;
          }
        }
      }
    }
  }

  return NULL;

}

/*++++++++++++++++++++++++++++++++++++++
  Given a treenode, either it is a cmavo, or it's an AUGMENTED with a
  non-attitudinal cmavo amongst the children.  Find the child with the right
  selmao, and trap if there isn't one.
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
strip_attitudinal(TreeNode *x)
{
  TreeNode *y;
  if (x->type == N_CMAVO) {
    return x;
  } else if (x->type == N_NONTERM) {
    TreeNode **child, **child0;
    int N;
    struct nonterm *nt;

    nt = &(x->data.nonterm);
    assert(nt->type == AUGMENTED);
    N = nt->nchildren;
    child = child0 = nt->children;
    while (((*child)->type == N_CMAVO) && ((*child)->data.cmavo.selmao == BAhE)) {
      child++;
      assert ((child - child0) < N);
    }
    return *child;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  If node is augmented, get main internal cmavo.
  ++++++++++++++++++++++++++++++++++++++*/

TreeNode *
maybe_strip_attitudinal(TreeNode *x)
{
  if ((x->type == N_NONTERM) && (x->data.nonterm.type == AUGMENTED)) {
    return strip_attitudinal(x);
  } else {
    return x;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Return 1 if all children are cmavo

  int is_simple_nonterm

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/

int
is_simple_nonterm(TreeNode *x)
{
  int ok;
  int nc;
  int i;
  struct nonterm *nt;
  TreeNode *c;

  assert(x->type == N_NONTERM);

  nt = &x->data.nonterm;
  nc = nt->nchildren;
  ok = 1;
  for (i=0; i<nc; i++) {
    c = nt->children[i];
    if (c->type == N_NONTERM) {
      ok = 0;
      break;
    }
  }
  return ok;
}


/*++++++++++++++++++++++++++++++++++++++
  Turn a string into uppercase.  Can only deal with one string at a time.
  ++++++++++++++++++++++++++++++++++++++*/

char *
make_uppercase(char *s)
{
  static char buffer[1024];
  char *p;
  strcpy(buffer, s);
  p = buffer;
  while (*p) *p = toupper(*p), p++;
  return buffer;
}
  
