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

#include "functions.h"
#include "canonluj.h"

#ifndef DEFAULT_DICTIONARY
#define DEFAULT_DICTIONARY "smujmaji.dat"
#endif

#ifdef HAVE_MMAP
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
static char *mmap_base = NULL;
#endif

extern int show_dictionary_defects;

static int inited = 0;

typedef struct {
  char *key;
  char *val;
} Keyval;

static Keyval *dict = NULL;
static int n_entries = 0;

static char buffers[64][512];
static int bufptr=0;
#define GETBUF() (&buffers[bufptr=(bufptr+1)&0x3f][0])

/* ================================================== */

typedef struct Component {
  int start; /* 0 for 1st, pos of + for others (before conversions) */
  int pure_start; /* likewise, but just the gismu/cmavo excl conversions) */
  char text[6];
  int places[6];
} Component;

/*+ Forward prototype for place/context translater. +*/
extern char *adv_translate(char *w, int place, TransContext ctx);
static void split_into_comps(char *canon, Component *comp, int *ncomp);

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

  n_entries = get_long(in);
  entries = new_array(Entry, n_entries);
  dict = new_array(Keyval, n_entries);

  for (i=0; i<n_entries; i++) {
    len = getc(in);
    entries[i].klen = len;
    len = getc(in);
    entries[i].vlen = len;
  }

#ifdef HAVE_MMAP

  {
    struct stat sb;
    off_t offset;
    int result;

    if (fstat(fileno(in), &sb) < 0) {
      fprintf(stderr, "Could not stat the dictionary file\n");
      exit(1);
    }

    offset = ftell(in);
    mmap_base = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fileno(in), 0);
    result = (int) mmap_base;

    if (result < 0) {
      perror("Could not mmap the dictionary data\n");
      exit(1);
    }

    /* Loop through to build pointer arrays */
    for (i=0; i<n_entries; i++) {
      if (i == 0) {
        dict[i].key = mmap_base + offset;
      } else {
        dict[i].key = dict[i-1].val + entries[i-1].vlen + 1; /* Allow for null termination */
      }
      dict[i].val = dict[i].key + entries[i].klen + 1; /* Allow for null termination */
    }
  }
  
#else

  {
    char key[1024], val[1024];

    for (i=0; i<n_entries; i++) {
      fread(key, sizeof(char), entries[i].klen + 1, in);
      fread(val, sizeof(char), entries[i].vlen + 1, in);
      dict[i].key = new_string(key);
      dict[i].val = new_string(val);
    }
  }

#endif

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
      dname = DEFAULT_DICTIONARY;
    }
    in = fopen(dname, "rb");
    if (!in) {
      if (show_dictionary_defects) {
        fprintf(stderr, "Cannot open dictionary\n");
      }
      inited = -1;
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
  char *buf;
  char *res;

  buf = GETBUF();
  
  init();
  if (inited == -1) return "?";
  
  res = lookup(word);
  if (res) {
    strcpy(buf, res);
    return buf;
  } else {
    return NULL;
  }

}

/* ================================================== */

/* Lookup a lujvo that isn't matched in the ordinary dictionary, by
   smashing it into consituent rafsi and glueing these together. */
static char *
translate_lujvo(char *word, int place)
{
  Component comp[32];
  int ncomp;
  char *canon;
  char *result;
  char *trans;
  int i;
  int xplace;
  
  canon = canon_lujvo(word);
  if (!canon) return "?";
  
  split_into_comps(canon, comp, &ncomp);
  if (ncomp < 2) {
    /* Prevent infinite recursion */
    return NULL;
  }
  result = GETBUF();
  result[0] = 0;
  for (i=0; i<ncomp; i++) {
    int last = (i == (ncomp - 1));
    xplace = comp[i].places[last ? place : 1];
    trans = adv_translate(comp[i].text, xplace, last ? TCX_NOUN : TCX_QUAL);
    if (i > 0) { strcat(result, "-"); }
    if (trans) {
      strcat(result, trans);
    } else {
      strcat(result, "?");
    }
  }

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

char *
translate_fuivla_prefix(char *w, int place)
{
  char *canon;
  Component comp[32];
  int n_comps;
  static char buffer[1024];
  char *trans;
  int i;

  canon = canon_lujvo(w);
  split_into_comps(canon, comp, &n_comps);
  buffer[0] = 0;
  for (i=0; i<n_comps; i++) {
    int first = (i == 0);
    int last  = (i == (n_comps-1));
    trans = adv_translate(comp[i].text, comp[i].places[1], last ? TCX_NOUN : TCX_QUAL);
    if (!first) strcat(buffer, "-");
    if (trans) {
      strcat(buffer, trans);
    } else {
      strcat(buffer, "?");
    }
  }   
  return buffer;
}

/* ================================================== */

char *
translate_unknown(char *w, int place)
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
    ltrans = translate_fuivla_prefix(buf, place);
    if (ltrans) {
      strcpy(buf, ltrans);
    } else {
      strcpy(buf, "?");
    }
    strcat(buf, "-[");
    strcat(buf, w + hyph + 1);
    strcat(buf, "]");
    return buf;
  } else {
    /* Need to try for a stage 4 fuivla */

    return translate_lujvo(w, place);
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
  (I) 'idiomatic' [x1 is X-ing], but where verb doesn't support -er
      form e.g. kuspe (x1 is extending)

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
  D     |     X(s) (5)      being X(s) (6)        X-ish            X(s) (6)
        |
  S     |       X            being X (6)          X-ish             X (6)
        |
  A     |    X-er(s) (3)     X-ing (3)            X-ing (3)      X-er(s) (3)
        |
  P     |    X thing(s)      being X                X            X thing(s)
        |
  R     |    thing(s) X      being X                X            thing(s) X
        |
  I     |  thing(s) X-ing    X-ing                X-ing        thing(s) X-ing

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

  return NULL;

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
  

  char * fix_trans_in_context

  char *trans

  TransContext ctx

  char *w1n
  ++++++++++++++++++++++++++++++++++++++*/

char *
fix_trans_in_context(char *src, char *trans, TransContext ctx, char *w1n, int found_full_trans)
{
  enum {CL_DISCRETE, CL_SUBSTANCE, CL_ACTOR, CL_PROPERTY, CL_REVERSE_PROPERTY, CL_IDIOMATIC} wordclass;
  char *result;
  char w1[128];

  result = GETBUF();
  
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
      case 'I':
        wordclass = CL_IDIOMATIC;
        break;
      default:
        if (show_dictionary_defects) {
          fprintf(stderr, "Dictionary contains bogus extended entry for [%s]\n", src);
        }
        return NULL;
        break;
    }
    
    strcpy(w1, trans+2);
    
    switch (wordclass) {
      case CL_DISCRETE:
        
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
            strcat(result, "-ish");
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
            strcat(result, "-ish");
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

      case CL_IDIOMATIC:
        {
          char tempbuf[1024];
          strcpy(tempbuf, append_ing(w1));
            
          switch (ctx) {
            case TCX_NOUN:
            case TCX_TAG:
              sprintf(result, "thing(s) %s", tempbuf);
              break;
            case TCX_VERB:
            case TCX_QUAL:
              strcpy(result, tempbuf);
              break;
          }
        }
      break;

    }

    return result;

  } else {
    if (!found_full_trans && show_dictionary_defects) {
      fprintf(stderr, "No advanced entry for [%s]\n", src);
    }
    /* Either it's not an advanced entry, we have to just return the
       word as-is, OR the word1n form matched. */
    return trans;
  }

}


/*++++++++++++++++++++++++++++++++++++++
  

  static char * subst_base_in_pattern

  char *trans

  char *base
  ++++++++++++++++++++++++++++++++++++++*/

static char *
subst_base_in_pattern(char *trans, char *base)
{
  char *result = GETBUF();
  char *p, *q, *r;
  char *localtrans;
  q = result;
  p = trans;
  while (*p) {
    if (*p == '%') {
      int place = *++p - '0';
      char context = *++p;
      TransContext ctx;
      switch (context) {
        case 'n': ctx = TCX_NOUN; break;
        case 'v': ctx = TCX_VERB; break;
        case 'q': ctx = TCX_QUAL; break;
        case 't': ctx = TCX_TAG; break;
        default: 
          ctx = TCX_NOUN;
          if (show_dictionary_defects) {
            fprintf(stderr, "Broken base context for %s\n", trans);
          }
          break;
      }
      localtrans = adv_translate(base, place, ctx);
      r = localtrans;
      while (*r) {
        *q++ = *r++;
      }
      ++p; /* onto the char beyond the context symbol */
    } else {
      *q++ = *p++;
    }
  }
  *q = 0;
  return result;
}

/*++++++++++++++++++++++++++++++++++++++
  Take a string a+b+c and split it into an array of components
  separated by + signs.  If any components are se, te etc, bind them
  as place exchanges to the following component.

  char *canon

  Component *comp

  int *ncomp
  ++++++++++++++++++++++++++++++++++++++*/

static void
split_into_comps(char *canon, Component *comp, int *ncomp)
{
  int i;
  int nc = 0;
  char buffer[256];
  char *p, *q;
  int place = 0;

  strcpy(buffer, canon);
  p = buffer;
  while (*p) p++;
  while (p > buffer) {
    while (p > buffer && *p != '+') p--;
    place = 0;
    q = p + (*p == '+');
    if (!strcmp(q, "se")) { place = 2; }
    else if (!strcmp(q, "te")) { place = 3; }
    else if (!strcmp(q, "ve")) { place = 4; }
    else if (!strcmp(q, "xe")) { place = 5; }
    if (place > 0) {
      int t = comp[0].places[1];
      comp[0].places[1] = comp[0].places[place];
      comp[0].places[place] = t;
      comp[0].start = (p - buffer);
    } else {
      for (i=nc-1; i>=0; i--) {
        comp[i+1] = comp[i];
      }
      for (i=1; i<=5; i++) {
        comp[0].places[i] = i;
      }
      nc++;
      comp[0].pure_start = comp[0].start = (p - buffer);
      strcpy(comp[0].text, q);
    }
    *p = 0;
  }

  *ncomp = nc;

}


/*++++++++++++++++++++++++++++++++++++++
  

  static char * lookup_template_match

  int prec

  char *orig

  Comp *comp

  int ncomp

  int place

  TransContext ctx
  ++++++++++++++++++++++++++++++++++++++*/

static char *
lookup_template_match(int prec, int suffix, int gather, char *orig, Component *comp, int ncomp, int place, TransContext ctx)
{
  char generic[128]; /* the part that's found in the LHS of the dictionary pattern match */
  char specific[256]; /* the other part of the string */
  char buffer[256];
  int cutg, cuts;
  int new_place;
  char *ctx_suf_as_string[4] = {"n", "v", "q", "t"};
  char *trans;
  int got_full_trans;
  char * subst;

  if (suffix) {
    cutg = comp[ncomp-gather].pure_start;
    cuts = comp[ncomp-gather].start;
    strcpy(generic, orig + cutg);
    strncpy(specific, orig, cuts);
    specific[cuts] = 0;
    new_place = comp[ncomp-gather].places[place];
  } else {
    cutg = comp[gather].start + 1;
    cuts = cutg;
    strncpy(generic, orig, cutg);
    generic[cutg] = 0;
    strcpy(specific, orig + cuts);
    new_place = comp[gather-1].places[place];
  }

  sprintf(buffer, "*%1d%s%1d%s",
          prec, generic, new_place, ctx_suf_as_string[(int) ctx]);
  trans = translate(buffer);
  if (trans) {
    got_full_trans = 1;
  } else {
    got_full_trans = 0;
    sprintf(buffer, "*%1d%s%1d",
            prec, generic, new_place);
    trans = translate(buffer);
  }
  
  if (trans) {
    if (trans[0] == '@') {
      /* redirection to another form */
      int redir_place = trans[1] - '0';
      return adv_translate(specific, redir_place, ctx);
    } else {
      /* Need to substitute % stuff. */
      subst = subst_base_in_pattern(trans, specific);
      if (got_full_trans) {
        return subst;
      } else {
        char buffer1[256], *trans1, w1n[256];
        sprintf(buffer1, "*%1d%s%1dn", prec, generic, new_place);
        trans1 = translate(buffer1);
        if (trans1) { strcpy(w1n, trans1); }
        else { w1n[0] = 0; }
        return fix_trans_in_context(buffer, subst, ctx, w1n, got_full_trans);
      }
    }
  } else {
    return NULL;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Try to match the Lojban word with various standard forms which the
  dictionary provides.

  char * attempt_pattern_match Return the translation obtained, or
  NULL if pattern match failed to find anything.

  char *w The Lojban word to pattern match on

  int place The place whose translation is required

  TransContext ctx The context in which the translation is required.
  ++++++++++++++++++++++++++++++++++++++*/

static char *
attempt_pattern_match(char *w, int place, TransContext ctx)
{
  char *canon;
  char *trans;

  Component comp[32];
  int ncomp;

  canon = canon_lujvo(w);

  /* Bail out now if it's not a lujvo that can be split up. */
  if (!canon) return NULL;

  /* If it's not got a '+' in it, give up too. */
  if (!strchr(canon, '+')) return NULL;

  /* Split components into array. Work from the right. */
  split_into_comps(canon, comp, &ncomp);

#if 0
  {
    int i;
    for (i=0; i<ncomp; i++) {
      fprintf(stderr, "Comp %d text %s places %d %d %d %d %d start %d pure %d\n",
              i, comp[i].text,
              comp[i].places[1], comp[i].places[2], comp[i].places[3], comp[i].places[4], comp[i].places[5],
              comp[i].start, comp[i].pure_start);
    }
  }
#endif

  /* If there's only 1 component after conversions have been
     mapped onto it, do a place switch */
  if (ncomp == 1) {
    int new_place = comp[0].places[place];
    return adv_translate(comp[0].text, new_place, ctx);

  } else {
    /* Multiple components. Loop over precedence and try to match
       suffix then prefix at each level. */
    int prec;
    int suffix;
    int gather;

    for (prec = 5; prec >= 0; prec--) {
      for (suffix = 1; suffix >= 0; suffix--) {
        for (gather = 1; gather <= 2; gather++) {
          trans = lookup_template_match(prec, suffix, gather, canon, comp, ncomp, place, ctx);
          if (trans) {
            if (strcmp(trans, "-")) {
              /* didn't match */
              return trans;
            }
          } else {
            break; /* out of gather loop */
          }
        }
      }
    }
  }

  return NULL;

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
  char *trans, *trans1;
  char w1n[128];
  char buffer[1024], buffer1[1024];
  static char result[1024];
  char ctx_suffix[4] = "nvqt";
  char *ctx_suf_as_string[4] = {"n", "v", "q", "t"};
  int found_full_trans=0;

  /* Special case.  A request for place 0 is mapped to a request for
     the x1 verb form.  This is to represent an 'infinitive' form
     required for translating things like jai bau cusku. */

  if (place == 0) {
    place = 1;
    ctx = TCX_VERB;
  }

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
        if (show_dictionary_defects) {
          fprintf(stderr, "Error in dictionary entry for place %d of %s\n", place, w);
        }
        return "??";
      }

      /* Never get here */
    }

    sprintf(buffer1, "%s%1dn", w, place);
    trans1 = translate(buffer1);
    if (trans1) {
      strcpy(w1n, trans1);
    } else {
      w1n[0] = 0;
    }

    strcpy(result, fix_trans_in_context(buffer, trans, ctx, w1n, found_full_trans));

    return result;

  } else {
    /* If we can't get any place-dependent translation, don't bother -
       the gismu headword entry is probably misleading and does more
       harm than good. */
    if (show_dictionary_defects) {
      fprintf(stderr, "No advanced entry for [%s]\n", buffer);
    }
    trans = translate(w);
    if (trans) {
      strcpy(result, trans);
      strcat(result, "??");
      return result;
    } else {
      /* Try to get pattern match for word */
      if (show_dictionary_defects) {
        fprintf(stderr, "No dictionary entry for [%s], attempting pattern match\n", w);
      }
      trans = attempt_pattern_match(w, place, ctx);
      if (trans) {
        return trans;
      }

      if (show_dictionary_defects) {
        fprintf(stderr, "No dictionary entry for [%s], attempting to break up as lujvo\n", w);
      }
      trans = translate_unknown(w, place);
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

