/***************************************
  $Header$

  Carry out the morphology functions (hence the name) - take consecutive
  strings of non-whitespace from the input stream and determine what kind of
  word has been read, plus how many cmavo need to be detached from the front of
  it.

  This code relies on some scanning tables built by separate perl scripts.  It
  can be compiled with -DTEST_MORF to provide a standalone testbench for the
  scanners.  If invoked with -v (i.e. verbose) this testbench will show extra
  information about the internal scanner states.  The input to this testbench
  is provided on stdin, one word per line.
  
  ***************************************/

/* COPYRIGHT */

#ifdef TEST_MORF
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "canonluj.h"
static int verbose=0;
static int expand_lujvo = 0; /* Show decomposition of lujvo */
static int allow_cultural_rafsi = 1; /* In testbench mode, always allow */
#else

#include <stddef.h>

/* When linked into the main program, get this option from the command line
   in main.c */
extern int allow_cultural_rafsi;

#endif

#include "morf.h"
#include "morf_dfa.h"
#include "bccheck.h"

enum processed_category {/*{{{*/
  W_UNKNOWN,
  W_CMAVOS, W_CMAVOS_END_CY,
  W_GISMU,
  W_LUJVO,
  W_CULTURAL_LUJVO,
  W_FUIVLA3, W_FUIVLA3_CVC,
  W_FUIVLA3X, W_FUIVLA3X_CVC,
  W_FUIVLA4,
  W_CMENE,
  W_BAD_TOSMABRU, W_CULTURAL_BAD_TOSMABRU,
  W_BAD_SLINKUI,
  W_BIZARRE
};
/*}}}*/
/* Include table for turning the letter stream into meta-classes (consonant,
 * vowel, permissible pair etc).  These 'meta-classes' are the tokens used by
 * the DFA.  These tables are built my mk_fetab.pl */
#include "morf_lex.c"

/* Include file for checking vowel pairs/clusters within the input stream. */
#include "morfvlex.c"

static unsigned char s2l[32] = /*{{{*/
/* Map N->1, R->2, other C->3, else ->0.  Used to trim down the last-but-one
 * letter, which is saved to allow the front-end to spot illegal triples and
 * type III fu'ivla hyphen patterns. */
{
  0, 0, 3, 3, 3, 0, 3, 3,
  0, 0, 3, 3, 3, 3, 1, 0,
  3, 0, 2, 3, 3, 0, 3, 0,
  3, 0, 3, 0, 0, 0, 0, 0
};
/*}}}*/

#if defined(TEST_MORF)
static char *toknam[] =/*{{{*/
/* Token names for -v mode */
{
  "UNK", "V", "APOS", "Y", "R", "N", "C",
  "NR", "CI", "CSI", "CP", "CS", "CN",
  "H", "HS", "BT", "VV", "VX", "VO", 
  "VY", "YY"
};
/*}}}*/
static char *actnam[] =/*{{{*/
/* Front end state machine actions, printable for -v mode */
{
  "CLR", "SFT", "FRZ"
};
/*}}}*/
static char charnames[32] = {/*{{{*/
  '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  '?', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
  'p', '?', 'r', 's', 't', 'u', 'v', 'w',
  'x', 'y', 'z', '?', '?', '?', '\'', '?'
};
/*}}}*/
static char vowelnames[8] = {/*{{{*/
  ',', 'C', 'y', 'a', 'e', 'i', 'o', 'u'
};
/*}}}*/
static char Lname[4] = {/*{{{*/
  'V', 'n', 'r', 'C'
};
/*}}}*/
#endif

static unsigned char mapchar[256] =/*{{{*/
/* Map the ASCII set to the range 0..31 (mostly
   by masking high order bits off the letters,
   except the apostrophe is given the value 30) */
{
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
  0x98, 0x99, 0x9a, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f
};
/*}}}*/
static unsigned char vmapchar[256] = {/*{{{*/
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* invalid -> consonant code */
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* (reject by main FSM) */
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* apos. -> 1=cons */
  0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, /* comma -> 0 */
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x03, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, /* aeiou->34567 */
  0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x06, /* cons ->1 */
  0x01, 0x01, 0x01, 0x01, 0x01, 0x07, 0x01, 0x01, /* y -> 2 */
  0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x03, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, /* aeiou->34567 */
  0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x06, /* cons ->1 */
  0x01, 0x01, 0x01, 0x01, 0x01, 0x07, 0x01, 0x01, /* y -> 2 */
  0x01, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
/*}}}*/

/*********************************************************************/

MorfType morf_scan(char *s, char ***buf_end, struct morf_xtra *arg_xtra)/*{{{*/
/* The main scanning routine.  's' is the string to be scanned.  buf_end is a
   pointer to a table of pointers to characters (i.e. pass by reference so we
   can pass a result back.)  This table is filled in with the positions in 's'
   where the prefix cmavo start. */
{
  unsigned int L, S, G;
  unsigned int vsm = 0111; /* 3 consonants as starting state */
  char *p, c;
  char **start, **pstart;
  char *cstart[256], **pcstart; /* Used for holding start of Cy structures */
  int ent, tent, tok, set, inhibit;
  int state, next_state;
  int inhibited = 0, initial = 1;
  int decrement = 0;

  /* Remember previous non-comma positions */
  char *p_1 = NULL, *p_2 = NULL;
  
  /* Remember position for stage-3 hyphen after CVC or long rafsi prefix */
  char *hyph3 = NULL;
  char *hyph4 = NULL;
  
  enum raw_category exival;
  enum processed_category result;
  int had_uppercase=0;
  int letter_uppercase;
  MorfType ext_result;

  /* Gather info in a local copy, in case client doesn't want it.
   * (We have to gather it anyway, to support the outputs in test
   * mode.). */

  struct morf_xtra xtra;

#ifdef TEST_MORF
  int split_cmene = 0;
#endif

  typedef enum {
    ACT_CLEAR=0, ACT_SHIFT=1, ACT_FREEZE=2
  } Action;

  Action act, last_act;
  
  start = *buf_end;
  pstart = start;

  pcstart = cstart;
  
  L = S = 0;
  state = 0;
  last_act = ACT_CLEAR;

#ifdef TEST_MORF
  if (verbose) {
    printf ("Chr L     S      G      vsm  tok   act set inh state'\n");
  }
#endif

  p = s;

  /*{{{  Main per-character loop */
  while (*p) {
    c = *p;

    /* If char is a comma, just advance now. */
    if (c == ',') {
      p++;
      continue;
    }
    
    /* Commas are now discarded, even for checking vowel cluster validity */

    vsm = ((vsm & 077) << 3) | vmapchar[(unsigned char) c & 0xff];
    
    G = (unsigned int) mapchar[(unsigned char) c & 0xff];
    letter_uppercase = (G >> 7) & 1;
    had_uppercase |= letter_uppercase;
    G &= 0x1f;

    ent = (L<<10) + (S<<5) + G;
    tent = inact[ent];
    tok = (tent & 0xf);
    act = (Action)((tent >> 4) & 0x3);
    set = !!(tent & 0x40);
    inhibit = !!(tent & 0x80);

    /* Logic to remember position of each vowel->consonant transition + start
     * of string.  These positions will be the start of the prefixed cmavo or
     * of the brivla.  In the case of gismu/lujvo/fuivla starting with V+ or
     * CV+, a different match is recognized by the DFA so that the table can
     * have one entry purged from the RHS (i.e. get rid of the spurious pointer
     * to the cluster within the brivla.) */

    inhibited |= inhibit;
    if ((set && !inhibited) || initial) {
      *pstart++ = p;
    }

    /* If tok is V or Y, need to adapt it based on the vowel cluster checker */
    if (tok == 1) { /* vowel recognized by main FSM */
      tok = vcheck[vsm];
    } else if (tok == 3) { /* y recognized by main FSM */
      tok = vcheck[vsm];
    } else {
      /* Just check that there hasn't been a doubled comma.
         (Doubled commas followed by vowels or y will be checked by VSM LUT
         reporting an unknown token in that case.) */
      if (!(vsm & 0770)) {
        tok = 0; /* unknown token, will jam the automaton */
      } else {
        /* pass tok through from consonant front-end FSM */
      }
    }

    if ((last_act == ACT_FREEZE) && (act == ACT_SHIFT)) {
      /* This happens if the input has a y-to-consonant transition.  Such
       * positions may be the start of Cy cmavo, if the whole string ends in a
       * sequence of Cy cmavo. */
      *pcstart++ = p;
    }

    /*{{{  Run attribute code*/
    switch (morf_attribute[state]) {
      case AT_UNKNOWN:
        break;
      case AT_S3_3:
      case AT_XS3_3:
        hyph3 = p_2;
        break;
      case AT_S3_4:
      case AT_XS3_4:
        hyph4 = p_2;
        break;
    }
    /*}}}*/

    p_2 = p_1;
    p_1 = p;
    p++;
    initial = 0;

    /* next_state function from file built by dfa builder */
    next_state = morf_next_state(state, tok);

#ifdef TEST_MORF
    if (verbose) {
      printf ("%c   %01x(%c)  %02x(%c)  %02x(%c)  %c%c%c  %-4s  %-3s  %d   %d  %4d\n",
              c, L, Lname[L], S, charnames[S], G, charnames[G],
              vowelnames[(vsm>>6)&7], vowelnames[(vsm>>3)&7], vowelnames[vsm&7],
              toknam[tok], actnam[act], set, inhibit, next_state);
    }
#endif

    /*{{{  Run action on main (consonant) shift reg. */
    switch (act) {
      case ACT_CLEAR:
        L = S = 0;
        break;
      case ACT_SHIFT:
        L = s2l[S];
        S = G;
        break;
      case ACT_FREEZE:
        break; /* freeze S (to do pair check after 'y') */
      default:
        abort();
    }
    last_act = act;
    /*}}}*/
    
    state = next_state;

    if (state < 0) break; /* syntax error */
    
  }
  /*}}}*/

  if ((state < 0) || (morf_exitval[state] == R_UNKNOWN)) {
    result = W_UNKNOWN;
    ext_result = MT_BOGUS;
    decrement = 0;
  } else {
    exival = morf_exitval[state];
    /*{{{  Extract word-type and CV/CC start flag */
    switch (exival) {
      case R_CMAVOS: result = W_CMAVOS; decrement = 0; break;
      case R_CMAVOS_END_CY: result = W_CMAVOS_END_CY; decrement = 0; break;
      case R_GISMU_0: result = W_GISMU; decrement = 0; break;
      case R_GISMU_1: result = W_GISMU; decrement = 1; break;
      case R_LUJVO_0: result = W_LUJVO; decrement = 0; break;
      case R_LUJVO_1: result = W_LUJVO; decrement = 1; break;
      case R_CULTURAL_LUJVO_0: result = W_CULTURAL_LUJVO; decrement = 0; break;
      case R_CULTURAL_LUJVO_1: result = W_CULTURAL_LUJVO; decrement = 1; break;
      case R_STAGE3_0: result = W_FUIVLA3; decrement = 0; break;
      case R_STAGE3_1: result = W_FUIVLA3; decrement = 1; break;
      case R_STAGE3_1_CVC: result = W_FUIVLA3_CVC; decrement = 1; break;
      case R_X_STAGE3_0: result = W_FUIVLA3X; decrement = 0; break;
      case R_X_STAGE3_0_CVC: result = W_FUIVLA3X_CVC; decrement = 0; break;
      case R_X_STAGE3_1: result = W_FUIVLA3X; decrement = 1; break;
      case R_X_STAGE3_1_CVC: result = W_FUIVLA3X_CVC; decrement = 1; break;
      case R_STAGE4_0: result = W_FUIVLA4; decrement = 0; break;
      case R_STAGE4_1: result = W_FUIVLA4; decrement = 1; break;
      case R_CMENE: result = W_CMENE; decrement = 0; break;
      case R_BAD_TOSMABRU: result = W_BAD_TOSMABRU; decrement = 1; break;
      case R_CULTURAL_BAD_TOSMABRU: result = W_CULTURAL_BAD_TOSMABRU; decrement = 1; break;
      case R_BAD_SLINKUI: result = W_BAD_SLINKUI; decrement = 0; break;
      case R_UNKNOWN:
      default:
        result = W_UNKNOWN; decrement = 0; break;
    }
    /*}}}*/
    /*{{{  Map to external word type / uppercase validity test */
    switch (result) {
      case W_CMAVOS_END_CY:
        /* Add start of trailing Cy cmavo to list of word start points */
        {
          char **x;
          x = cstart;
          while (x < pcstart) {
            *pstart++ = *x++;
          }
        }
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_CMAVOS;
        break;
      case W_CMAVOS:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_CMAVOS;
        break;
      case W_GISMU:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_GISMU;
        if (decrement) pstart--;
        break;
      case W_LUJVO:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_LUJVO;
        if (decrement) pstart--;
        break;
      case W_CULTURAL_LUJVO:
        ext_result = had_uppercase        ? MT_BAD_UPPERCASE :
                     allow_cultural_rafsi ? MT_LUJVO : MT_BOGUS ;
        if (decrement) pstart--;
        break;
      case W_FUIVLA3:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_FUIVLA3;
        if (decrement) pstart--;
        xtra.u.stage_3.hyph = hyph4;
        break;
      case W_FUIVLA3_CVC:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_FUIVLA3_CVC;
        if (decrement) pstart--;
        xtra.u.stage_3.hyph = hyph3;
        break;
      case W_FUIVLA3X:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_FUIVLA3X;
        if (decrement) pstart--;
        xtra.u.stage_3.hyph = hyph4;
        break;
      case W_FUIVLA3X_CVC:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_FUIVLA3X_CVC;
        if (decrement) pstart--;
        xtra.u.stage_3.hyph = hyph3;
        break;
      case W_FUIVLA4:
        ext_result = had_uppercase ? MT_BAD_UPPERCASE : MT_FUIVLA4;
        if (decrement) pstart--;
        break;
      case W_BAD_TOSMABRU:
      case W_CULTURAL_BAD_TOSMABRU:
      case W_BAD_SLINKUI:
        /* Don't care about uppercase/lowercase status */
        ext_result = MT_BOGUS;
        if (decrement) pstart--;
        break;
      case W_CMENE:
        /* Cmene are allowed to have uppercase letters in them. */
        ext_result = MT_CMENE;
        pstart = start+1;
        xtra.u.cmene.is_bad = is_bad_cmene(s, &xtra.u.cmene.can_split,
          &xtra.u.cmene.ladoi, &xtra.u.cmene.tail);
        break;
      default:
        ext_result = MT_BOGUS;
        break;
    }
    /*}}}*/
  }

#ifdef TEST_MORF
  if ((state < 0) || (morf_exitval[state] == 0)) {
    printf("%-25s : UNMATCHED : %s\n", s, s);
  } else {
    char *a;
    char **x;

    printf("%-25s : ", s);

    if (verbose) printf("[EV=%2d] ", exival);

    /*{{{  Print word type */
    switch (result) {
      case W_UNKNOWN:
        printf("Unrecognized");
        break;
      case W_CMAVOS:
        printf("cmavo(s)");
        break;
      case W_CMAVOS_END_CY:
        printf("cmavo(s)");
        break;
      case W_GISMU:
        printf("gismu");
        break;
      case W_LUJVO:
        printf("lujvo");
        break;
      case W_CULTURAL_LUJVO:
        printf("lujvo (with cultural rafsi)");
        break;
      case W_FUIVLA3:
        printf("fu'ivla (stage-3)");
        break;
      case W_FUIVLA3_CVC:
        printf("fu'ivla (stage-3 short rafsi)");
        break;
      case W_FUIVLA3X:
        printf("fu'ivla (multi-stage-3)");
        break;
      case W_FUIVLA3X_CVC:
        printf("fu'ivla (multi-stage-3, final short rafsi)");
        break;
      case W_FUIVLA4:
        printf("fu'ivla (stage-4)");
        break;
      case W_CMENE:
        if (xtra.u.cmene.is_bad && xtra.u.cmene.can_split) {
          printf("bad cmene (breaks up)");
        } else if (xtra.u.cmene.is_bad && !xtra.u.cmene.can_split) {
          printf("bad cmene (doesn't break up)");
        } else if (!xtra.u.cmene.is_bad) {
          printf("cmene");
        }
        split_cmene = xtra.u.cmene.is_bad && xtra.u.cmene.can_split;
        break;
      case W_BAD_TOSMABRU:
        printf("Bad lujvo (y hyphen not required)");
        break;
      case W_CULTURAL_BAD_TOSMABRU:
        printf("Bad lujvo (y hyphen not required, with cultural rafsi)");
        break;
      case W_BAD_SLINKUI:
        printf("Bad fu'ivla (fails slinku'i test)");
        break;
      case W_BIZARRE:
        printf("Internal program bug");
        break;
    }
    /*}}}*/
    /*{{{  Print invalid uppercase msg*/
    switch (result) {
      case W_UNKNOWN:
      case W_CMAVOS:
      case W_CMAVOS_END_CY:
      case W_GISMU:
      case W_LUJVO:
      case W_CULTURAL_LUJVO:
      case W_FUIVLA3:
      case W_FUIVLA3_CVC:
      case W_FUIVLA3X:
      case W_FUIVLA3X_CVC:
      case W_FUIVLA4:
      case W_BAD_TOSMABRU:
      case W_CULTURAL_BAD_TOSMABRU:
      case W_BAD_SLINKUI:
        if (had_uppercase) {
          printf(" (contains invalid uppercase)");
        }
        break;
      case W_CMENE:
      case W_BIZARRE:
        /* Nothing to do */
        break;
    }
    /*}}}*/

    putchar(' ');
    putchar(':');
    putchar(' ');

    /* {{{ Assert that attributes were picked up properly */
    switch (result) {
      case W_FUIVLA3:
      case W_FUIVLA3X:
        assert(hyph4);
        break;
      case W_FUIVLA3_CVC:
      case W_FUIVLA3X_CVC:
        assert(hyph3);
        break;
      default:
        break;
    }
    /*}}}*/

    /*{{{  Print original word with prefix cmavo split off */
    for (a=s, x=start; *a; a++) {

      /* Print spaces to separate prefix cmavo */
      if (x && (a == *x)) {
        x++;
        putchar(' ');
        if (x == pstart) x = NULL;
      }
      
      /*{{{  Insert pre-char separators */
      switch (result) {
        case W_FUIVLA3:
        case W_FUIVLA3X:
          if (a == hyph4) putchar('/');
          break;
        case W_FUIVLA3_CVC:
        case W_FUIVLA3X_CVC:
          if (a == hyph3) putchar('/');
          break;
        case W_CMENE:
          if (split_cmene) {
            /* Show divisions between parts of the split, but omit the first
             * marker if la or doi occurs at the very start of the word. */
            if (((a == xtra.u.cmene.ladoi) && (a != s)) ||
                (a == xtra.u.cmene.tail)) {
              putchar ('+');
            }
          }
          break;
        default:
          break;
      }
      /*}}}*/

      /* Emit actual character */
      putchar(*a);
      
      /*{{{  Insert post-char separators */
      switch (result) {
        case W_FUIVLA3:
        case W_FUIVLA3X:
          if (a == hyph4) putchar('/');
          break;
        case W_FUIVLA3_CVC:
        case W_FUIVLA3X_CVC:
          if (a == hyph3) putchar('/');
          break;
        default:
          break;
      }
      /*}}}*/
    }
    /*}}}*/

    /*{{{ Show expansion of lujvo */
    switch(result) {
      case W_LUJVO:
        if (expand_lujvo) {
          char *canon;
          canon = canon_lujvo(pstart[-1]);
          printf(" [%s]", canon);
        }
        break;

      default:
        break;
    }
    /*}}}*/
    
    putchar('\n');
    
  }
#endif

  *buf_end = pstart - 1;
  /* Allow arg_xtra to be NULL, as the data isn't always needed */
  if (arg_xtra) *arg_xtra = xtra;
  return ext_result;
}
/*}}}*/

#ifdef TEST_MORF
int main (int argc, char **argv) {/*{{{*/
  char buffer[128];
  char *start[256], **pstart;
  char *word = NULL;
  while (++argv, --argc) {
    if (!strncmp(*argv, "-v", 2)) {
      verbose = 1;
    } else if (!strcmp(*argv, "-el")) {
      expand_lujvo = 1;
    } else if (!strncmp(*argv, "-", 1)) {
      fprintf(stderr, "Unrecognized command line argument '%s'\n", *argv);
    } else {
      word = *argv;
    }
  }
  if (word) {
    pstart = start;
    morf_scan(word, &pstart, NULL);
  } else {
    while (fgets(buffer, sizeof(buffer), stdin)) {
      buffer[strlen(buffer)-1] = 0;
      if (buffer[0] == '#') continue; /* Allow comment lines in test source file */
      pstart = start;
      morf_scan(buffer, &pstart, NULL);
    }
  }
  return 0;
}/*}}}*/
#endif


