
/***************************************
  $Header$

  Monitor passage of elidable tokens into the parser, and determine whether one
  or more of them could be discarded with no change in meaning of the text.
  ***************************************/

/* COPYRIGHT */

/* Define to get lots of extra debug info */
/* #define TRACE_SR 1 */

#define YYSTYPE TreeNode *

#include <stdio.h>
#include "nodes.h"
#include "rpc_tab.h"
#include "cmavotab.h"
#include "functions.h"
#include "elide.h"

/* Include table built from Bison output, that has a bitmap of which
   tokens are shiftable in which states */
#include "elitabs.c"

/* Pointers grabbed from registration function */
static const short *yypact = NULL;
static const short *yytranslate = NULL;
static const short *yycheck = NULL;
static const short *yytable = NULL;

/* Command line option -se */
extern int show_elisions;


/* Structure + single instance for buffering a sequence of zero or more
   potentially elidable separators/terminators followed by a single
   non-elidable symbol. */

struct {
  int n; /* Number of tokens buffered */
  int max; /* Current size of buf and flags arrays */
  int rp; /* Index of next token to be read */
  int sp; /* Index of next token to be shifted */
  TokenType *buf; /* Token buffer */

  /* flags array works as follows ...
  
     flags[k] has (k-1) bits in it.  If the jth bit is set, it means the kth
     entry of buf could have been shifted in one of the reducing states that is
     encountered prior to the jth entry of buf being shifted. */

  unsigned long **flags;
  
} tokbuf = { 
  0, 0, 0, 0, NULL, NULL
};

/*++++++++++++++++++++++++++++++++++++++
  Called to set a flag bit
  ++++++++++++++++++++++++++++++++++++++*/

static inline void
set_flag(int could_be_shifted, int lookahead)
{
  int j = lookahead;
  int k = could_be_shifted;
  int bank = j >> 5;
  unsigned long mask = 1UL << (j & 31);
  tokbuf.flags[k][bank] |= mask;
}

/*++++++++++++++++++++++++++++++++++++++
  Called to check if a flag bit is set
  ++++++++++++++++++++++++++++++++++++++*/

static inline int
is_set(int could_be_shifted, int lookahead)
{
  int j = lookahead;
  int k = could_be_shifted;
  int bank = j >> 5;
  unsigned long mask = 1UL << (j & 31);
  return !!(tokbuf.flags[k][bank] & mask);
}

/*++++++++++++++++++++++++++++++++++++++
  Detect whether a token can shift in a state
  ++++++++++++++++++++++++++++++++++++++*/

static inline int
can_shift_in_state(int state, int value)
{
  int offset = value - CODE_OFFSET;
  int block = offset >> 5;
  unsigned long mask = 1UL << (offset & 31);
  
  block += state * BLOCKS_PER_STATE;

  return !!(elitab[block] & mask);
}

/*++++++++++++++++++++++++++++++++++++++
  Clear all flag bits
  ++++++++++++++++++++++++++++++++++++++*/

static void
clear_flags(void)
{
  int k, j, bank;
  unsigned long mask;
  for (k = 0; k < tokbuf.n; k++) {
    for (j = 0; j <= (k>>5); j++) {
      tokbuf.flags[k][j] = 0UL;
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Called to increase the size of the token buffer.
  ++++++++++++++++++++++++++++++++++++++*/

static void
grow_buffer(void)
{
  int i;
  int first;
  int nblocks;
  first = tokbuf.max;
  tokbuf.max += 32;
  nblocks = tokbuf.max >> 5;
  tokbuf.buf = (TokenType *) (tokbuf.buf ? Realloc(tokbuf.buf, sizeof(TokenType) * tokbuf.max) :
                                           Malloc (            sizeof(TokenType) * tokbuf.max));
                                          
  tokbuf.flags = (unsigned long **) (tokbuf.flags ? Realloc(tokbuf.flags, sizeof(unsigned long *) * tokbuf.max) :
                                                    Malloc (              sizeof(unsigned long *) * tokbuf.max));

  for (i=first; i < tokbuf.max; i++) {
    tokbuf.flags[i] = (unsigned long *) Malloc(sizeof(unsigned long) * nblocks);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Called by modified yyparse to register the addresses of the parser tables
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_register(const short *pact, const short *translate, const short *check, const short *table)
{
  yypact = pact;
  yytranslate = translate;
  yycheck = check;
  yytable = table;
}

/*++++++++++++++++++++++++++++++++++++++
  Return 1 if the cmavo is elidable
  ++++++++++++++++++++++++++++++++++++++*/

static int
is_elidable(YYSTYPE tok)
{
  if (!tok) return 0; /* end of string */
  if (tok->type != N_CMAVO) return 0;
  
  switch (cmavo_table[tok->data.cmavo.code].code) {
    case CM_BEhO:
    case CM_BOI:
    case CM_CU:
    case CM_DOhU:
    case CM_FEhU:
    case CM_GEhU:
    case CM_KEI:
    case CM_KEhE:
    case CM_KU:
    case CM_KUhE:
    case CM_KUhO:
    case CM_LIhU:
    case CM_LOhO:
    case CM_LUhU:
    case CM_MEhU:
    case CM_NUhU:
    case CM_SEhU:
    case CM_TEhU:
    case CM_TOI:
    case CM_TUhU:
    case CM_VAU:
    case CM_VEhO:
      /* Any of these is elidable, with the exception that if the cmavo has
         indicators attached to it, the meaning would obviously change if it
         were omitted; in this case, it functions as a 'peg' on which to hang
         indicators that apply to the entire construction it terminates. */
      if (tok->ui_next != (TreeNode *)&tok->ui_next) {
        return 0;
      } else {
        return 1;
      }
      break;
    default:
      return 0;
      break;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Emit the sequence of elidable cmavo + line/col no. information
  ++++++++++++++++++++++++++++++++++++++*/

static void
emit_elidable_sequence(void)
{
  int i;
  
  putchar('\'');
  for (i=0; i<tokbuf.n - 1; i++) {
    TreeNode *tok = tokbuf.buf[i].yylval;
    if (i > 0) putchar(' ');
    printf("%s", cmavo_table[tok->data.cmavo.code].cmavo);
  }
  putchar ('\'');
  if (tokbuf.n > 2) {
    printf("\n (from line %d col %d to line %d col %d)\n",
           tokbuf.buf[0].yylval->start_line,
           tokbuf.buf[0].yylval->start_column,
           tokbuf.buf[tokbuf.n - 2].yylval->start_line,
           tokbuf.buf[tokbuf.n - 2].yylval->start_column);
  } else {
    printf(" (at line %d col %d)\n",
           tokbuf.buf[0].yylval->start_line,
           tokbuf.buf[0].yylval->start_column);
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Print banner out for the report on minimal sequences (only do it before the
  first successful reduction in word count is about to be printed.)
  ++++++++++++++++++++++++++++++++++++++*/

static void
show_banner_for_minimal_paterns(void)
{
  int i;
  printf("The words ");
  emit_elidable_sequence();
  printf("could be safely reduced to any of these minimal patterns:\n");
}

/*++++++++++++++++++++++++++++++++++++++
  Recursive function to determine a minimal but sufficient set of elidables
  which forces the text to have the same meaning.  By 'minimal' is meant :
  retaining any more of the original elidables is superfluous, yet removing any
  of the 'minimal' set causes the meaning to change.

  int tok_idx Index into the tokbuf.tok array corr. to the token which is to be
  prevented from shifting in an earlier reducing state

  int *later_indices List of indices of tokens further to the right which are
  already required to be kept

  int next_index The next entry in the list to be filled.
  ++++++++++++++++++++++++++++++++++++++*/

static void
show_minimal_elidables(int tok_idx, int *later_indices, int next_index, int prev_can_shift_at, int *patcount)
{
  int can_shift_at;
  
  for (can_shift_at = tok_idx - 1;
       can_shift_at >= 0;
       can_shift_at--) {
    
    if (is_set(tok_idx, can_shift_at)) {
      break;
    }
  }

  if (can_shift_at < 0) {
    /* This token could not shift any earlier, so is safe as a first guard
       token. Print results out */
    if (next_index < tokbuf.n - 1) {
      /* A strict reduction in word count has been achieved. */
      
      int i;
      if (*patcount == 1) {
        show_banner_for_minimal_paterns();
      }
      printf ("  Pattern %d : ", *patcount);
      ++*patcount;
      for (i=next_index-1; i>=0; i--) {
        TreeNode *tok = tokbuf.buf[later_indices[i]].yylval;
        printf (" %s", cmavo_table[tok->data.cmavo.code].cmavo);
      }
      printf("\n");
    }
  } else {
    /* This token could have shifted earlier.  Some token between the
       can_shift_at posn. and the one before the tok_idx position is required
       to guard. */

    int i;
    for (i=can_shift_at; i < tok_idx; i++) {
      /* Eliminate cases where a shorter minimal sequence would be achieved by
         just discarding tokens to the right (in particular, the most recent
         token on the right.  In effect, the one we're about to pick to guard
         it would have guarded the one following it too anyway, making it
         pointless. */
      if (i >= prev_can_shift_at) break; /* No point considering any others */
      later_indices[next_index] = i;
      /* Recursive call to scan the tokens to the left of the new guard token
         */
      show_minimal_elidables(i, later_indices, next_index + 1, can_shift_at, patcount);
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Trace shifts by the parser.  When an elidable is shifted, we can cease
  checking for whether anything coming after it could have been shifted in
  a state where a reduction took place
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_shift(int yystate, int yychar)
{
  if (!show_elisions) return;

#ifdef TRACE_SR
  printf("Shift called in state %d\n", yystate);
#endif

  tokbuf.sp = tokbuf.rp;

  if (tokbuf.sp == tokbuf.n) {
    /* do reporting - the non-elidable has been shifted */
    /* For now, just see if the final token is safe, i.e, the full seq. could
      have been dropped */
    int i, j, any_set=0;
    TreeNode *tok;

#ifdef TRACE_SR
  printf("Got here, shift sp=n\n");
#endif
    
    for (i=0; i<tokbuf.n; i++) {
      if (is_set(tokbuf.n - 1, i)) {
        any_set = 1;
        break;
      }
    }
    
    if (!any_set && (tokbuf.n > 1)) {
      printf("You could omit ");
      switch (tokbuf.n) {
        case 2:  printf("the word"); break;
        case 3:  printf("both the words"); break;
        default: printf("all the words"); break;
      }
      putchar(' ');
      emit_elidable_sequence();
      putchar('\n');
    } else if (any_set) {
      /* array of indices for elidables that must be kept */
      int *idxs = new_array(int, tokbuf.n); /* sized for worst case */
      int i;
      int patcount = 1;
      show_minimal_elidables(tokbuf.n - 1, idxs, 0, tokbuf.n, &patcount);
      if (patcount > 1) printf("\n");
      Free(idxs);
    }
  } else {
    int i;
#ifdef TRACE_SR
  printf("Got here, shift sp!=n, state=%d\n", yystate);
#endif
    for (i=tokbuf.rp; i<tokbuf.n; i++) {
      if (can_shift_in_state(yystate, tokbuf.buf[i].value)) {
        /* Because it's a separator rather than a terminator, CU needs
           special treatment, because there will be a state where it conflicts
           with the following token for shifting. */
#ifdef TRACE_SR
        int t1, t2, t3;
        t1 = (i == tokbuf.n - 1);
        t2 = (tokbuf.sp == tokbuf.n - 1);
        t3 = (tokbuf.buf[tokbuf.sp - 1].value == CU);
        printf("Got here 1 i=%d rp=%d sp=%d n=%d tests=%d,%d,%d\n", i, tokbuf.rp, tokbuf.sp, tokbuf.n, t1, t2, t3);
#endif
        if ((i == tokbuf.n - 1) && (tokbuf.sp == tokbuf.n - 1) &&
            (tokbuf.buf[tokbuf.sp - 1].value == CU)) {
          /* Do nothing */
        } else {
          /* Default case, set hazard flag.
             Beware, use old value of tokbuf.sp (reverse effect of earlier
             increment). */
#ifdef TRACE_SR
        printf("Got here 2, setting [%d,%d]\n", i, tokbuf.sp-1);
#endif
          set_flag(i, tokbuf.sp-1);

        }
      }
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Trace reductions by the parser.  This is the clever bit where we check
  whether the following cmavo could shift in the state where we are reducing.
  If this doesn't happen in any state before the elidable is shifted, then the
  elidable was unnecessary.
  ++++++++++++++++++++++++++++++++++++++*/

void
elide_trace_reduce(int yystate, int yyn)
{
  int i;

  if (!show_elisions) return;

  if (tokbuf.sp == tokbuf.rp) {
    /* Bison has not read the lookahead token.  Therefore, any reduction
       occurring now must be just a automatic reduction, with no option to
       shift based on the lookahead token type.  Consequently, there is no
       option of shifting a token further on in the chain. */

#ifdef TRACE_SR
    printf("Reduce called in state %d, no check\n", yystate);
#endif
    return;
  } /* else */

  /* Otherwise, check which later tokens could have shifted in the state being
     reduced.  Any that can shift will imply the need for an earlier token to
     be present to guard against the shift happening */
#ifdef TRACE_SR
  printf("Reduce called in state %d\n", yystate);
#endif
  for (i=tokbuf.rp; i<tokbuf.n; i++) {
    if (can_shift_in_state(yystate, tokbuf.buf[i].value)) {
#ifdef TRACE_SR
        printf("Got here 3, setting [%d,%d]\n", i, tokbuf.sp);
#endif
      set_flag(i, tokbuf.sp);
    }
  }

}

/*++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++
  Provide the yylex function
  ++++++++++++++++++++++++++++++++++++++*/

int
yylex(void)
{
  extern YYSTYPE yylval;
  extern YYLTYPE yylloc;
  int result;
  int old_n;

  if (!show_elisions) {
    /* Minimalist approach, no need for the fancy buffering */
    TokenType tok;
    yylex1(&tok);
    yylval = tok.yylval;
    yylloc = tok.yylloc;
    return tok.value;
  } /* else ... */

  /* Otherwise, build up a buffer of arbitrarily many elidables with a single
     non-elidable at the end.  Drain this buffer into the bison layer,
     performing shift-ability checks on the later tokens as each bison state is
     reached. */

  if (tokbuf.rp < tokbuf.n) {
    yylval = tokbuf.buf[tokbuf.rp].yylval;
    yylloc = tokbuf.buf[tokbuf.rp].yylloc;
    result = tokbuf.buf[tokbuf.rp].value;
    tokbuf.sp = tokbuf.rp; /* Bison never has more than 1 lookahead token (!) */
    ++tokbuf.rp;
    return result;
    
  } else {
    tokbuf.rp = tokbuf.n = 0;
    do {
      if (tokbuf.n == tokbuf.max) {
        grow_buffer();
      }
      
      old_n = tokbuf.n;
      yylex1(&tokbuf.buf[tokbuf.n++]);

    } while (is_elidable(tokbuf.buf[old_n].yylval));

    clear_flags();

#if 0
    fprintf(stderr, "refill gave %d tokens\n", tokbuf.n);
#endif

    yylval = tokbuf.buf[0].yylval;
    yylloc = tokbuf.buf[0].yylloc;
    result = tokbuf.buf[0].value;
    tokbuf.sp = tokbuf.rp;
    ++tokbuf.rp;
    return result;

  } 
  
}

/*++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++*/


