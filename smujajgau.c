/*
  $Header$

  Program for reading plaintext dictionaries in the form

  lojban:english:comment      or just
  lojban:english
  
  and compiling them into a sorted database.  If the lojban is of the
  form X1 (i.e. ends in a digit), the entries X1n, X1v, X1a and X1t
  are deleted; they may be redefined by a later entry in the source
  dictionary.


  This is a total rewrite of the original gdbm based dictionary
  scheme.  This uses a locally defined file format (removing the need
  for gdbm library to be available), and compiles the whole dictionary
  in-core before writing it out.  This is many times faster because
  there aren't any random access disc accesses that gdbm generates.
  However, the core image could be quite significant for a big
  dictionary.

  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "version.h"

#define new_string(s) ((s) ? strcpy((char *) malloc(1+strlen(s)), (s)) : NULL)
#define new(T) (T *) malloc(sizeof(T))
#define new_array(T, n) (T *) malloc(sizeof(T) * (n))

typedef enum {
  DEFINE,
  ERASE
} Action;

/*+ Structure describing each 'transaction' to apply to build the
  final dictionary.  This allows the same word to be redefined several
  times in successive input files, with the latest definition taking
  precedence. +*/
typedef struct {
  Action action;
  char *key; /* The key to lookup */
  char *val; /* The result of the lookup (invalid for ERASE) */
  int ord; /* The ordinal position of this transaction in the complete
              sequence, used to resolve which comes first if several
              definitions for a single word over-ride one another. */
} Trans;

typedef struct Link {
  struct Link *next;
  Trans *trans;
} Link;

static Link *links = NULL;

/*+ Number of transactions on the list or in the array +*/
static int transord = 0;

/*+ Array of transactions, built up from the 'links' singly linked list +*/
static Trans **transac;

#define MKL 256
static int wordlens[MKL];


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
clear_histogram(void)
{
  int i;
  for (i=0; i<MKL; i++) {
    wordlens[i] = 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
print_histogram(void)
{
  int i;
  for (i=0; i<MKL; i++) {
    fprintf(stderr, "Words of length %3d : %6d\n", i, wordlens[i]);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Add a definition of a word to the transaction list.

  char *key

  char *val
  ++++++++++++++++++++++++++++++++++++++*/

static void
add_defn(char *key, char *val)
{
  Trans *new_trans;
  Link *new_link;
  new_trans = new(Trans);
  new_link = new(Link);

  new_trans->action = DEFINE;
  new_trans->key = new_string(key);
  new_trans->val = new_string(val);
  new_trans->ord = transord++;

  new_link->next = links;
  new_link->trans = new_trans;

  links = new_link;
}


/*++++++++++++++++++++++++++++++++++++++
  Add a deletion of a word to the transaction list.

  char *key
  ++++++++++++++++++++++++++++++++++++++*/

static void
add_deletion(char *key)
{
  Trans *new_trans;
  Link *new_link;
  new_trans = new(Trans);
  new_link = new(Link);

  new_trans->action = ERASE;
  new_trans->key = new_string(key);
  new_trans->val = NULL;
  new_trans->ord = transord++;

  new_link->next = links;
  new_link->trans = new_trans;

  links = new_link;
}


/*++++++++++++++++++++++++++++++++++++++
  Turn the linked list into an array, required for sorting.
  ++++++++++++++++++++++++++++++++++++++*/

static void
build_array(void)
{
  int i;
  Link *x;
  transac = new_array(Trans *, transord);
  for (x=links, i=transord-1;
       i>=0;
       x=x->next, i--) {
    transac[i] = x->trans;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Comparison function for sorting.  Sort first by the key field, then
  by the ordinal.  So if a key occurs twice, do the correct order of
  over-riding the definitions.

  static int sort_comparison
  ++++++++++++++++++++++++++++++++++++++*/

static int
sort_comparison(const void *a, const void *b)
{
  const Trans **aa = (const Trans **) a;
  const Trans **bb = (const Trans **) b;
  int sc;
  int ao, bo;
  sc = strcmp((*aa)->key, (*bb)->key);
  if (sc) {
    return sc;
  } else {
    ao = (*aa)->ord;
    bo = (*bb)->ord;
    if (ao > bo) {
      return 1;
    } else if (ao < bo) {
      return -1;
    } else {
      return 0;
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Sort the transaction array into order.
  ++++++++++++++++++++++++++++++++++++++*/

static void
sort_array(void)
{
  qsort(transac, transord, sizeof(Trans *), sort_comparison);
}


/*++++++++++++++++++++++++++++++++++++++
  Work down through the array.  For each key value, preserve only the
  final definition, if any.  Mark all over-ridden definitions and
  erasures with the ERASE action.
  ++++++++++++++++++++++++++++++++++++++*/

static void
rationalise_transactions(void)
{
  int top, bot; /* indices spanning a particular key, inclusive */
  int i;
  top = transord - 1;
  while (top >= 0) {
    bot = top - 1;
    while ((bot >= 0) &&
           (!strcmp(transac[bot]->key, transac[top]->key))) {
      bot--;
    }
    bot++;
    if (transac[top]->action == ERASE) { /* delete the definition altogether */
      for (i=bot; i<=top; i++) {
        transac[i]->action = ERASE;
      }
    } else {
      /* Keep final definition */
      for (i=bot; i<top; i++) {
        transac[i]->action = ERASE;
      }
    }
    top = bot - 1;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Squash all erasures and over-ridden definitions out of the list.
  ++++++++++++++++++++++++++++++++++++++*/

static void
compress_transactions(void)
{
  int i, j;
  i = j = 0;
  while (j < transord) {
    if (transac[j]->action == DEFINE) {
      transac[i++] = transac[j];
    }
    j++;
  }
  transord = i;
}


/*++++++++++++++++++++++++++++++++++++++
  Write a 'long' integer to file avoiding endianness problems.

  FILE *out

  unsigned long val
  ++++++++++++++++++++++++++++++++++++++*/

static void
put_long(FILE *out, unsigned long val)
{
  unsigned long a, b, c, d;
  /* Avoid endian-ness problem if we were to use fwrite */
  a = (val >> 24) & 0xff;
  b = (val >> 16) & 0xff;
  c = (val >>  8) & 0xff;
  d = (val >>  0) & 0xff;
  putc(a, out);
  putc(b, out);
  putc(c, out);
  putc(d, out);
}

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
  Write a single byte integer to file, range checking the operand.

  FILE *out

  unsigned long val
  ++++++++++++++++++++++++++++++++++++++*/

inline static void
put_char(FILE *out, unsigned long val)
{
  if (val > 255) {
    fprintf(stderr, "Tried to write a value that was too big\n");
  }
  putc(val&0xff, out);
}

/*++++++++++++++++++++++++++++++++++++++
  Write the entire dictionary to file

  FILE *out
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_database(FILE *out)
{
  int i, len;
  Trans *t;
  put_long(out, transord);
  for (i=0; i<transord; i++) {
    t = transac[i];
    len = strlen(t->key);
    ++wordlens[len];
    put_char(out, len);
    len = strlen(t->val);
    ++wordlens[len];
    put_char(out, len);
  }

  for (i=0; i<transord; i++) {
    t = transac[i];

    /* Write terminating null characters for each string.  This gives
       us the option of mmap'ing the data in translate.c */

    fwrite(t->key, sizeof(char), 1 + strlen(t->key), out);
    fwrite(t->val, sizeof(char), 1 + strlen(t->val), out);
  }
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

  int n_entries;
  Entry *entries;
  int i, len;
  char key[1024], val[1024];

  n_entries = get_long(in);
  entries = new_array(Entry, n_entries);
  for (i=0; i<n_entries; i++) {
    len = getc(in);
    entries[i].klen = len;
    len = getc(in);
    entries[i].vlen = len;
  }
  for (i=0; i<n_entries; i++) {
    fread(key, sizeof(char), entries[i].klen + 1, in); /* Read null termination */
    fread(val, sizeof(char), entries[i].vlen + 1, in); /* Read null termination .. */
    key[entries[i].klen] = 0; /* ... but set it anyway for safety */
    val[entries[i].vlen] = 0;
    add_defn(key, val);
  }

  free(entries);
}

/*++++++++++++++++++++++++++++++++++++++
  If we define an entry of the form abcde1, delete any entries of the form
  abcde1[nvat].  These have to be redefined after, if required.

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

static void
preen(char *s) {
  char buffer[128];
  char *suffixes[] = {"n", "v", "a", "t"};
  int i;
  for (i=0; i<4; i++) {
    strcpy(buffer, s);
    strcat(buffer, suffixes[i]);
    add_deletion(buffer);
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Handle one input file

  FILE *f
  ++++++++++++++++++++++++++++++++++++++*/

static void
do_file(FILE *f)
{
  char line[2048];
  char src[1024];
  char dest[1024];
  char *p, *q, *r;

  while (fgets(line, sizeof(line), f)) {
    line[strlen(line)-1] = 0;
    p = line;
    while (*p && isspace(*p)) p++;
    if (!*p) {
      /* Line is blank */
      continue;
    }
    if (strchr("#!;", *p)) {
      /* line is a comment */
      continue;
    }
    q = src;
    while (*p && *p!=':') {
      *q++ = *p++;
    }
    if (!*p) {
      fprintf(stderr, "Line [%s] does not contain a colon\n", line);
    } else {
      *q = 0;
      p++;
      r = dest;
      while (*p && *p!=':') {
        *r++ = *p++;
      }
      *r = 0;
      /* If we're about to add a case of something, check we get rid
         of any more specific entries first. */
      if (isdigit(src[strlen(src)-1])) {
        preen(src);
      }
      add_defn(src, dest);
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  The main routine

  int main

  int argc

  char **argv
  ++++++++++++++++++++++++++++++++++++++*/

int
main (int argc, char **argv) {
  char *dbname;
  FILE *in, *out;

  clear_histogram();
  
  if ((argc > 1) && (!strcmp(argv[1], "-v"))) {
    fprintf(stderr, "jvocuhadju version %s\n", version_string);
    exit(0);
  }

  if (argc < 3) {
    fprintf(stderr, "Usage : %s <dbname> <source1> ... <sourceN>\n", argv[0]);
    exit(1);
  }

  dbname = argv[1];
  argv += 2;

  /* Try to read input database */
  in = fopen(dbname, "r");
  if (in) {
    fprintf(stderr, "Reading existing database ...\n");
    read_database(in);
    fclose(in);
  } else {
    /* Doesn't exist, benign */
  }

  /* Run through input files, generating the transaction list. */
  if (*argv) {
    while (*argv) {
      fprintf(stderr, "Reading file %s ... \n", *argv);
      in = fopen(*argv, "r");
      if (!in) {
        fprintf(stderr, "Could not open %s\n", *argv);
      } else {
        do_file(in);
        fclose(in);
      }
      ++argv;
    }
  } else {
    do_file(stdin);
  }

  /* Go through processing steps */
  fprintf(stderr, "Building transaction array from list ...\n");
  build_array();
  fprintf(stderr, "Sorting transaction array ...\n");
  sort_array();
  fprintf(stderr, "Crunching transaction array ...\n");
  rationalise_transactions();
  compress_transactions();
  
  fprintf(stderr, "Write database ...\n");

  out = fopen(dbname, "w");
  if (!out) {
    fprintf(stderr, "Cannot open database file to write to\n");
    exit(1);
  }

  write_database(out);

  fclose(out);

  if (0) {
    print_histogram();
  }

  return 0;
}
