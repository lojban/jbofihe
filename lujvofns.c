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

unsigned char attr_table[256] = {
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
};

/*++++++++++++++++++++++++++++++++++++++
  

  int is_consonant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_consonant(char c) {
  return !!(attr_table[c] & CONSONANT);
}

/*++++++++++++++++++++++++++++++++++++++
  Check whether a character is a consonant.

  int is_consonant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_uppercase_consonant(char c) {
  int m = CONSONANT | UPPERCASE;
  return ((attr_table[c] & m) == m);
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_vowel

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_vowel(char c) {
  return !!(attr_table[c] & VOWEL);
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_voiced

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_voiced(char c) {
  return !!(attr_table[c] & VOICED);
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_unvoiced

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_unvoiced(char c) {
  return !!(attr_table[c] & UNVOICED);
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_sibilant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_sibilant(char c) {
  return !!(attr_table[c] & SIBILANT);
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_ccv

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_ccv(char *s) {
  if (is_consonant(s[0]) &&
      is_consonant(s[1]) &&
      is_vowel(s[2])) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_cvv

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_cvv(char *s) {
  if (is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      (is_vowel(s[2]) ||
       (is_vowel(s[3]) && s[2] == '\''))) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_cvc

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_cvc(char *s) {
  if (is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_consonant(s[2])) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_cvccv

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_cvccv(char *s) {
  if (is_consonant(s[0]) &&
      is_vowel(s[1]) &&
      is_consonant(s[2]) &&
      is_consonant(s[3]) &&
      is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_ccvcv

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_ccvcv(char *s) {
  if (is_consonant(s[0]) &&
      is_consonant(s[1]) &&
      is_vowel(s[2]) &&
      is_consonant(s[3]) &&
      is_vowel(s[4])) {
    return 1;
  } else {
    return 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Check whether a consonant pair is permissible

  int is_pairok

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_pairok(char *s) {
  int test1, test2, test3, test4; /* all true if acceptable */

  test1 = (s[0] != s[1]);
  test2 = !((is_voiced(s[0]) && is_unvoiced(s[1])) ||
            (is_voiced(s[1]) && is_unvoiced(s[0])));
  test3 = !(is_sibilant(s[0]) && is_sibilant(s[1]));
  test4 = strncmp(s, "cx", 2) && strncmp(s, "kx", 2) && 
    strncmp(s, "xc", 2) && strncmp(s, "xk", 2) && strncmp(s, "mz", 2);

  return test1 && test2 && test3 && test4;

}

/*++++++++++++++++++++++++++++++++++++++
  Return 1 if an initial consonant pair is acceptable for a lujvo, otherwise return 0.

  int is_initial_pair_ok

  char *s The string whose first 2 chars are to be tested.
  ++++++++++++++++++++++++++++++++++++++*/

int
is_initialpairok(char *s) {

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


/*++++++++++++++++++++++++++++++++++++++
  Return 1 if the string t has the correct form to be a lujvo (note,
  there is no dictionary lookup of the apparent constituent rafsi)

  int is_valid_lujvo

  char *t
  ++++++++++++++++++++++++++++++++++++++*/

int
is_valid_lujvo(char *t) {
  char *p;
  int debug = 0;

  if (debug) printf("Testing [%s] as valid lujvo\n", t);
  if (strlen(t) < 5) {
    if (debug) printf("Initial length too short, invalid\n");
    return 0;
  }
  while (*t) {
    if (debug) printf("Residual [%s]\n", t);
    p = strchr(t, 'y');
    if ((p-t) == 4) {
      /* Starts with abcdy, have to check whether abcd could be a valid rafsi */
      if (is_consonant(t[0]) && is_consonant(t[1]) && is_vowel(t[2]) && is_consonant(t[3]) && is_initialpairok(t)) {
        if (debug) printf("4 letter rafsi CCVC at start valid then y\n");
        t+=5;
      } else if (is_consonant(t[0]) && is_vowel(t[1]) && is_consonant(t[2]) && is_consonant(t[3]) && is_pairok(t+2)) {
        if (debug) printf("4 letter rafsi CVCC at start valid then y\n");
        t+=5;
      } else {
        if (debug) printf("4 letter rafsi at start invalid\n");
        return 0;
      }
    } else if ((p-t) == 3) {
      /* Starts with 3 letter rafsi then a join */
      if (is_ccv(t)) {
        if (is_initialpairok(t)) {
          /* Perhaps ought to check whether rafsi form is in dictionary? */
          if (debug) printf("3 letter rafsi CCV at start valid then y\n");
          return 1;
        } else {
          if (debug) printf("3 letter rafsi CCV at start invalid then y\n");
          return 0;
        }
      } else if (is_cvc(t)) {
        /* Dictionary test? */
        if (debug) printf("3 letter rafsi CVC at start valid then y\n");
        return 1;
      } else if (is_cvv(t)) {
        /* Not possible */
        fprintf(stderr, "Can't have y after CVV form rafsi\n");
        exit(1);
      }
    } else if ((p-t) < 3) {
      if (debug) printf("<3 letters left, invalid\n");
      return 0;
    } else if ((strlen(t) > 5) || (strlen(t) == 3)) {
      /* Strip leading rafsi if valid */
      if (is_ccv(t)) {
        if (is_initialpairok(t)) {
          if (debug) printf("Initial CCV, examine tail\n");
          t+=3;
          /* Go round loop again */
        } else {
          if (debug) printf("Initial invalid CCV\n");
          return 0;
        }
      } else if (is_cvc(t)) {
        if (debug) printf("Initial CVC, examine tail\n");
        t+=3;
        /* Go round again */
      } else if (is_cvv(t)) {
        if (debug) printf("Initial CVV, examine tail\n");
        if (t[2] == '\'') {
          t+=4;
        } else {
          t+=3;
        }
      } else {
        /* Not valid lujvo */
        if (debug) printf("Invalid, prefix not any rafsi form\n");
        return 0;
      }
    } else if (strlen(t) == 5) {
      /* Just a gismu left, assume OK if correct form */
      if (is_cvccv(t) || is_ccvcv(t)) { 
        if (debug) printf("Matches gismu form\n");
        return 1;
      } else {
        if (debug) printf("Unmatched 5 character form\n");
        return 0;
      }
    } else {
      if (debug) printf("Unrecognized length\n");
      return 0;
    }
  }
  /* If we fall out of the loop, all rafsi have been checked off. */
  return 1;
}


#ifdef TEST
int main() {
  int c;
  for (c='a'; c<='z'; c++) {
    printf("%c c=%d uc=%d v=%d sib=%d voi=%d unv=%d\n",
            c, is_consonant(c), is_uppercase_consonant(c), is_vowel(c), is_sibilant(c), is_voiced(c), is_unvoiced(c));
  }
  return 0;
}
#endif

