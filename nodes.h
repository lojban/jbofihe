/***************************************
  $Header$

  Node type definitions for use in the bison parser and its interface
  with the lexer / preprocessor.
  ***************************************/

/* COPYRIGHT */


#ifndef NODES_H
#define NODES_H    /*+ To stop multiple inclusions. +*/

#include "nonterm.h"

struct treenode;

typedef enum {
  N_MARKER,
  N_GARBAGE,
  N_CMAVO,
  N_ZOI,
  N_ZO,
  N_LOhU,
  N_BU,
  N_BRIVLA,
  N_CMENE,
  N_NONTERM
} NodeTypes;

struct marker {
  int tok; /* To lie at same offset as selmao in struct cmavo, to
              facilitate error lookahead, given that all values are
              disjoint. */
  char *text;
};

struct garbage {
  char *word;
};

struct cmavo {
  int selmao; /* which can be modified to do token groupings,
                 e.g. SE_BAI etc as required to work around the
                 grammar not being LR(1) */
  int code; /* Reference into cmavo table */
};

struct zoi {
  char *form; /* zoi or la'o */
  char *term; /* delimiter */
  char *text; /* body text */
};

struct zo {
  char *text;
};

struct lohu {
  char *text;
};

struct brivla {
  char *word;
};

struct cmene {
  char *word;
};

struct bu {
  char *word;
};

typedef enum {
  BR_NONE,
  BR_ROUND,
  BR_SQUARE,
  BR_BRACE,
  BR_ANGLE,
  BR_CEIL,
  BR_FLOOR,
  BR_TRIANGLE
} BracketType;

struct nonterm {
  int nchildren;
  int number; /* Sequence number (can print out to help user match start and end of constructs) */
  BracketType brackets; /* bracketing to apply to this construction */
  NonTerm type;
  struct treenode **children;
};

struct treenode;

/* ================================================== */
/* Extension field types */

typedef enum extension_type {
  EX_CONV,
  EX_BAICONV,
  EX_DONTGLOSS,
  EX_TERMVECTORS,
  EX_TERMVECTOR,
  EX_TERMTAGS,
  EX_GLOSSTYPE,
  EX_DONETU1,
  EX_DONES3,
  EX_TENSECTX,
  EX_NEGINDICATOR,
  EX_CAIINDICATOR,
  EX_CONNECTIVE,
  EX_ANTECEDENT,
  EX_REQUIREBRAC,
  EX_RELCLAUSELINK,
  EX_CONTAINSKEHA
} ExtensionType;

typedef struct x_conversion {
  int conv; /* Which place of the word goes into the x1 place of the
               construction. */
} XConversion;

typedef struct x_baiconversion {
  int conv;
} XBaiConversion;

typedef struct x_dontgloss {
  int pad;
} XDontGloss;

struct TermVector;

typedef struct x_termvectors {
  struct TermVector *pre;
  struct TermVector *post;
} XTermVectors;

typedef struct x_termvector {
  struct TermVector *vec;
} XTermVector;

typedef struct x_glosstype {
  int in_selbri; /* 1 in main selbri, 0 in a sumti */
  int is_tertau; /* 1 if it's the tertau */
} XGlosstype;

/* ================================================== */

typedef enum {
  TTT_BRIVLA,
  TTT_JAITAG,
  TTT_JAI,
  TTT_ABSTRACTION,
  TTT_ME,
  TTT_GAhO,
  TTT_NUhA
} XTermTagType;

typedef struct {
  struct treenode *x;
} XTT_Brivla;

typedef struct {
  struct treenode *tag;
} XTT_JaiTag;

typedef struct {
  int pad;
} XTT_Jai;

typedef struct {
  struct treenode *nu;
} XTT_Abstraction;

typedef struct {
  struct treenode *sumti;
} XTT_Me;

typedef struct {
  int pad;
} XTT_Gaho;

typedef struct {
  int pad;
} XTT_Nuha;
  
typedef struct x_termtag {
  XTermTagType type;
  int pos; /* Needs extending to do JAI etc */

  /* Not union, because the jai variants need to access the brivla one
     too.  Look at cleaning this up sometime */
  XTT_Brivla brivla;
  XTT_JaiTag jaitag;
  XTT_Jai    jai;
  XTT_Abstraction abstraction;
  XTT_Me     me;
  XTT_Gaho   gaho;
  XTT_Nuha   nuha;
} XTermTag;

typedef struct x_termtags {
  struct x_termtags *next;
  struct x_termtag  tag;
} XTermTags;

typedef struct x_donetu1 {
  int pad;
} XDoneTU1;

typedef struct x_dones3 {
  int pad;
} XDoneS3;

typedef struct x_tensectx {
  enum tense_contexts {
    TSC_OTHER,
    TSC_SELBRI,
    TSC_TERM,
    TSC_NOUN,
    TSC_LINK,
    TSC_CONNECT,
    TSC_JAITAG
  } ctx;
} XTenseCtx;

typedef struct x_negindicator {
  int pad;
} XNegIndicator;

typedef struct x_caiindicator {
  enum cai_codes {
    CC_CAI,
    CC_SAI,
    CC_RUhE,
    CC_CUhI,
    CC_RUhENAI,
    CC_SAINAI,
    CC_CAINAI,
    CC_PEI,
    CC_PEINAI
  } code;
} XCaiIndicator;

typedef struct x_connective {
  enum connective_position {
    CNP_GE,      /* ge part of forethought connective */
    CNP_GI,      /* gi part of forethought connective */
    CNP_GE_JOIK, /* joik_gi in a gek */
    CNP_GI_JOIK, /* gi matched with a joik_gi */
    CNP_GE_STAG, /* stag_gik in a gek */
    CNP_GI_STAG, /* gi matched with a stag_gik */
    CNP_AFTER /* after-thought connective */
  } pos;

  /* This is used for the logical connectives */
  char *pattern; /* TTFF etc */

  /* This is used for the non-logical and tag connectives */
  struct treenode *js; /* joik or stag */
  int neg1; /* true if first half has NAI applied */
  int neg2; /* true if second half has NAI applied */
} XConnective;

typedef struct {
  struct treenode *node;
} XAntecedent;

typedef struct {
  int pad;
} XRequireBrac;

typedef struct {
  struct treenode *rel;
} XRelClauseLink;

typedef struct {
  int pad;
} XContainsKeha;

typedef union {
  XConversion conversion;
  XBaiConversion bai_conversion;
  XDontGloss dont_gloss;
  XTermVectors term_vectors;
  XTermVector term_vector;
  XTermTags   term_tags;
  XGlosstype  glosstype;
  XDoneTU1 done_tu1;
  XDoneS3 done_s3;
  XTenseCtx tense_ctx;
  XNegIndicator neg_indicator;
  XCaiIndicator cai_indicator;
  XConnective connective;
  XAntecedent antecedent;
  XRequireBrac require_brac;
  XRelClauseLink rel_clause_link;
  XContainsKeha contains_keha;
} ExtensionData;

typedef struct extension {
  struct extension *next;
  enum extension_type type;
  ExtensionData data;
} Extension;

/* ================================================== */
/* The main tree node type */

typedef struct treenode {
  struct treenode *next;
  struct treenode *prev;
  struct treenode *parent;      /* Back link to the parent
                                   nonterminal, to allow parse tree
                                   traversal later */
  struct treenode *bahe;        /* Points singly to any bahe node
                                   preceding the current one */
  struct treenode *ui_next;     /* Indicators linked list */
  struct treenode *ui_prev;

  /* Extension data if any, NULL if not (bahe/ui might be stuffed on
     here when it's shaken down a bit) */
  struct extension *ext;

  int eols; /* Number of line feeds following token in input */
  int start_line, start_column; /* Where does token start in input */
  NodeTypes type;
  union {
    struct marker  marker;
    struct garbage garbage;
    struct cmavo   cmavo;
    struct zoi     zoi;
    struct zo      zo;
    struct lohu    lohu;
    struct bu      bu;
    struct brivla  brivla;
    struct cmene   cmene;
    struct nonterm nonterm;
  } data;
} TreeNode;

struct tk_cmavo {
  int selmao;
};

struct tk_marker {
  int tok;
};

#endif /* NODES_H */
