/***************************************
  $Header$

  Monitor passage of elidable tokens into the parser, and determine whether one
  or more of them could be discarded with no change in meaning of the text.
  ***************************************/

/* COPYRIGHT */

/* Define to get lots of extra debug info */
#define TRACE_SR 0

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

/* Command line options -se & -sev */
extern int show_elisions;
extern int show_elisions_verbose;

/* Structure for holding 1 bit flags */

typedef struct {
  /* Is token is BOI or VEhO */
  unsigned boiveho:1;

  /* Flags indicating whether a rule of type 'subscript' was reduced *prior*
     to this token being shifted */
  unsigned subscript_before:1;

  /* Flag indicating whether token is one of the private markers NUMBER_ROI,
     NUMBER_MOI, NUMBER_MEI */
  unsigned tok_is_number_marker:1;

} OneBitFlags;


/* Structure + single instance for buffering a sequence of zero or more
   potentially elidable separators/terminators followed by a single
   non-elidable symbol. */

struct {
  int n; /* Number of tokens buffered */
  int max; /* Current size of dynamic arrays */
  int rp; /* Index of next token to be read */
  int sp; /* Index of next token to be shifted */

  
  TokenType *buf; /* Token buffer */

  /* conflicts array works as follows ...
  
     conflicts[k] has (k-1) bits in it.  If the jth bit is set, it means the kth
     entry of buf could have been shifted in one of the reducing states that is
     encountered prior to the jth entry of buf being shifted. */

  unsigned long **conflicts;

  OneBitFlags *flags;

} tokbuf = { 
  0, 0, 0, 0, NULL, NULL, NULL
};

/* Flag indicating that previous token was PA prior to tokens being read into the tokbuf. */ 
static int prev_token_was_pa_etc = 0;
static int cur_token_was_pa_etc = 0;

/*++++++++++++++++++++++++++++++++++++++
  Called to set a flag bit
  ++++++++++++++++++++++++++++++++++++++*/

static inline void
set_conflict(int could_be_shifted, int lookahead)
{
  int j = lookahead;
  int k = could_be_shifted;
  int bank = j >> 5;
  unsigned long mask = 1UL << (j & 31);
  tokbuf.conflicts[k][bank] |= mask;
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
  return !!(tokbuf.conflicts[k][bank] & mask);
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
clear_conflicts(void)
{
  int k, j, bank;
  unsigned long mask;
  for (k = 0; k < tokbuf.n; k++) {
    for (j = 0; j <= (k>>5); j++) {
      tokbuf.conflicts[k][j] = 0UL;
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
  tokbuf.buf       = grow_array(TokenType,       tokbuf.max, tokbuf.buf);
  tokbuf.conflicts = grow_array(unsigned long *, tokbuf.max, tokbuf.conflicts);
  tokbuf.flags     = grow_array(OneBitFlags,     tokbuf.max, tokbuf.flags);

  for (i=first; i < tokbuf.max; i++) {
    tokbuf.conflicts[i] = (unsigned long *) Malloc(sizeof(unsigned long) * nblocks);
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Return 1 if the cmavo is elidable
  ++++++++++++++++++++++++++++++++++++++*/

static int
token_is_elidable(YYSTYPE tok, int *is_boiveho)
{
  if (!tok) return 0; /* end of string */
  if (tok->type != N_CMAVO) return 0;

  *is_boiveho = 0;
  
  switch (cmavo_table[tok->data.cmavo.code].code) {
    case CM_BEhO:
    case CM_CU:
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
    case CM_TEhU:
    case CM_TUhU:
    case CM_VAU:
      /* Any of these is elidable, with the exception that if the cmavo has
         indicators or 'free' terms attached to it, the meaning would obviously
         change if it were omitted; in this case, it functions as a 'peg' on
         which to hang indicators/'free' terms that apply to the entire
         construction it terminates. */

      if (tok->data.cmavo.followed_by_free == 1) return 0;

      if (tok->ui_next != (TreeNode *)&tok->ui_next) {
        return 0;
      } else {
        return 1;
      }
      break;

    /* These 3 can *never* have 'free' terms bound to them, because they only
       ever terminate sub-rules of 'free' themselves.  Hence it is only
       relevant whether they are followed by indicators. */
    case CM_DOhU:
    case CM_SEhU:
    case CM_TOI:
      if (tok->ui_next != (TreeNode *)&tok->ui_next) {
        return 0;
      } else {
        return 1;
      }
      break;

    case CM_BOI:
    case CM_VEhO:

      *is_boiveho = 1;
    
      /* These two have to be handled specially.  Because of the way they
         appear in the 'subscript' rules, the appearance of a 'free' term
         afterwards is not enough to force them to be included.  They are
         unlike SEhU, DOhU, TOI because those can *never* have a 'free' term
         attached to them.  BOI and VEhO can (within 'quantifier').  Hence, BOI
         and VEhO have to be speculatively kept elidable, and a check made
         later on as to whether one of the subscript rules is reduced after
         they have been shifted. */  
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
  Detect whether a token is one of the private marker tokens that categ.c
  inserts to ease the parser's task
  ++++++++++++++++++++++++++++++++++++++*/

static int
token_is_number_marker(TreeNode *x)
{
  int result;
  
  if (x) {
    if (x->type == N_MARKER) {
      switch (x->data.marker.tok) {
        case PRIVATE_NUMBER_ROI:
        case PRIVATE_NUMBER_MAI:
        case PRIVATE_NUMBER_MOI:
          result = 1;
          break;
        default:
          result = 0;
          break;
      }
    } else {
      result = 0;
    }
  } else {
    result = 0;
  }
  return result;
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
  Generate report for verbose mode
  ++++++++++++++++++++++++++++++++++++++*/

static void
produce_verbose_report(void)
{
  int i, j;
  for (i=0; i<tokbuf.n; i++) {
    putchar(' ');
    putchar(' ');
    for (j=0; j<=i; j++) {
      if (j == i) {
        TreeNode *tok = tokbuf.buf[j].yylval;
        char *text;
        if (j == (tokbuf.n - 1)) {
          text = "<NEXT WORD>";
        } else {
          text = cmavo_table[tok->data.cmavo.code].cmavo;
        }
        printf(" <- %s\n", text);
      } else {
        printf("%s|", is_set(i, j) ? "X" : " ");
      }
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

#if TRACE_SR
  printf("Shift called in state %d\n", yystate);
#endif

  tokbuf.sp = tokbuf.rp;

  if (tokbuf.sp == tokbuf.n) {
    /* do reporting - the non-elidable has been shifted */
    int i, j, any_set=0;
    TreeNode *tok;
    int nn;

    /* Fix-up code for BOI & VEhO.  If one of these is followed by something
       that starts a 'free' term, it is not until we know whether the BOI/VEhO
       was reduced at the end of a subscript (rather than a quantifier) that we
       can determine whether the BOI/VEhO has to be treated as non-elidable or
       not */

    nn = tokbuf.n; /* less typing */

#if 0
    printf("nn=%d\n", nn); fflush(stdout);
    for (i=0; i<nn; i++) {
      TreeNode *xx = tokbuf.buf[i].yylval;
      printf("bv[%d]=%d sb[%d]=%d fbf[%d]=%d\n",
             i, tokbuf.boiveho[i],
             i, tokbuf.subscript_before[i],
             i, xx ? xx->data.cmavo.followed_by_free : 0);
      fflush(stdout);
    }
    printf("\n");
#endif
    
    if (tokbuf.flags[nn-2].boiveho &&
        !tokbuf.flags[nn-1].subscript_before &&
        tokbuf.buf[nn-2].yylval->data.cmavo.followed_by_free) {
      /* BOI/VEhO occurred at the end of a quantifier; if it was at the end
         of a subscript, the parser would have reduced the subscript before
         getting to the point where the 'free'-starting token could shift.
         Therefore, the following 'free' term is attached to the BOI/VEhO
         itself rather than being a peer of the subscript the BOI/VEhO is the
         end of, so back up to make BOI/VEhO the non-elidable 'NEXT TOKEN' in
         the report. */
#if 0
      printf("Retreated one token for boi/ve'o\n");
#endif
      tokbuf.n--;
    }

#if 0
    printf("bv0=%d ptp=%d tnm[n-1]=%d\n",
           tokbuf.flags[0].boiveho,
           prev_token_was_pa_etc,
           tokbuf.flags[tokbuf.n - 1].tok_is_number_marker);
#endif

    /* If non-elidable was a marker that precedes numbers, see whether first
       elidable in the buffer is boi or ve'o, and whether this was preceded by
       a token that would merge with the number if all the intervening
       elidables were discarded.  If so, insert a synthetic conflict by
       pretending that the marker is shiftable prior to the boi/ve'o.  This
       will provide the required reporting later. */
    if (tokbuf.flags[0].boiveho &&
        prev_token_was_pa_etc &&
        tokbuf.flags[tokbuf.n - 1].tok_is_number_marker) {
        
     set_conflict(tokbuf.n - 1, 0);
    }

    if (show_elisions_verbose && (tokbuf.n > 1)) {
      printf("Early shift hazards for words ");
      emit_elidable_sequence();
      produce_verbose_report();
      putchar('\n');
    }

#if TRACE_SR
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
#if TRACE_SR
  printf("Got here, shift sp!=n, state=%d\n", yystate);
#endif
    for (i=tokbuf.rp; i<tokbuf.n; i++) {
      if (can_shift_in_state(yystate, tokbuf.buf[i].value)) {
        /* Because it's a separator rather than a terminator, CU needs
           special treatment, because there will be a state where it conflicts
           with the following token for shifting. */
        int offset, priv, t1, t2, t3;
        offset =  1;
        t1 = (i == tokbuf.n - offset);
        t2 = (tokbuf.sp == tokbuf.n - offset);
        t3 = (tokbuf.buf[tokbuf.sp - 1].value == CU);
#if TRACE_SR
        printf("Got here 1 i=%d rp=%d sp=%d n=%d tests=%d,%d,%d\n", i, tokbuf.rp, tokbuf.sp, tokbuf.n, t1, t2, t3);
#endif
        if (t1 && t2 && t3) {
          /* Do nothing */
        } else {
          /* Default case, set hazard flag.
             Beware, use old value of tokbuf.sp (reverse effect of earlier
             increment). */
#if TRACE_SR
        printf("Got here 2, setting [%d,%d] in state %d\n", i, tokbuf.sp-1, yystate);
#endif
          set_conflict(i, tokbuf.sp-1);

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

  /* Check whether rule being reduced is one of type 'subscript' - to support
     handling of BOI and VEhO */
  if ((yyn >= LO_SUBSCRIPT) && (yyn <= HI_SUBSCRIPT)) {
#if 0
    printf("Setting sb[%d]=1\n", tokbuf.sp); fflush(stdout);
#endif
    tokbuf.flags[tokbuf.sp].subscript_before = 1;
  }

  if (tokbuf.sp == tokbuf.rp) {
    /* Bison has not read the lookahead token.  Therefore, any reduction
       occurring now must be just a automatic reduction, with no option to
       shift based on the lookahead token type.  Consequently, there is no
       option of shifting a token further on in the chain. */

#if TRACE_SR
    printf("Reduce called in state %d for rule %d, no check\n", yystate, yyn);
#endif

  } else {
    /* Otherwise, check which later tokens could have shifted in the state being
       reduced.  Any that can shift will imply the need for an earlier token to
       be present to guard against the shift happening */
#if TRACE_SR
    printf("Reduce called in state %d for rule %d\n", yystate, yyn);
#endif
    for (i=tokbuf.rp; i<tokbuf.n; i++) {
      if (can_shift_in_state(yystate, tokbuf.buf[i].value)) {
#if TRACE_SR
          printf("Got here 3, setting [%d,%d] in state %d\n", i, tokbuf.sp, yystate);
#endif
        set_conflict(i, tokbuf.sp);
      }
    }
  }
  return;
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
  int is_elidable;
  int is_boiveho;
  int is_private;

  if (!show_elisions) {
    /* Minimalist approach, no need for the fancy buffering */
    TokenType tok;
    yylex1(&tok);
    yylval = tok.yylval;
    yylloc = tok.yylloc;
    return tok.value;
  } /* else ... */


  /* Shift along the register to track whether we got a PA or a letter cmavo
     right before starting a block of elidables */

  prev_token_was_pa_etc = cur_token_was_pa_etc;

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

      tokbuf.flags[old_n].boiveho = 0;
      tokbuf.flags[old_n].subscript_before = 0;

      is_elidable = token_is_elidable(tokbuf.buf[old_n].yylval, &is_boiveho);
      tokbuf.flags[old_n].boiveho = is_boiveho;
      tokbuf.flags[old_n].tok_is_number_marker = token_is_number_marker(tokbuf.buf[old_n].yylval);
    } while (is_elidable);

    clear_conflicts();

#if 0
    fprintf(stderr, "refill gave %d tokens\n", tokbuf.n);
#endif

    yylval = tokbuf.buf[0].yylval;
    yylloc = tokbuf.buf[0].yylloc;
    result = tokbuf.buf[0].value;
    tokbuf.sp = tokbuf.rp;
    ++tokbuf.rp;

    if (tokbuf.rp == tokbuf.n) {
      /* Single token, or non-elidable at end of tokbuf */
      switch (result) {
        case PA:
        case BU:
        case BY:
        case LAU:
        case TEI:
        case FOI:
          cur_token_was_pa_etc = 1;
          break;
        default:
          cur_token_was_pa_etc = 0;
          break;

      }
    }
    
    return result;

  } 
  
}

/*++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++++++++++++*/


