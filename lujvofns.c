/***************************************
  $Header$

  Functions to do with manipulating lujvo etc.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <string.h>

#include "lujvofns.h"


/*++++++++++++++++++++++++++++++++++++++
  

  int is_consonant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_consonant(char c) {
  if (strchr("bcdfgjklmnprstvxz", c)) {
    return 1;
  } else {
    return 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Check whether a character is a consonant.

  int is_consonant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_uppercase_consonant(char c) {
  if (strchr("BCDFGJKLMNPRSTVXZ", c)) {
    return 1;
  } else {
    return 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  

  int is_vowel

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_vowel(char c) {
  if (strchr("aeiou", c)) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_voiced

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_voiced(char c) {
  if (strchr("bdgvjz", c)) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_unvoiced

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_unvoiced(char c) {
  if (strchr("ptkfcsx", c)) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_sibilant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

int
is_sibilant(char c) {
  if (strchr("cjsz", c)) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  

  int is_ccv

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

int
is_ccv(char *s) {
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_consonant(s[1]) &&
      s[2] && is_vowel(s[2])) {
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
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      ((s[2] && is_vowel(s[2])) ||
       (s[3] && is_vowel(s[3]) && s[2] == '\''))) {
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
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      s[2] && is_consonant(s[2])) {
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
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_vowel(s[1]) &&
      s[2] && is_consonant(s[2]) &&
      s[3] && is_consonant(s[3]) &&
      s[4] && is_vowel(s[4])) {
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
  if (s[0] && is_consonant(s[0]) &&
      s[1] && is_consonant(s[1]) &&
      s[2] && is_vowel(s[2]) &&
      s[3] && is_consonant(s[3]) &&
      s[4] && is_vowel(s[4])) {
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
      } else if (is_consonant(t[0]) && is_vowel(t[1]) && is_consonant(t[2]) && is_consonant(t[3]) && is_pairok(t)) {
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


