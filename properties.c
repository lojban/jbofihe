/***************************************
  $Header$

  Manipulate property lists on tree nodes.
  ***************************************/

/* COPYRIGHT */

#include "nodes.h"
#include "functions.h"


/*++++++++++++++++++++++++++++++
  Internal function to search for a particular extension record within
  a particular tree node.
  ++++++++++++++++++++++++++++++*/

static Extension *
lookup_extension(TreeNode *x, ExtensionType type)
{
  Extension *y;
  y = x->ext;
  while (y && y->type != type) {
    y = y->next;
  }

  return y;
}

/*++++++++++++++++++++++++++++++
  Lookup an existing extension record, or create a new one if not
  found.
  ++++++++++++++++++++++++++++++*/

static Extension *
obtain_extension(TreeNode *x, ExtensionType type)
{
  Extension *y;

  y = lookup_extension(x, type);
  if (!y) {
    y = new(Extension);
    y->type = type;
    y->next = x->ext;
    x->ext = y;
  }

  return y;

}


/* ================================================== */

/*++++++++++++++++++++++++++++++
  Get the conversion extension for a treenode
  ++++++++++++++++++++++++++++++*/

static ExtensionData *
fetch_conversion(TreeNode *x, int create, ExtensionType type)
{
  Extension *y;
  y = create ? obtain_extension(x, type) : lookup_extension (x, type);
  if (y) {
    return &(y->data);
  } else {
    return NULL;
  }

}


/*++++++++++++++++++++++++++++++
  Get the conversion extension for a treenode
  ++++++++++++++++++++++++++++++*/

XConversion *
prop_conversion(TreeNode *x, int create)
{
  return (XConversion *) fetch_conversion(x, create, EX_CONV);
}

/*++++++++++++++++++++++++++++++
  Get the bai_conversion extension for a treenode
  ++++++++++++++++++++++++++++++*/

XBaiConversion *
prop_bai_conversion(TreeNode *x, int create)
{
  return (XBaiConversion *) fetch_conversion(x, create, EX_BAICONV);
}

/*++++++++++++++++++++++++++++++
  Get the dont_gloss extension for a treenode
  ++++++++++++++++++++++++++++++*/

XDontGloss *
prop_dont_gloss(TreeNode *x, int create)
{
  return (XDontGloss *) fetch_conversion(x, create, EX_DONTGLOSS);
}

/*++++++++++++++++++++++++++++++
  Get the term_vectors extension for a treenode
  ++++++++++++++++++++++++++++++*/

XTermVectors *
prop_term_vectors(TreeNode *x, int create)
{
  return (XTermVectors *) fetch_conversion(x, create, EX_TERMVECTORS);
}

/*++++++++++++++++++++++++++++++
  Get the term_vector extension for a treenode
  ++++++++++++++++++++++++++++++*/

XTermVector *
prop_term_vector(TreeNode *x, int create)
{
  return (XTermVector *) fetch_conversion(x, create, EX_TERMVECTOR);
}

/*++++++++++++++++++++++++++++++
  Get the term_tags extension for a treenode
  ++++++++++++++++++++++++++++++*/

XTermTags *
prop_term_tags(TreeNode *x, int create)
{
  return (XTermTags *) fetch_conversion(x, create, EX_TERMTAGS);
}


/*++++++++++++++++++++++++++++++

  Get the glosstype extension for a treenode.  We have to do some
  initialisation here.

  ++++++++++++++++++++++++++++++*/

XGlosstype *
prop_glosstype(TreeNode *x, int create)
{
  XGlosstype *result;
  result = (XGlosstype *) fetch_conversion(x, NO, EX_GLOSSTYPE);
  if (!result && create) {
    result = (XGlosstype *) fetch_conversion(x, YES, EX_GLOSSTYPE);
    result->is_tertau = 0;
  }

  return result;
}

/*++++++++++++++++++++++++++++++
  Get the done_tu1 extension for a treenode
  ++++++++++++++++++++++++++++++*/

XDoneTU1 *
prop_done_tu1(TreeNode *x, int create)
{
  return (XDoneTU1 *) fetch_conversion(x, create, EX_DONETU1);
}

/*++++++++++++++++++++++++++++++
  Get the done_s3 extension for a treenode
  ++++++++++++++++++++++++++++++*/

XDoneS3 *
prop_done_s3(TreeNode *x, int create)
{
  return (XDoneS3 *) fetch_conversion(x, create, EX_DONES3);
}

/*++++++++++++++++++++++++++++++
  Get the tense_ctx extension for a treenode
  ++++++++++++++++++++++++++++++*/

XTenseCtx *
prop_tense_ctx(TreeNode *x, int create)
{
  return (XTenseCtx *) fetch_conversion(x, create, EX_TENSECTX);
}

/*++++++++++++++++++++++++++++++
  Get the neg_indicator extension for a treenode
  ++++++++++++++++++++++++++++++*/

XNegIndicator *
prop_neg_indicator(TreeNode *x, int create)
{
  return (XNegIndicator *) fetch_conversion(x, create, EX_NEGINDICATOR);
}


/*++++++++++++++++++++++++++++++
  Get the cai_indicator extension for a treenode
  ++++++++++++++++++++++++++++++*/

XCaiIndicator *
prop_cai_indicator(TreeNode *x, int create)
{
  return (XCaiIndicator *) fetch_conversion(x, create, EX_CAIINDICATOR);
}

/*++++++++++++++++++++++++++++++
  Get the tense_ctx extension for a treenode
  ++++++++++++++++++++++++++++++*/

XConnective *
prop_connective(TreeNode *x, int create)
{
  return (XConnective *) fetch_conversion(x, create, EX_CONNECTIVE);
}

/*++++++++++++++++++++++++++++++
  Get the antecedent extension for a treenode
  ++++++++++++++++++++++++++++++*/

XAntecedent *
prop_antecedent(TreeNode *x, int create)
{
  return (XAntecedent *) fetch_conversion(x, create, EX_ANTECEDENT);
}


/*++++++++++++++++++++++++++++++
  Get the require_brac extension for a treenode
  ++++++++++++++++++++++++++++++*/

XRequireBrac *
prop_require_brac(TreeNode *x, int create)
{
  return (XRequireBrac *) fetch_conversion(x, create, EX_REQUIREBRAC);
}

/*++++++++++++++++++++++++++++++
  Get the rel_clause_link extension for a treenode
  ++++++++++++++++++++++++++++++*/

XRelClauseLink *
prop_rel_clause_link(TreeNode *x, int create)
{
  return (XRelClauseLink *) fetch_conversion(x, create, EX_RELCLAUSELINK);
}

/*++++++++++++++++++++++++++++++
  Get the contains_keha extension for a treenode
  ++++++++++++++++++++++++++++++*/

XContainsKeha *
prop_contains_keha(TreeNode *x, int create)
{
  return (XContainsKeha *) fetch_conversion(x, create, EX_CONTAINSKEHA);
}

/*++++++++++++++++++++++++++++++
  Get the elidable extension for a treenode
  ++++++++++++++++++++++++++++++*/

XElidable *
prop_elidable(TreeNode *x, int create)
{
  return (XElidable *) fetch_conversion(x, create, EX_ELIDABLE);
}

