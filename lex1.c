/***************************************
  $Header$

  Read the input file and perform low-level lexing functions.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "cmavotab.h"
#include "nodes.h"
#include "functions.h"
#include "lujvofns.h"
#include "morf.h"
#include "bccheck.h"

static char zoi_form[8]; /* la'o or zoi */
static int  zoi_delim_next;
static char zoi_delim[64];
static int zoi_start_line, zoi_start_col;
static char *zoi_data;

extern int had_bad_tokens; /* in main.c */

static int process_word(char *buf, int start_line, int start_column);

/*++++++++++++++++++++++++++++++

  Tables for encoding cmavos into the indices in the cmavo_table.

  ++++++++++++++++++++++++++++++*/


static int tab1[] = { -1,  1,  2,  3, -1, /* abcde */
                      4,  5, -1, -1,  6, /* fghij */
                      7,  8,  9, 10, -1, /* klmno */
                      11, -1, 12, 13, 14, /* pqrst */
                      -1, 15, -1, 16, -1, /* uvwxy */
                      17}; /* z */

static int tab2[] = {  1, -1, -1, -1,  2, /* abcde */
                       -1, -1, -1,  3, -1, /* fghij */
                       -1, -1, -1, -1,  4, /* klmno */
                       -1, -1, -1, -1, -1, /* pqrst */
                       5, -1, -1, -1,  6, /* uvwxy */
                       -1}; /* z */


/*++++++++++++++++++++++++++++++++++++++
  Compute a hash function of a string representing a cmavo.  This
  hashes into the cmavo_table array.

  static int hash_cmavo

  char *buf
  ++++++++++++++++++++++++++++++++++++++*/


static int
hash_cmavo(char *buf) {
  int val, v;
  char *p;

  val = 0;
  p = buf;
  if (*p == '\'') return -1;
  v = tab1[*p - 'a'];
  if (v > 0) {
    val = v * 98;
    p++;
  }

  if (*p == '\'') return -1;
  v = tab2[*p - 'a'];
  val += v * 14;
  p++;

  if (!*p) return val;

  if (*p == '\'' || *p == ',') {
    val += 7;
    p++;
  }

  if (*p == '\'') return -1;
  v = tab2[*p - 'a'];
  val += v;
  return val;
}


/*++++++++++++++++++++++++++++++++++++++
  Process an individual cmavo.

  char *buf
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_cmavo(char *buf, int start_line, int start_column)
{
  int hash;
  TreeNode *node;
  int i, n, ok;

  /* Check cmavo contains only valid characters */
  n = strlen(buf);
  ok = 1;
  for (i=0; i<n; i++) {
    if (!strchr("abcdefgijklmnoprstuvxyz'", buf[i])) {
      ok = 0;
      break;
    }
  }

  if (!ok) {
    fprintf(stderr, "Bad cmavo [%s]\n", buf);
    had_bad_tokens = 1;
    return;
  }
    
  hash = hash_cmavo(buf);

  node = new_node();

  if (hash >= 0 && hash <= 1750 && !strcmp(buf, cmavo_table[hash].cmavo)) {
    node->data.cmavo.code = hash;
    node->type = N_CMAVO;
    node->start_line = start_line;
    node->start_column = start_column;
    node->data.cmavo.selmao = cmavo_table[hash].selmao;
  } else {
    fprintf(stderr, "Bad cmavo [%s] at line %d column %d\n",
            buf, start_line, start_column);
    had_bad_tokens = 1;
    node->data.garbage.word = new_string(buf);
    node->type = N_GARBAGE;
    node->start_line = start_line;
    node->start_column = start_column;
  }    
  add_token(node);

}

/*++++++++++++++++++++++++++++++++++++++
  Take the text in 'x' and add it as a single brivla to the token list.

  char *x

  int start_line

  int start_column
  ++++++++++++++++++++++++++++++++++++++*/

static void
add_brivla_token(char *x, int start_line, int start_column, enum BrivlaType brivla_type)
{
  TreeNode *tok;

  tok = new_node();
  tok->start_line = start_line;
  tok->start_column = start_column;
  tok->type = N_BRIVLA;
  /* Leave commas etc in for BRIVLA - you can use them in a
     fu'ivla? */
  tok->data.brivla.word = new_string(x);
  tok->data.brivla.type = brivla_type;
  add_token(tok);


}

/*++++++++++++++++++++++++++++++++++++++
  Handle a single token that has a consonant at the end.  This is basically a
  cmene, except that checks have to be done for embedded la, lai, doi.
  ++++++++++++++++++++++++++++++++++++++*/

static void
process_cmene(char *buf, int start_line, int start_column,
              int is_bad, int can_split, char *ladoi, char *tail)
{
  TreeNode *tok;

  if (is_bad)  {
    char prefix[1024], labuf[8];
    char *p, *q;
    int len1, len2;
    for (p=buf, q=prefix, len1=0; p!=ladoi; len1++) *q++ = *p++;
    *q = 0;
    for (p=ladoi, q=labuf, len2=0; p!=tail; len2++) *q++ = *p++;
    *q = 0;

    if (prefix[0]) process_word(prefix, start_line, start_column);
    process_word(labuf, start_line, start_column+len1);
    process_word(tail, start_line, start_column+len1+len2);
  } else {
    tok = new_node();
    tok->start_line = start_line;
    tok->start_column = start_column;
    tok->type = N_CMENE;
    tok->data.cmene.word = new_string(buf);
    add_token(tok);
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Take cmavo off the front of a gismu, lujvo or fu'ivla
  ++++++++++++++++++++++++++++++++++++++*/

static void
add_preceding_cmavo(char **pws, char **pwe, int start_line, int *column)
{
  char **pw;
  char *p, *q;
  int incr;
  char buf2[1024];
  for (pw=pws; pw<pwe; pw++) {
    incr = 0;
    for (p=*pw, q=buf2; p<*(pw+1);) {
      *q++ = *p++;
      incr++;
    }
    *q = 0;
    process_cmavo(buf2, start_line, *column);
    *column += incr;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Handle a single word delimited by whitespace or periods.  Break it
  into single words if necessary and handle each in turn.  Return 1 if
  not EOF, or 0 if fa'o detected.

  char *buf
  ++++++++++++++++++++++++++++++++++++++*/

static int
process_word(char *buf, int start_line, int start_column)
{
  char buf2[1024];
  TreeNode *tok;
  char *word_starts[1024];
  char **pws, **pwe;
  MorfType morf_type;
  struct morf_xtra mx;
  int column, incr;

  if (zoi_data) {
    if (!strcmp(buf, zoi_delim)) {
      tok = new_node();
      tok->start_line = zoi_start_line;
      tok->start_column = zoi_start_col;
      tok->type = N_ZOI;
      tok->data.zoi.form = new_string(zoi_form);
      tok->data.zoi.term = new_string(zoi_delim);
      tok->data.zoi.text = zoi_data;
      add_token(tok);
      zoi_data = NULL;
      zoi_delim[0] = 0;
    } else {
      if (zoi_data[0]) {
        zoi_data = extend_string(zoi_data, " ");
      }
      zoi_data = extend_string(zoi_data, buf);
    }
    return 1;
  }

  if (zoi_delim_next) {
    strcpy(zoi_delim, buf);
    zoi_data = new_string("");
    zoi_delim_next = 0;
    return 1;
  }

  if (!strcmp(buf, "zoi")) {
    strcpy(zoi_form, "zoi");
    zoi_delim_next = 1;
    zoi_start_line = start_line;
    zoi_start_col = start_column;
    return 1;
  }

  if (!strcmp(buf, "la'o")) {
    strcpy(zoi_form, "la'o");
    zoi_delim_next = 1;
    zoi_start_line = start_line;
    zoi_start_col = start_column;
    return 1;
  }

  /* Analyse word type */
  
  pws = pwe = word_starts;
  /* FIXME: Need to get morf_xtra info back here, to help with splitting bad
   * cmene */
  morf_type = morf_scan(buf, &pwe, &mx);
  column = start_column;
  switch (morf_type) {
    case MT_BOGUS:
      fprintf(stderr, "Unrecognizable word '%s' at line %d column %d\n",
              buf, start_line, column);
      had_bad_tokens = 1;
      break;
    case MT_BAD_UPPERCASE:
      fprintf(stderr, "Invalid uppercase letters in word '%s' at line %d column %d\n",
              buf, start_line, column);
      had_bad_tokens = 1;
      break;
    case MT_GISMU:
      add_preceding_cmavo(pws, pwe, start_line, &column);
      add_brivla_token(*pwe, start_line, column, BVT_GISMU);
      break;
    case MT_LUJVO:
      add_preceding_cmavo(pws, pwe, start_line, &column);
      add_brivla_token(*pwe, start_line, column, BVT_LUJVO);
      break;
    case MT_FUIVLA3:
    case MT_FUIVLA3_CVC:
    case MT_FUIVLA3X:
    case MT_FUIVLA3X_CVC:
      add_preceding_cmavo(pws, pwe, start_line, &column);
      add_brivla_token(*pwe, start_line, column, BVT_FUIVLA3);
      break;
    case MT_FUIVLA4:
      add_preceding_cmavo(pws, pwe, start_line, &column);
      add_brivla_token(*pwe, start_line, column, BVT_FUIVLA4);
      break;
    case MT_CMENE:
      process_cmene(buf, start_line, column,
                    mx.u.cmene.is_bad,
                    mx.u.cmene.can_split,
                    mx.u.cmene.ladoi,
                    mx.u.cmene.tail);
      break;
    case MT_CMAVOS:
      {
        char **pw;
        char *p, *q;
        for (pw=pws; pw<=pwe; pw++) {
          incr = 0;
          for (p=*pw, q=buf2; ((pw<pwe)&&(p<*(pw+1)))||((pw==pwe)&&(*p));) {
            *q++ = *p++;
            incr++;
          }
          *q = 0;
          process_cmavo(buf2, start_line, column);
          column += incr;
        }
      }
      break;
    default:
      assert(0);
      break;
  }
  return 1;
}

#define CLOSE_TOKEN \
  *p = 0; \
  if (p > buffer) { \
    process_word(buffer, start_line, start_column); \
    new_tok = 1; \
    p = buffer; \
  }

#define SET_START if (new_tok) { new_tok = 0; start_line = line; start_column = column; }

/*++++++++++++++++++++++++++++++++++++++
  Parse an entire file through to the end.

  FILE *f The file handle to parse
  ++++++++++++++++++++++++++++++++++++++*/

void
parse_file(FILE *f)
{
  char buffer[1024]; /* buffer for words */
  char *p;
  int c;
  int line;
  int column;
  int start_line=1, start_column=0; /* These are only initialised to
                                       shut up the compiler */
  int new_tok;
  int paren_depth = 0;

  zoi_delim[0] = 0;
  zoi_delim_next = 0;

  line = 1;
  column = 0;
  new_tok = 1;

  p = buffer;
  do {
    c = fgetc(f);
    column++;
    if (paren_depth > 0) {
      if (c == ')') {
        paren_depth--;
        new_tok = 1;
      } else if (c == '(') {
        paren_depth++;
      } else if (c == '\n') {
        line++;
        column=0;
      }
      
    } else {
      
      if (c>='0' && c<='9') {
        static char *numbers[] = {"no", "pa", "re", "ci", "vo",
                                  "mu", "xa", "ze", "bi", "so"};
        CLOSE_TOKEN;
        SET_START;
        strcpy(buffer, numbers[(int)(c-'0')]);
        process_word(buffer, start_line, start_column);
      } else if (c == ':') {
        CLOSE_TOKEN;
        SET_START;
        strcpy(buffer, "pi'e");
        process_word(buffer, start_line, start_column);
      } else {               
        switch (c) {
          case ' ':
          case '\t':
          case '\n':
          case '.':
          case '(':
          case EOF:
            CLOSE_TOKEN;
            if (c == '\n') {
              mark_eol();
              line++;
              column = 0;
            }
            if (c == '(') paren_depth = 1;
            if (c == EOF) goto done;
            break;
          default:
            SET_START;
            *p++ = c;
            break;
        }
      }
    }
  } while (1);

done:

  if (zoi_data) {
    fprintf(stderr, "Unclosed %s construction at end of file\n", zoi_form);
    had_bad_tokens = 1;
  }
  
  return;

}
