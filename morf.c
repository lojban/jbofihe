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
static int verbose=0;
#endif

#include "morf.h"

/*
  Define exit values used in the NFA/DFA.  These are separate bits to allow
  them to be or'd together.  The _0 versions are the cases where the consonant
  cluster occurs at the start, the _1 versions are those where it occurs a bit
  later (i.e. the list of prefix cmavo has to be backed up by 1 to avoid
  counting the start of the brivla as a prefix cmavo.)
   */
#define BIT_CMAVOS        0x0001
#define BIT_CMAVOS_END_CY 0x0002
#define BIT_GISMU_0       0x0004
#define BIT_GISMU_1       0x0008
#define BIT_LUJVO_0       0x0010
#define BIT_LUJVO_1       0x0020
#define BIT_LUJVO_1T      0x0040
#define BIT_LUJVO_TAIL_OK 0x0080
#define BIT_FUIVLA_0      0x0100
#define BIT_FUIVLA_1      0x0200
#define BIT_SLINKUI_0     0x0400
#define BIT_SLINKUI_1     0x0800
#define BIT_CMENE         0x1000

/* Define the values returned by priority coding the bit patterns */
#define W_UNKNOWN         0
#define W_CMAVOS          1
#define W_CMAVOS_END_CY   2
#define W_GISMU           3
#define W_LUJVO           4
#define W_FUIVLA          5
#define W_CMENE           6
#define W_BAD_TOSMABRU    8
#define W_BAD_SLINKUI     9
#define W_BIZARRE        10

/* Include table for turning the letter stream into meta-classes (consonant,
 * vowel, permissible pair etc).  These 'meta-classes' are the tokens used by
 * the DFA.  These tables are built my mk_fetab.pl */
#include "morf_lex.c"

/* Include the main DFA scanning tables built by nfa2dfa.pl. */
#include "morf_dfa.c"

/* Include the tables used to encode the or'd together bit patterns into a
 * single output decision as to the type of word.  These tables are built by
 * mk_enctab.pl */
#include "morf_enc.c"

/* Map N->1, R->2, other C->3, else ->0.  Used to trim down the last-but-one
 * letter, which is saved to allow the front-end to spot illegal triples and
 * type III fu'ivla hyphen patterns. */

static unsigned char s2l[32] = {
  0, 0, 3, 3, 3, 0, 3, 3,
  0, 0, 3, 3, 3, 3, 1, 0,
  3, 0, 2, 3, 3, 0, 3, 0,
  3, 0, 3, 0, 0, 0, 0, 0
};

/* Token names for -v mode */
static unsigned char *toknam[] = {
  "UNK", "V", "APOS", "Y", "R", "N", "C",
  "NR", "CI", "CP", "CN", "H", "BT"
};

/* Front end state machine actions, printable for -v mode */
static unsigned char *actnam[] = {
  "CLR", "SFT", "FRZ"
};

/* Map the ASCII set to the range 0..31 (mostly
   by masking high order bits off the letters,
   except the apostrophe is given the value 30) */

static unsigned char mapchar[256] = {
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

  0x1f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

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


/* The main scanning routine.  's' is the string to be scanned.  buf_end is a
 * pointer to a table of pointers to characters (i.e. pass by reference so we
 * can pass a result back.)  This table is filled in with the positions in 's'
 * where the prefix cmavo start. */
MorfType
morf_scan(char *s, char ***buf_end)
{
  unsigned int L, S, G;
  char *p, c;
  char **start, **pstart;
  char *cstart[256], **pcstart; /* Used for holding start of Cy structures */
  int ent, tent, tok, set, inhibit;
  int state, next_state;
  int inhibited = 0, initial = 1;
  int exival;
  int decrement = 0;
  int result;
  MorfType ext_result;

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
    printf ("Chr L  S   G  tok    act set inh state'\n");
  }
#endif

  p = s;
  while (*p) {
    c = *p;
    if (c == ',') continue;
    G = (unsigned int) mapchar[c];

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

    if ((last_act == ACT_FREEZE) && (act == ACT_SHIFT)) {
      /* This happens if the input has a y-to-consonant transition.  Such
       * positions may be the start of Cy cmavo, if the whole string ends in a
       * sequence of Cy cmavo. */
      *pcstart++ = p;
    }

    p++;
    initial = 0;
    next_state = NEXT_STATE(state, tok);

#ifdef TEST_MORF
    if (verbose) {
      printf ("%c   %01x  %02x  %02x  %-4s  %-3s  %d   %d  %4d\n",
              c, L, S, G, toknam[tok], actnam[act], set, inhibit, next_state);
    }
#endif

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

    state = next_state;

    if (state < 0) break; /* syntax error */
    
  }
  
  if ((state < 0) || (morf_exitval[state] == 0)) {
    result = W_UNKNOWN;
    decrement = 0;
  } else {
    exival = morf_exitval[state];
    result = morf_enctab1[exival & 0xff];
    decrement = result>>7;
    result &= 0x7f;
    if (!result) {
      result = morf_enctab2[(exival >> 8) & 0x1f];
      decrement = result>>7;
      result &= 0x7f;
    }
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
        ext_result = MT_CMAVOS;
        break;
      case W_CMAVOS:
        ext_result = MT_CMAVOS;
        break;
      case W_GISMU:
      case W_LUJVO:
      case W_FUIVLA:
        ext_result = MT_BRIVLA;
        if (decrement) pstart--;
        break;
      case W_BAD_TOSMABRU:
      case W_BAD_SLINKUI:
        ext_result = MT_BOGUS;
        if (decrement) pstart--;
        break;
      case W_CMENE:
        ext_result = MT_CMENE;
        pstart = start+1;
        break;
      default:
        ext_result = MT_BOGUS;
        break;
    }
  }

#ifdef TEST_MORF
  if ((state < 0) || (morf_exitval[state] == 0)) {
    printf("%-25s : UNMATCHED!\n", s);
  } else {
    char *a;
    char **x;

    printf("%-25s : ", s);
    printf("[EV=%08lx] ", exival);


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
      case W_FUIVLA:
        printf("fu'ivla");
        break;
      case W_CMENE:
        printf("cmene");
        break;
      case W_BAD_TOSMABRU:
        printf("Bad lujvo (y hyphen not required)");
        break;
      case W_BAD_SLINKUI:
        printf("Bad fu'ivla (fails slinku'i test)");
        break;
      case W_BIZARRE:
        printf("Internal program bug");
        break;
    }

    putchar(' ');
    putchar(' ');

    for (a=s, x=start; *a; a++) {
      if (x && (a == *x)) {
        x++;
        putchar(' ');
        if (x == pstart) x = NULL;
      }
      putchar(*a);
    }
    putchar('\n');
    
  }
#endif

  *buf_end = pstart - 1;
  return ext_result;
}

#ifdef TEST_MORF
int main (int argc, char **argv) {
  char buffer[128];
  char *start[256], **pstart;
  while (++argv, --argc) {
    if (!strncmp(*argv, "-v", 2)) {
      verbose = 1;
    }
  }
  while (fgets(buffer, sizeof(buffer), stdin)) {
    buffer[strlen(buffer)-1] = 0;
    if (buffer[0] == '#') continue; /* Allow comment lines in test source file */
    pstart = start;
    morf_scan(buffer, &pstart);
  }
  return 0;
}
#endif


