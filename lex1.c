/***************************************
  $Header$

  Read the input file and perform low-level lexing functions.
  ***************************************/


#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cmavotab.h"
#include "nodes.h"
#include "functions.h"

/*++++++++++++++++++++++++++++++++++++++
  Check whether a character is a consonant.

  static int is_consonant

  char c
  ++++++++++++++++++++++++++++++++++++++*/

static int is_consonant(char c) {
  if (strchr("bcdfgjklmnprstvxz", c)) {
    return 1;
  } else {
    return 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Check whether a character is a vowel.

  static int is_vowel

  char c
  ++++++++++++++++++++++++++++++++++++++*/

static int is_vowel(char c) {
  if (strchr("aeiou", c)) {
    return 1;
  } else {
    return 0;
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Are there any consonant clusters in the buffer?

  static int any_clusters

  char *buf
  ++++++++++++++++++++++++++++++++++++++*/

static int
any_clusters(char *buf)
{
  char *p;
  int last_was_cons = 0;
  for (p = buf;
       *p;
       p++) {
    if (is_consonant(*p)) {
      if (last_was_cons) {
        return 1;
      }
      last_was_cons = 1;
    } else if (is_vowel(*p)) {
      last_was_cons = 0;
    }
  }
   
  return 0;
  
}

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
  v = tab1[*p - 'a'];
  if (v > 0) {
    val = v * 98;
    p++;
  }

  v = tab2[*p - 'a'];
  val += v * 14;
  p++;

  if (!*p) return val;

  if (*p == '\'') {
    val += 7;
    p++;
  }

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
    return;
  }
    
  hash = hash_cmavo(buf);

  if (hash < 0 || hash > 1750) {
    fprintf(stderr, "Bad cmavo [%s]\n", buf);
    return;
  }

  node = new_node();
  if (!strcmp(buf, cmavo_table[hash].cmavo)) {
    node->data.cmavo.code = hash;
    node->type = N_CMAVO;
    node->start_line = start_line;
    node->start_column = start_column;
    node->data.cmavo.selmao = cmavo_table[hash].selmao;
  } else {
    node->data.garbage.word = new_string(buf);
    node->type = N_GARBAGE;
    node->start_line = start_line;
    node->start_column = start_column;
  }    
  add_token(node);

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
  char buf2[1024], buf3[1024];
  char *p, *q;
  TreeNode *tok;


  /* Analyse word type */
  /* Does word end in consonant (leave commas etc in for this)? */
  p = buf;
  while (*p) p++;
  p--;
  if (is_consonant(*p)) {
    tok = new_node();
    tok->start_line = start_line;
    tok->start_column = start_column;
    tok->type = N_CMENE;
    tok->data.cmene.word = new_string(buf);
    add_token(tok);
  } else {

    /* Take out commas and other spurious characters and canonicalise to
       LC */
    for (p = buf, q = buf2;
         *p;
         p++) {
      if (isalpha((int) *p) || *p == '\'') {
        *q++ = tolower(*p);
      }
    }

    *q = 0;
    
    /* Any clusters? */
    
    if (any_clusters(buf2)) {
      tok = new_node();
      tok->start_line = start_line;
      tok->start_column = start_column;
      tok->type = N_BRIVLA;
      /* Leave commas etc in for BRIVLA - you can use them in a
         fu'ivla? */
      tok->data.brivla.word = new_string(buf);
      add_token(tok);
    } else {
      /* Break into individual cmavo */
      q = buf3;
      for (p = buf2;
           *p;
           p++) {
        *q++ = *p;
        if (is_consonant(p[1]) || !p[1]) {
          *q = 0;
          process_cmavo(buf3, start_line, start_column);
          if (!strcmp(buf3, "fa'o")) {
            /* End of stream token */
            return 0;
          }
          q = buf3;
        }
      }
    }
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
      } else if (c == '(') {
        paren_depth++;
      }
    } else {
      
      if (new_tok) {
        new_tok = 0;
        start_line = line;
        start_column = column;
      }
      if (c>='0' && c<='9') {
        static char *numbers[] = {"no", "pa", "re", "ci", "vo",
                                  "mu", "xa", "ze", "bi", "so"};
        CLOSE_TOKEN;
        strcpy(buffer, numbers[(int)(c-'0')]);
        process_word(buffer, start_line, start_column);
      } else if (c == ':') {
        CLOSE_TOKEN;
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
            *p++ = c;
            break;
        }
      }
    }
  } while (1);

done:
  return;

}
