/***************************************
  $Header$

  Translation functions.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "canonluj.h"

/* ================================================== */

typedef struct {
  char *rafsi;
  char *gismu; /* or cmavo */
} R2G;

static R2G r2g[] = {
#include "canonluj.inc"
};

#define Number(x) ((sizeof(x))/sizeof(x[0]))

/* ================================================== */

static int
is_consonant(char c)
{
  return (strchr("bcdfgjklmnprstvxz", c) != 0);
}

static int
IC(char c) {
  return is_consonant(c);
}

/* ================================================== */

static int
is_vowel(char c)
{
  return (strchr("aeiou", c) != 0);
}

static int
IV(char c) {
  return is_vowel(c);
}


/* ================================================== */

static int
contains_compound_consonant(char *x)
{
  char *p, *q, *s;

  if (strlen(x) < 2) {
    return 0;
  }

  p = x;
  do {
    q = p + 1;
    s = q + 1;
    if (is_consonant(*p)) {
      if (is_consonant(*q)) return 1;
      if ((*q == 'y') && (is_consonant(*s))) return 1;
    }
    p++;
  } while (*p);
  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Comparison function for bsearch on the rafsi->gismu list.

  static int compare_r2g
  ++++++++++++++++++++++++++++++++++++++*/

static int
compare_r2g(const void *a, const void *b)
{
  const R2G *aa = (const R2G *) a;
  const R2G *bb = (const R2G *) b;
  return strcmp(aa->rafsi, bb->rafsi);
}

/*++++++++++++++++++++++++++++++++++++++
  Look up a single rafsi and return the corresponding gismu/cmavo.

  static char * do_component

  char *x
  ++++++++++++++++++++++++++++++++++++++*/

static char *
lookup_component(char *x)
{
  R2G specimen;
  R2G *match;
  specimen.rafsi = x;
  match = (R2G *) bsearch(&specimen, r2g, Number(r2g), sizeof(R2G), compare_r2g);
  if (!match) {
    return "?";
  } else {
    return match->gismu;
  }
}

/* ================================================== */

static void
strip_leading_rafsi(char **y, int len, int hy, int first, char *result)
{
  char buf[5], *comp;
  char *x = *y;
  int i;
  for (i=0; i<len; i++) buf[i] = x[i];
  buf[len] = 0;
  comp = lookup_component(buf);
  if (!first) strcat(result, "+");
  strcat(result, comp ? comp : "?");
  *y = x + len + hy;
}

/* ================================================== */

/* ================================================== */

static char *
split_lujvo (char *x)
{
  static char result[2048];
  int first=1; /* At start, so we can handle r and n hyphens */
  
  result[0] = 0;

  while (1) {
    /* Advance x to look at the tail of the string. */
    int len = strlen(x);

    switch (len) {
      case 0:
      case 1:
      case 2:
        /* Impossible */
        return NULL;
        break;

      case 3:
      case 4:
        {
          char *comp = lookup_component(x);
          if (comp) {
            if (!first) strcat(result, "+");
            strcat(result,comp);
          }            
          return result;
        }
        break;

      case 5:
        if (!first) strcat(result, "+");
        strcat(result, x);
        return result;
        break;

      default:
        {
          if (x[3] == 'y') {
            strip_leading_rafsi(&x, 3, 1, first, result);
          } else if (x[4] == 'y') {
            strip_leading_rafsi(&x, 4, 1, first, result);
          } else {
            /* Have to decide if there's an r/n hyphen */
            if (!is_consonant(x[0])) return NULL; /* can't ever happen */
            if (IC(x[1]) && IV(x[2])) { /* CCV at start */
              strip_leading_rafsi(&x, 3, 0, first, result);
            } else if (IV(x[1]) && IC(x[2])) {
              strip_leading_rafsi(&x, 3, 0, first, result);
            } else if (IV(x[1])) {
              if (x[2] == '\'') {
                /* Everything pushed up by 1 */
                if (IV(x[3])) {
                  if (((x[4] == 'r') && (IC(x[5]))) ||
                       ((x[4] == 'n') && (x[5] == 'r'))) {
                    /* This pattern can only occur if a hyphen is there. */
                    strip_leading_rafsi(&x, 4, 1, first, result);
                  } else {
                    strip_leading_rafsi(&x, 4, 0, first, result);
                  }
                } else {
                  /* CV'C form, impossible. */
                  return NULL; 
                }
              } else {
                if (IV(x[2])) {
                  if (((x[3] == 'r') && (IC(x[4]))) ||
                       ((x[3] == 'n') && (x[4] == 'r'))) {
                    /* This pattern can only occur if a hyphen is there. */
                    strip_leading_rafsi(&x, 3, 1, first, result);
                  } else {
                    strip_leading_rafsi(&x, 3, 0, first, result);
                  }
                } else {
                  /* CVC form, no r/n hyphen */
                  strip_leading_rafsi(&x, 3, 0, first, result);
                }
              }
            } else {
              /* Impossible */
              return NULL;
            }
          }

        }

        break;
    }

    first = 0;
  }


}



/* ================================================== */


/*++++++++++++++++++++++++++++++++++++++

  Take a lujvo which may contains some 3 letter rafsi etc.  Break it
  up into constituent gismu and cmavo.  Be lenient about trailing CVC
  forms etc - want to use this on the results of stripping -mau, -me'a
  etc off things.

  char * canon_lujvo Return components separated by + signs.

  char *x Input lujvo
  ++++++++++++++++++++++++++++++++++++++*/

char *
canon_lujvo (char *x)
{
  if (strchr(x, '+')) {
    /* If it looks like the strings has already been through here, be
       an identity function.  Used in translator to cope with
       recursive pattern matches. */
    return x;
  } else if (contains_compound_consonant(x)) {
    return split_lujvo(x);
  } else {
    return NULL;
#if 0
    return split_compound_cmavo(x);
#endif
  }
  
}

/* ================================================== */

#ifdef TEST

int main (int argc, char **argv) {
  char line[128], *result;
  while (fgets(line, sizeof(line), stdin)) {
    line[strlen(line)-1] = 0; /* strip NL */
    result = canon_lujvo(line);
    printf("%s -> %s\n", line, result ? result : "?");
  }
}

#endif

/* ================================================== */

/* ================================================== */

/* ================================================== */


/* ================================================== */
