/***************************************
  $Header$

  Low level routines for accessing the dictionary.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "functions.h"
#include "canonluj.h"
#include "morf.h"

#ifndef DEFAULT_DICTIONARY
#define DEFAULT_DICTIONARY "smujmaji.dat"
#endif

#include <unistd.h>
#include <sys/stat.h>

#ifdef HAVE_MMAP
#include <sys/types.h>
#include <sys/mman.h>
#endif

typedef struct {
  char *key;
  char *val;
} Keyval;

#ifdef EMBEDDED_DICTIONARY

#include "dictdata.c"

#else

static Keyval *dict = NULL;
static int n_entries = 0;

#endif

#ifndef EMBEDDED_DICTIONARY
static int inited = 0;

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

  struct stat sb;
  off_t offset;
  int result;
  char *dict_base = NULL;

  n_entries = get_long(in);
  entries = new_array(Entry, n_entries);
  dict = new_array(Keyval, n_entries);

  for (i=0; i<n_entries; i++) {
    len = getc(in);
    entries[i].klen = len;
    len = getc(in);
    entries[i].vlen = len;
  }

  if (fstat(fileno(in), &sb) < 0) {
    fprintf(stderr, "Could not stat the dictionary file\n");
    exit(1);
  }

  offset = ftell(in);

#ifdef HAVE_MMAP

  {
    char *mmap_base = NULL;
    mmap_base = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fileno(in), 0);
    result = (int) mmap_base;

    if (result < 0) {
      perror("Could not mmap the dictionary data\n");
      exit(1);
    }

    dict_base = mmap_base + offset;
  }

#else

  {
    size_t dict_size = sb.st_size - offset;

    dict_base = new_array(char, dict_size);
    result = fread(dict_base, sizeof(char), dict_size, in);
    if (result != dict_size) {
      perror("Could not read dictionary contents\n");
      exit(1);
    }
  }

#endif
  
  /* Loop through to build pointer arrays */
  for (i=0; i<n_entries; i++) {
    if (i == 0) {
      dict[i].key = dict_base;
    } else {
      dict[i].key = dict[i-1].val + entries[i-1].vlen + 1; /* Allow for null termination */
    }
    dict[i].val = dict[i].key + entries[i].klen + 1; /* Allow for null termination */
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
      dname = DEFAULT_DICTIONARY;
    }
    in = fopen(dname, "rb");
    if (!in) {
      inited = -1;
    } else {
      read_database(in);
      fclose(in);
    }
  }
}
#endif

/* ================================================== */

char *
dict_lookup(char *key)
{
  int l, m, h;
  int r;

#ifndef EMBEDDED_DICTIONARY
  if (!inited) {
    init();
  }
  if (inited < 0) {
    /* earlier problem opening dictionary file etc */
    return NULL;
  }
#endif
  
  /* Bisection search follows. */
  l = 0;
  h = n_entries;
  while (h > l) {
    m = (h + l) >> 1;
    r = strcmp(key, dict[m].key);
    if (r == 0) goto found;
    if (m == l) break;
    if (r < 0) h = m;
    else       l = m;
  }
  return NULL;

found:
  return dict[m].val;
}




