/***************************************
  $Header$

  Translation functions.
  ***************************************/

/* COPYRIGHT */

#include <gdbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "functions.h"

static int inited = 0;

typedef struct {
  char *key;
  char *val;
} Keyval;

static Keyval *dict = NULL;
static int n_entries = 0;

/*++++++++++++++++++++++++++++++++++++++
  Read a 'long' integer from file avoiding endianness problems.

  static unsigned long get_long

  FILE *in
  ++++++++++++++++++++++++++++++++++++++*/

static unsigned long
get_long(FILE *in)
{
  unsigned long a, b, c, d;
  /* Avoid endian-ness problem if we were to use fwrite */
  a = getc(in);
  b = getc(in);
  c = getc(in);
  d = getc(in);
  return (a << 24) | (b << 16) | (c << 8) | (d << 0);
}

/*++++++++++++++++++++++++++++++++++++++
  Read the database to build the transaction list.

  FILE *in
  ++++++++++++++++++++++++++++++++++++++*/


static void
read_database(FILE *in)
{
  typedef struct {
    int klen;
    int vlen;
  } Entry;

  Entry *entries;
  int i, len;
  char key[1024], val[1024];

  n_entries = get_long(in);
  entries = new_array(Entry, n_entries);
  dict = new_array(Keyval, n_entries);

  for (i=0; i<n_entries; i++) {
    len = getc(in);
    entries[i].klen = len;
    len = getc(in);
    entries[i].vlen = len;
  }
  for (i=0; i<n_entries; i++) {
    fread(key, sizeof(char), entries[i].klen, in);
    fread(val, sizeof(char), entries[i].vlen, in);
    key[entries[i].klen] = 0;
    val[entries[i].vlen] = 0;
    dict[i].key = new_string(key);
    dict[i].val = new_string(val);
  }

  Free(entries);
}



/* ================================================== */

static void
init(void) 
{
  char *dname;
  FILE *in;

  if (!inited) {
    inited = 1;
    dname = getenv("JBOFIHE_DICTIONARY");
    if (!dname) {
      dname = "./dictionary.bin";
    }
    in = fopen(dname, "r");
    if (!in) {
      fprintf(stderr, "Cannot open dictionary\n");
    } else {
      read_database(in);
      fclose(in);
    }
  }
}

/* ================================================== */

static int
comparison(const void *a, const void *b)
{
  const Keyval *aa, *bb;
  aa = (Keyval *) a;
  bb = (Keyval *) b;
  return strcmp(aa->key, bb->key);
}

/* ================================================== */

static char *
lookup(char *key)
{
  Keyval k, *res;
  k.key = key;
  res = bsearch(&k, dict, n_entries, sizeof(Keyval), comparison);
  if (res) {
    return res->val;
  } else {
    return NULL;
  }
}


/* ================================================== */

char *
translate(char *word)
{
  static char buf[1024];
  char *res;

  init();
  
  res = lookup(word);
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

  init();

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

/* ================================================== */

/*

  This section of the file deals with what I call 'advanced
  translation'.  The idea is to have (or be able to generate)
  different glosses for the various positions of brivla, depending on
  the context in which they arise in the text.

  The contexts that are recognized are :

  le X : noun (N)
  le A cu X : verb (V)
  le A cu X Y : adjective/adverb (=qualifier Q)
  [X->] le A cu Y : case tag (T)

  We identify 4 classes of word.  Different places of the same brivla
  may fall into different classes.  These are (patterns shown in []) :

  (D) discrete noun [x1 is a _, x1 are _ (plural)]
      e.g. nanmu (x1 is a man)
  (S) substance noun [x1 is _, x1 is _-like]
      e.g. djacu [x1 is some water, x1 is water-like)
  (A) actor noun [x1 is a _ (actor), x1 _ (action verb)]
      e.g. bajra (x1 is a runner, x1 runs)
  (P) property (adjective) [x1 is _ (adjective)]
      e.g. badri (x1 is sad)
  (R) reverse property (adjective) [x1 is _ (adjective)]
      e.g. te ckana (x1 is supported by a bed)

  (Perhaps some other classes will be defined later.)

  We write the entries in the dictionary like this

  nanmu1:D;man
  djacu1:S;water
  bajra1:A;run
  badri1:P;sad

  The glosses are generated according to the following table, where X is the
  gloss following the semicolon in the dictionary :

                                     CONTEXT
CLASS   |     N(oun)       V(erb) (4)          Q(ualifier)        T(ag) (2)
--------+--------------------------------------------------------------------
  D     |     X(s) (5)      being X(s) (6)          X              X(s) (6)
        |
  S     |       X            being X (6)            X               X (6)
        |
  A     |    X-er(s) (3)     X-ing (3)            X-ing (3)      X-er(s) (3)
        |
  P     |    X thing(s)      being X                X            X thing(s)
        |
  R     |    thing(s) X      being X                X            thing(s) X

  Notes
  (1) a->an if X starts with a vowel.

  (2) Glosses not put in for the x1 place, since they are apparent
  from the form of the gloss applied to the selbri itself.

  (3) Double final consonant of X where required (X ends in VC?)
  
  (4) Verb forms are written as participles for 2 reasons.  First,
  they avoid worrying about to use is/are, plus using is/are looks
  silly with other than present tense.  Second, it looks good as part
  of an abstraction (le nu nanmu) - this almost flows as real English!

  (5) (es) if word ends in s, z, ch.  (ies) if it ends in (-Cy).

  (6) Replace X or X(s) by the noun form, if a special noun entry is
  found in the dictionary (unless then overridden by an explicit V or
  T entry)

  If this ruleset is insufficient for some reason, some of the glosses
  can be specifically provided.  For example, the DN combination for
  nanmu1 will give the gloss 'man(s)', which is bad.  In this case,
  the NVQT context can be specified~:
  
  nanmu1n:man/men

  In this case, the verb form will automatically become 'being a
  man/men', unless a nanmu1v entry is found.

-------------------

  Other horrors to look into :

  lo X should gloss as 'a,any' for D,A,P and 'some' for S


 */

static char consonants[] = "bcdfghjklmnpqrstvwxz";
static char vowels[] = "aeiou";

static char buffers[32][1024];
static int bufptr=0;
#define GETBUF() (&buffers[bufptr=(bufptr+1)&0xf][0])

static int
starts_with_preposition(char *x)
{
  char *y;
  y = x;
  while (isspace(*y)) y++;
  if (!strncmp(y, "in", 2) ||
      !strncmp(y, "at", 2) ||
      !strncmp(y, "with", 4) ||
      !strncmp(y, "through", 7)
      ) {
    return 1;
  } else {
    return 0;
  }

}


/*++++++++++++++++++++++++++++++++++++++
  Remove the '*' markers from a translation (only really applies to
  discrete class words being used as qualifiers, so far).

  static char * basic_trans

  char *x
  ++++++++++++++++++++++++++++++++++++++*/

static char *
basic_trans(char *x)
{
  char *result, *p, *q;
  result = GETBUF();
  p = x, q = result;
  while (*p) {
    if (*p != '*') {
      *q++ = *p;
    }
    p++;
  }
  *q = 0;
  return result;
}

/*++++++++++++++++++++++++++++++++++++++
  Make a word plural, applying standard English rules for when to use
  -es or -ies instead of plain -s.

  static char * make_plural

  char *x
  ++++++++++++++++++++++++++++++++++++++*/

static char *
make_plural(char *x)
{
  char *result;
  int n;
  char *star_pos;

  result = GETBUF();

  star_pos = strchr(x, '*');
  if (star_pos) {
    char head[1024];
    char *result2;
    char *p, *q;
    result2 = GETBUF();
    p = x, q = head;
    while (p < star_pos) *q++ = *p++;
    *q = 0;
    p++;
    strcpy(result2, make_plural(head));
    if (strchr(p, '*')) {
      strcat(result2, make_plural(p));
    } else {
      strcat(result2, p);
    }
    return result2;
  } else {
    n = strlen(x);
    if (n>0 && (x[n-1] == 's' || x[n-1] == 'z' || x[n-1] == 'x')) {
      sprintf(result, "%s(es)", x);
    } else if (n>1 && x[n-1] == 'h' && x[n-2] == 'c') {
      sprintf(result, "%s(es)", x);
    } else if (n>1 && x[n-1] == 'y' && strchr(consonants, x[n-2])) {
      sprintf(result, "%s(ies)", x);
    } else {
      sprintf(result, "%s(s)", x);
    }

    return result;
  }

}


/*++++++++++++++++++++++++++++++++++++++
  Append "-er" to a word

  static char * append_er

  char *x
  ++++++++++++++++++++++++++++++++++++++*/

static char *
append_er(char *x)
{
  static char result[128];
  int n;
  char *star_pos;

  star_pos = strchr(x, '*');
  if (star_pos) {
    char head[1024];
    char *result2;
    char *p, *q;
    result2 = GETBUF();
    p = x, q = head;
    while (p < star_pos) *q++ = *p++;
    *q = 0;
    p++;
    strcpy(result2, append_er(head));
    if (!starts_with_preposition(p)) {
      strcat(result2, " of");
    }
    if (strchr(p, '*')) {
      strcat(result2, append_er(p));
    } else {
      strcat(result2, p);
    }
    return result2;
  } else {
    n = strlen(x);
    if (n > 1 && strchr(vowels, x[n-2]) && strchr(consonants, x[n-1])) {
      if (n > 2 && strchr(vowels, x[n-3])) {
        sprintf(result, "%s-er(s)", x);
      } else {
        sprintf(result, "%s-%cer(s)", x, x[n-1]);
      }
    } else if (n > 0 && x[n-1] == 'e') {
      sprintf(result, "%s-r(s)", x);
    } else {
      sprintf(result, "%s-er(s)", x);
    }
    return result;
  }

}

/*++++++++++++++++++++++++++++++++++++++
  Append "-ing" to a word

  static char * append_ing

  char *x
  ++++++++++++++++++++++++++++++++++++++*/

static char *
append_ing(char *x)
{
  static char result[128];
  int n;
  char *star_pos;

  star_pos = strchr(x, '*');
  if (star_pos) {
    char head[1024];
    char *result2;
    char *p, *q;
    result2 = GETBUF();
    p = x, q = head;
    while (p < star_pos) *q++ = *p++;
    *q = 0;
    p++;
    strcpy(result2, append_ing(head));
    if (strchr(p, '*')) {
      strcat(result2, append_ing(p));
    } else {
      strcat(result2, p);
    }
    return result2;
  } else {

    n = strlen(x);
    if (n > 1 && strchr(vowels, x[n-2]) && strchr(consonants, x[n-1])) {
      if (n > 2 && strchr(vowels, x[n-3])) {
        sprintf(result, "%s-ing", x);
      } else {
        sprintf(result, "%s-%cing", x, x[n-1]);
      }
    } else if (n > 1 && x[n-1] == 'e' && !strchr(vowels, x[n-2])) {
      strncpy(result, x, n-1);
      result[n-1] = 0;
      strcat(result, "-ing");
    } else {
      sprintf(result, "%s-ing", x);
    }
    
    return result;
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  static char * translate_pattern

  char *w

  int place

  char *suffix
  ++++++++++++++++++++++++++++++++++++++*/

static char *
translate_pattern(char *w, int place, char *suffix)
{
  char *new_start = NULL;
  int swap;
  static char result[128];
  char *buffer, *buffer2;
  char *trans;

  if (!strncmp("sel", w, 3)) {
    new_start = w+3;
    swap = 2;
  } else if (!strncmp("ter", w, 3)) {
    new_start = w+3;
    swap = 3;
  } else if (!strncmp("vel", w, 3)) {
    new_start = w+3;
    swap = 4;
  } else if (!strncmp("xel", w, 3)) {
    new_start = w+3;
    swap = 5;
  }
  
  if (new_start) {
    if (*new_start == 'y') {
      new_start++;
    }

    if (place == 1) {
      place = swap;
    } else if (place == swap) {
      place = 1;
    }

    buffer = GETBUF();

    /* FIXME - if we're left with a 3 letter rafsi, we ought to look
       up the gismu it corresponds to and translate that. */
    if (strlen(new_start) < 5) {
      sprintf(buffer,"@%s", new_start);
      trans = translate(buffer);
      if (!trans) {
        /* Invalid rafsi? or one derived from a cmavo.  In either
           case, we've no way of getting a default place structure for
           it. */
        return NULL;
      }
      buffer2 = GETBUF();
      strcpy(buffer2, trans);
      new_start = buffer2;
    }

    sprintf(buffer, "%s%1d%s", new_start, place, suffix);
    trans = translate(buffer);
    if (trans) {
      strcpy(result, trans);
      return result;
    } else {
      sprintf(buffer, "%s%1d", new_start, place);
      trans = translate(buffer);
      if (trans) {
        strcpy(result, trans);
        strcat(result, "??");
        return result;
      } else {
        return NULL;
      }
    }
    
  } else {
    return NULL;
  }


}

/*++++++++++++++++++++++++++++++++++++++
  'Advanced' translate.

  char * adv_translate Returns the english gloss of the word passed.

  char *w

  TransContext ctx
  ++++++++++++++++++++++++++++++++++++++*/

char *
adv_translate(char *w, int place, TransContext ctx)
{
  char *trans;
  char w1[128], w1n[128];
  char buffer[1024];
  static char result[1024];
  char ctx_suffix[4] = "nvqt";
  char *ctx_suf_as_string[4] = {"n", "v", "q", "t"};
  enum {CL_DISCRETE, CL_SUBSTANCE, CL_ACTOR, CL_PROPERTY, CL_REVERSE_PROPERTY} wordclass;
  int found_full_trans=0;

  /* Try looking up the explicit gloss asked for */
  sprintf(buffer, "%s%1d%c", w, place, ctx_suffix[ctx]);
  trans = translate(buffer);
  if (trans) {
    found_full_trans = 1;
  }

  if (!trans) {
    /* OK, no full translation found.  Lookup the wn form */
    sprintf(buffer, "%s%1d", w, place);
    trans = translate(buffer);
    if (!trans) {
      /* Try to match sel..., ter..., ...mau etc */
      trans = translate_pattern(w, place, "");
    }
  }

  if (!trans) {
    /* Try to match sel..., ter..., ...mau etc */
    trans = translate_pattern(w, place, ctx_suf_as_string[ctx]);
  }

  if (trans) {
    if (trans[0] == '@') {
      char *p, *q;
      int pos;
      p = buffer;
      q = trans+1;
      while (isalpha(*q)) {
        *p++ = *q++;
      }
      *p = 0;
      if (isdigit(*q)) {
        pos = *q - '0';
        return adv_translate(buffer, pos, ctx);
      } else {
        fprintf(stderr, "Error in dictionary entry for place %d of %s\n", place, w);
        return "??";
      }

      /* Never get here */
    }

    if (trans[1] == ';') {
      switch (trans[0]) {
        case 'D':
        case 'E': /* place holder for adding 'event' type later */
          wordclass = CL_DISCRETE;
          break;
        case 'S':
          wordclass = CL_SUBSTANCE;
          break;
        case 'A':
          wordclass = CL_ACTOR;
          break;
        case 'P':
          wordclass = CL_PROPERTY;
          break;
        case 'R':
          wordclass = CL_REVERSE_PROPERTY;
          break;
        default:
          fprintf(stderr, "Dictionary contains bogus extended entry for [%s]\n", buffer);
          return NULL;
          break;
      }
      
      strcpy(w1, trans+2);

      switch (wordclass) {
        case CL_DISCRETE:

          sprintf(buffer, "%s%1dn", w, place);
          trans = translate(buffer);
          if (trans) {
            strcpy(w1n, trans);
          } else {
            w1n[0] = 0;
          }

          switch (ctx) {
            case TCX_NOUN:
              strcpy(result, make_plural(w1));
              break;
            case TCX_VERB:
              if (*w1n) {
                sprintf(result, "being %s", w1n);
              } else {
                sprintf(result, "being %s", make_plural(w1));
              }
              break;
            case TCX_QUAL:
              strcpy(result, basic_trans(w1));
              break;
            case TCX_TAG:
              if (*w1n) {
                strcpy(result, w1n);
              } else {
                strcpy(result, make_plural(w1));
              }
              break;
          }

          break;

        case CL_SUBSTANCE:

          sprintf(buffer, "%s%1dn", w, place);
          trans = translate(buffer);
          if (trans) {
            strcpy(w1n, trans);
          } else {
            w1n[0] = 0;
          }

          switch (ctx) {
            case TCX_NOUN:
              strcpy(result, basic_trans(w1));
              break;
            case TCX_VERB:
              if (*w1n) {
                sprintf(result, "being %s", w1n);
              } else {
                sprintf(result, "being %s", w1);
              }
              break;
            case TCX_QUAL:
              strcpy(result, w1);
              break;
            case TCX_TAG:
              if (*w1n) {
                strcpy(result, w1n);
              } else {
                strcpy(result, w1);
              }
              break;
          }
          break;

        case CL_ACTOR:
          switch (ctx) {
            case TCX_NOUN:
            case TCX_TAG:
              strcpy(result, append_er(w1));
              break;
            case TCX_VERB:
            case TCX_QUAL:
              strcpy(result, append_ing(w1));
              break;
          }
          break;

        case CL_PROPERTY:
          switch (ctx) {
            case TCX_NOUN:
              sprintf(result, "%s thing(s)", w1);
              break;
            case TCX_VERB:
              sprintf(result, "being %s", w1);
              break;
            case TCX_QUAL:
              strcpy(result, w1);
              break;
            case TCX_TAG:
              sprintf(result, "%s thing(s)", w1);
              break;
          }
          break;

        case CL_REVERSE_PROPERTY:
          switch (ctx) {
            case TCX_NOUN:
              sprintf(result, "thing(s) %s", w1);
              break;
            case TCX_VERB:
              sprintf(result, "being %s", w1);
              break;
            case TCX_QUAL:
              strcpy(result, w1);
              break;
            case TCX_TAG:
              sprintf(result, "thing(s) %s", w1);
              break;
          }
          break;
      }

      return result;

    } else {
      if (!found_full_trans) {
        fprintf(stderr, "No advanced entry for [%s]\n", buffer);
      }
      /* Either it's not an advanced entry, we have to just return the
         word as-is, OR the word1n form matched. */
      return trans;
    }
  } else {
    /* If we can't get any place-dependent translation, don't bother -
       the gismu headword entry is probably misleading and does more
       harm than good. */
    fprintf(stderr, "No advanced entry for [%s]\n", buffer);
    trans = translate(w);
    if (trans) {
      strcpy(result, trans);
      strcat(result, "??");
      return result;
    } else {
      fprintf(stderr, "No dictionary entry for [%s], attempting to break up as lujvo\n", w);
      trans = translate_unknown(w);
      if (trans) {
        strcpy(result, trans);
        strcat(result, "??");
        return result;
      } else {
        strcpy(result, "??");
        return result;
      }
    }
  }
}

