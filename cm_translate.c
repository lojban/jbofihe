/***************************************
  $Header$

  Translation routine for mini-translater.

  ***************************************/

/* COPYRIGHT */

#include "cm.h"
#include "dictaccs.h"

#ifndef DEFAULT_DICTIONARY
#define DEFAULT_DICTIONARY "smujmaji.dat"
#endif

#include <unistd.h>
#include <sys/stat.h>

/* ================================================== */

char *
translate(char *word)
{
  static char buf[1024];
  char *res;

  res = dict_lookup(word);
  if (res) {
    strcpy(buf, res);
    return buf;
  } else {
    return NULL;
  }

}

/* ================================================== */

static void
append_trans(char *trans, char *result, int initial)
{
  if (trans) {
    if (initial) {
      strcpy(result, trans);
    } else {
      strcat(result, "-");
      strcat(result, trans);
    }
  } else {
    if (initial) {
      strcpy(result, "?");
    } else {
      strcat(result, "-?");
    }
  }
}


/* ================================================== */

static char *
translate_comp(char *w, int *conversion)
{
  char buf[64], buf2[4];
  static char result[256];
  char *trans;
  strcpy(buf, w);
  if (*conversion) {
    buf2[0] = '0' + *conversion;
    buf2[1] = 0;
    strcat(buf, buf2);
    trans = translate(buf);
    if (!trans) {
      result[0] = 0;
      switch (*conversion) {
        case 2:
          strcpy(result, "2nd conv-");
          break;
        case 3:
          strcpy(result, "3rd conv-");
          break;
        case 4:
          strcpy(result, "4th conv-");
          break;
        case 5:
          strcpy(result, "5th conv-");
          break;
      }
      trans = translate(w);
      if (trans) {
        strcat (result, trans);
      } else {
        strcat (result, "??");
      }
      return result;
    }
  } else {
    trans = translate(w);
  }
  *conversion = 0;
  return trans;
}

/* ================================================== */

/* Lookup a lujvo that isn't matched in the ordinary dictionary, by
   smashing it into consituent rafsi and glueing these together. */
static char *
translate_lujvo(char *word)
{
  static char result[4096];
  char buf[64];
  char *w, *trans;
  int initial;
  char *yy;
  int ypos=0;
  int conversion=0;

  w = word;
  result[0] = 0;
  initial = 1;

  do {
    if (strlen(w) == 5) {
      /* trailing gismu */
      trans = translate_comp(w, &conversion);
      append_trans(trans, result, initial);
      break;
    } else {
      /* Not 5 letter, see what else */
      if (strlen(w) < 5) {
        /* Don't bother about whether this is initial or not, we need to be able to lookup
           single rafsi for breaking fuivla up. */
        
        strcpy(buf,"%");
        strcat(buf,w);
        trans = translate_comp(buf, &conversion);
        if (trans) {
          if (!initial) {
            strcat(result, "-");
          }
          strcat(result, trans);
        } else {
          if (!initial) {
            strcat(result, "-");
          }
          strcat(result, "?");
        }
        break;
      } else {
        /* Try to pull off leading rafsi component and process remainder */
        yy = strchr(w, 'y');
        if (yy) {
          ypos = yy - w;
        }
        if (yy && ypos == 3) {
          /* 3 letter rafsi with y after */
          if (!strncmp(w, "sel", 3)) {
            conversion = 2;
          } else if (!strncmp(w, "ter", 3)) {
            conversion = 3;
          } else if (!strncmp(w, "vel", 3)) {
            conversion = 4;
          } else if (!strncmp(w, "xel", 3)) {
            conversion = 5;
          } else {
            buf[0] = '%';
            strncpy(buf+1,w,3);
            buf[4] = 0;
            trans = translate_comp(buf, &conversion);
            append_trans(trans, result, initial);
          }
          w += 4; /* and go around again */
        } else if (yy && ypos == 4) {
          /* 4 letter rafsi with y after */
          buf[0] = '%';
          strncpy(buf+1,w,4);
          buf[5] = 0;
          trans = translate_comp(buf, &conversion);
          append_trans(trans, result, initial);
          w += 5; /* and go around again */

        } else {
          /* 'y' does not terminate leading rafsi, or there is no 'y'
             in the word at all.  Try to pull off 3 letter rafsi, or 4
             letter one of form CV'V.  Remember to remove following
             glue character if necessary, can only apply to first
             rafsi. */
          buf[0] = '%';
          if (w[2] == '\'') {
            strncpy(buf+1,w,4);
            buf[5] = 0;
            trans = translate_comp(buf, &conversion);
            append_trans(trans, result, initial);

            if (strchr("aeiou", w[1]) &&
                strchr("aeiou", w[3]) &&
                initial &&
                w[4] &&
                (w[4] == 'r' ||
                 (w[4] == 'n' && w[5] == 'r'))) {
              w += 5;
            } else {
              w += 4;
            }
          } else {
            if (!strncmp(w, "sel", 3)) {
              conversion = 2;
            } else if (!strncmp(w, "ter", 3)) {
              conversion = 3;
            } else if (!strncmp(w, "vel", 3)) {
              conversion = 4;
            } else if (!strncmp(w, "xel", 3)) {
              conversion = 5;
            } else {
              strncpy(buf+1,w,3);
              buf[4] = 0;
              trans = translate_comp(buf, &conversion);
              append_trans(trans, result, initial);
            }
            if (strchr("aeiou", w[1]) &&
                strchr("aeiou", w[2]) &&
                initial &&
                w[3] &&
                (w[3] == 'r' ||
                 (w[3] == 'n' && w[4] == 'r'))) {
              w += 4;
            } else {
              w += 3;
            }
          }          
        }
      }
    }

    initial = 0;

  } while (*w);
  return result;
}

/* ================================================== */

static int
is_consonant(char c)
{
  return (strchr("bcdfgjklmnprstvxz", c) != 0);
}

/* ================================================== */

static int
is_consonant_not_r(char c)
{
  return (strchr("bcdfgjklmnpstvxz", c) != 0);
}

/* ================================================== */

/* ================================================== */

/* ================================================== */

char *
translate_unknown(char *w)
{
  static char buf[2048];
  int len, i;
  int hyph;
  char *p, *q;
  char *ltrans;

  /* See whether the word is a fuivla.  If so, lookup the leading
     portion as a lujvo/rafsi, otherwise lookup the whole thing as a
     lujvo. */
  
  /* Stage 3 fuivla characterised by starting with a CVC or 4 letter
     rafsi or lujvo, then a hyphen, then a lojbanised version of the
     import */
  
  len = strlen(w);
  hyph = 0;
  /* Seek location of hyphen.  Import word must have at least 2 letters. */
  for (i=1; i<len-2; i++) {
    if (is_consonant_not_r(w[i-1]) && (w[i] == 'r') && is_consonant_not_r(w[i+1])) {
      hyph = i;
      break;
    }
    
    if (is_consonant(w[i-1]) && is_consonant(w[i+1])) {
      if ((w[i-1] == 'n') && (w[i+1] == 'r') && (w[i] == 'l')) {
        hyph = i;
        break;
      } else if ((w[i-1] == 'r') && (w[i+1] == 'n') && (w[i] == 'l')) {
        hyph = i;
        break;
      } else if (w[i] == 'n') {
        hyph = i;
        break;
      }
    }
  }

  if (hyph) {
    for (p=w, q=buf, i=0; i<hyph; p++, q++, i++) {
      *q = *p;
    }
    *q = 0;
    ltrans = translate_lujvo(buf);
    strcpy(buf, ltrans);
    strcat(buf, "-[");
    strcat(buf, w + hyph + 1);
    strcat(buf, "]");
    return buf;
  } else {
    /* Need to try for a stage 4 fuivla */

    return translate_lujvo(w);
  }

}
