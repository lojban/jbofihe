/***************************************
  $Header$

  Functions to do with manipulating lujvo etc.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <string.h>

#include "lujvofns.h"

#define CONSONANT 0001
#define VOWEL     0002
#define LETTER_Y  0004
#define APOS      0010
#define SIBILANT  0020
#define VOICED    0040
#define UNVOICED  0100
#define UPPERCASE 0200

unsigned char attr_table[256] = {/*{{{*/
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 000 - 007 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 010 - 017 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 020 - 027 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 030 - 037 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0010, /* 040 - 047 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 050 - 057 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 060 - 067 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 070 - 077 */

  0000, 0202, 0241, 0321, 0241, 0202, 0301, 0241, /* 100 - 107 */
  0200, 0202, 0261, 0301, 0201, 0201, 0201, 0202, /* 110 - 117 */
  0301, 0200, 0201, 0321, 0301, 0202, 0241, 0200, /* 120 - 127 */
  0301, 0204, 0261, 0000, 0000, 0000, 0000, 0000, /* 130 - 137 */
  0000, 0002, 0041, 0121, 0041, 0002, 0101, 0041, /* 140 - 147 */
  0000, 0002, 0061, 0101, 0001, 0001, 0001, 0002, /* 150 - 157 */
  0101, 0000, 0001, 0121, 0101, 0002, 0041, 0000, /* 160 - 167 */
  0101, 0004, 0061, 0000, 0000, 0000, 0000, 0000, /* 170 - 177 */

  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 200 - 207 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 210 - 217 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 220 - 227 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 230 - 237 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 240 - 247 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 250 - 257 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 260 - 267 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 270 - 277 */

  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 300 - 307 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 310 - 317 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 320 - 327 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 330 - 337 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 340 - 347 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 350 - 357 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, /* 360 - 367 */
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000  /* 370 - 377 */
};/*}}}*/

int is_consonant(char c) {/*{{{*/
  return !!(attr_table[(unsigned)c & 0xff] & CONSONANT);
}
/*}}}*/
int is_uppercase_consonant(char c) {/*{{{*/
  int m = CONSONANT | UPPERCASE;
  return ((attr_table[(unsigned)c & 0xff] & m) == m);
}
/*}}}*/
int is_vowel(char c) {/*{{{*/
  return !!(attr_table[(unsigned)c & 0xff] & VOWEL);
}
/*}}}*/
int is_voiced(char c) {/*{{{*/
  return !!(attr_table[(unsigned)c & 0xff] & VOICED);
}
/*}}}*/
int is_unvoiced(char c) {/*{{{*/
  return !!(attr_table[(unsigned)c & 0xff] & UNVOICED);
}
/*}}}*/
int is_sibilant(char c) {/*{{{*/
  return !!(attr_table[(unsigned)c & 0xff] & SIBILANT);
}
/*}}}*/
int is_ccv(char *s) {/*{{{*/
  if ((strlen(s) >= 3) &&
      is_consonant(s[0]) &&
      is_consonant(s[1]) &&
      is_vowel(s[2])) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/
int is_cvv(char *s) {/*{{{*/
  int len = strlen(s);
  if ((len >= 3) && 
      is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_vowel(s[2])) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/
int is_cvav(char *s) {/*{{{*/
  int len = strlen(s);
  if ((len >= 4) && 
      is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      (s[2] == '\'') &&
      is_vowel(s[3])) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/
int is_cvc(char *s) {/*{{{*/
  if ((strlen(s) >= 3) &&
      is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_consonant(s[2])) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/
int is_cvccv(char *s) {/*{{{*/
  if ((strlen(s) >= 5) &&
      is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_consonant(s[2]) &&
      is_consonant(s[3]) &&
      is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}/*}}}*/
int is_cvccy(char *s) {/*{{{*/
  if ((strlen(s) >= 5) &&
      is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_consonant(s[2]) &&
      is_consonant(s[3]) &&
      (s[4] == 'y')) {
    return 1;
  } else {
    return 0;
  }
}/*}}}*/
int is_ccvcv(char *s) {/*{{{*/
  if ((strlen(s) >= 5) &&
      is_consonant(s[0]) &&
      is_consonant(s[1]) &&
      is_vowel(s[2]) &&
      is_consonant(s[3]) &&
      is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}/*}}}*/
int is_ccvcy(char *s) {/*{{{*/
  if ((strlen(s) >= 5) &&
      is_consonant(s[0]) &&
      is_consonant(s[1]) &&
      is_vowel(s[2]) &&
      is_consonant(s[3]) &&
      (s[4] == 'y')) {
    return 1;
  } else {
    return 0;
  }
}/*}}}*/
int is_pairok(char *s) {/*{{{*/
  /* Check whether a consonant pair is permissible */
  int test1, test2, test3, test4; /* all true if acceptable */

  test1 = (s[0] != s[1]);
  test2 = !((is_voiced(s[0]) && is_unvoiced(s[1])) ||
            (is_voiced(s[1]) && is_unvoiced(s[0])));
  test3 = !(is_sibilant(s[0]) && is_sibilant(s[1]));
  test4 = strncmp(s, "cx", 2) && strncmp(s, "kx", 2) && 
    strncmp(s, "xc", 2) && strncmp(s, "xk", 2) && strncmp(s, "mz", 2);

  return test1 && test2 && test3 && test4;

}
/*}}}*/
int is_initialpairok(char *s) {/*{{{*/
  /* Return 1 if an initial consonant pair is acceptable for a lujvo, otherwise
     return 0. */

  switch (s[0]) {
    case 'b':
    case 'f':
    case 'g':
    case 'k':
    case 'm':
    case 'p':
    case 'v':
    case 'x':
      return !!strchr("lr", s[1]);
    case 'c':
    case 's':
      return !!strchr("fklmnprt", s[1]);
    case 'd':
      return !!strchr("jrz", s[1]);
    case 'j':
    case 'z':
      return !!strchr("bdgmv", s[1]);
    case 't':
      return !!strchr("crs", s[1]);
    default:
      return 0;
  }
}
/*}}}*/
int is_bad_triple (char *s) {/*{{{*/
/* Check whether a triple is bad */
  if (!strcmp(s,"ntc") || !strcmp(s,"nts") || !strcmp(s,"ndj") || !strcmp(s,"ndz"))
    return 1;
  else
    return 0;
}
/*}}}*/

/* ========================================*/

#define ADVANCE(n) t+=(n), len-=(n)

/* is_valid_lujvo(), used by the jvocu'adju program to do tosmabru checks. */

int
is_valid_lujvo(char *t)
{
  int len = strlen(t);
  char buf[1024];

  /* Flags to check CVV + hyphen stuff. */
  int start_cvv;
  int had_rn_hyphen;
  int final_ccv;

  /* Number of components seen */
  int nrafsi;
  
  start_cvv = 0;
  had_rn_hyphen = 0;
  nrafsi = 0;

  for (;;) {
    if ((len == 5) && is_cvccv(t)) {/*{{{*/
      if (nrafsi==0) return 0; /* Can't be initial */
      if (!is_pairok(t+2)) return 0;
      ADVANCE(5);
      /*}}}*/
    } else if ((len == 5) && is_ccvcv(t)) {/*{{{*/
      if (nrafsi==0) return 0; /* Can't be initial */
      if (!is_pairok(t)) return 0;
      ADVANCE(5);
      /*}}}*/
    } else if (is_cvccy(t)) {/*{{{*/
      if (len==5) return 0; /* Can't be final */
      if (!is_pairok(t+2)) return 0;
      ADVANCE(5);
      /*}}}*/
    } else if (is_ccvcy(t)) {/*{{{*/
      if (len==5) return 0; /* Can't be final */
      if (!is_initialpairok(t)) return 0;
      ADVANCE(5);
/*}}}*/
    } else if (is_cvc(t)) {/*{{{*/
      int pair_ok, bad_triple, smabru, initial, need_y;
      if (len<6) return 0; /* Can't be final, must be at least 3 letters after
                              now */
      if (t[3] == 'y') {
        buf[0] = t[2];
        strcpy(buf+1, t+4);
        pair_ok = is_pairok(buf);
        bad_triple = is_bad_triple(buf);
        initial = (nrafsi == 0);
        smabru = initial ? is_valid_lujvo(buf) : 0;
        need_y = bad_triple || (initial && smabru);

        /* Check whether the y is unnecessary */
        if (pair_ok && !need_y) return 0;
            
        ADVANCE(4);

      } else {
        
        pair_ok = is_pairok(t+2);
        bad_triple = is_bad_triple(t+2);
        initial = (nrafsi == 0);
        smabru = initial ? is_valid_lujvo(t+2) : 0;
        need_y = bad_triple || (initial && smabru);

        /* Check whether there should be a y */
        if (!pair_ok || need_y) return 0;
            
        ADVANCE(3);
      }
      /*}}}*/
    } else if (is_cvv(t)) {/*{{{*/
      if (nrafsi == 0) {
        if (len < 6) return 0;
        /* Strip hyphen */
        start_cvv = 1;
        if (((t[3] == 'r') && (is_consonant(t[4]) && (t[4] != 'r'))) ||
            ((t[3] == 'n') && (t[4] == 'r'))) {
          had_rn_hyphen = 1;         
          ADVANCE(4);
        } else {
          ADVANCE(3);
        }
      } else {
        ADVANCE(3);
      }
      /*}}}*/
    } else if (is_cvav(t)) {/*{{{*/
      if (nrafsi == 0) {
        if (len < 7) return 0;
        /* Strip hyphen */
        start_cvv = 1;
        if (((t[4] == 'r') && (is_consonant(t[5]) && (t[5] != 'r'))) ||
            ((t[4] == 'n') && (t[5] == 'r'))) {
          had_rn_hyphen = 1;         
          ADVANCE(5);
        } else {
          ADVANCE(4);
        }
      } else {
        ADVANCE(4);
      }
      /*}}}*/
    } else if (is_ccv(t)) {/*{{{*/
      if (!is_initialpairok(t)) return 0;
 
      if (len == 3) final_ccv = 1;
      ADVANCE(3);
/*}}}*/
    } else {/*{{{*/
      /* Anything else is invalid */
      return 0;
/*}}}*/
    }

    nrafsi++;
    if (!*t) break; /* At end of word */
  }

  /* Final processing to check validity */
  if (start_cvv) {
    if (((nrafsi == 2) && final_ccv) || had_rn_hyphen)
      return 1;
    else
      return 0;
  } else {
    return 1;
  }
}

#ifdef TEST_PRIMITIVES
int main() {
  int c;
  for (c='a'; c<='z'; c++) {
    printf("%c c=%d uc=%d v=%d sib=%d voi=%d unv=%d\n",
            c, is_consonant(c), is_uppercase_consonant(c), is_vowel(c), is_sibilant(c), is_voiced(c), is_unvoiced(c));
  }
  return 0;
}
#endif

#ifdef TEST_IS_VALID_LUJVO
int main(int argc, char **argv) {
  int ivl;
  
  if (argc < 2) {
    fprintf(stderr, "Need a lujvo to check as an argument\n");
    exit(1);
  }

  ivl = is_valid_lujvo(argv[1]);

  printf("%s : %s\n", argv[1], ivl ? "is a lujvo" : "not a lujvo");

  return !ivl;
}

#endif

