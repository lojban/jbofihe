/***************************************
  $Header$

  Read the input file and perform low-level lexing functions.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "cmavotab.h"
#include "nodes.h"
#include "functions.h"
#include "lujvofns.h"

static char zoi_form[8]; /* la'o or zoi */
static int  zoi_delim_next;
static char zoi_delim[64];
static int zoi_start_line, zoi_start_col;
static char *zoi_data;

/*++++++++++++++++++++++++++++++++++++++
  Are there any consonant clusters in the buffer?

  static int any_clusters

  char *buf
  ++++++++++++++++++++++++++++++++++++++*/

static int
any_clusters(char *buf)
{
  char *p;
  int last_was_cons = -1;
  for (p = buf;
       *p;
       p++) {
    if (is_consonant(*p)) {
      if (last_was_cons >= 0) {
        return last_was_cons;
      }
      last_was_cons = p - buf;
    } else if (is_vowel(*p)) {
      last_was_cons = -1;
    }
  }
   
  return -1;
  
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
  Take 1 cmavo off the front of a buffer and update the buffer pointer.

  char **bb Pointer to the buffer pointer.

  int start_line

  int start_column
  ++++++++++++++++++++++++++++++++++++++*/

static int
do_a_cmavo(char **bb, int start_line, int start_column) {
  char *b = *bb;
  char buf3[1024];
  char *p, *q;

  q = buf3;
  for (p = b;
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
      b = p + 1;
      break;
    }
  }

  *bb = b;
  return 1;
}


/*++++++++++++++++++++++++++++++++++++++
  Take the text in 'x' and add it as a single brivla to the token list.

  char *x

  int start_line

  int start_column
  ++++++++++++++++++++++++++++++++++++++*/

static void
add_brivla_token(char *x, int start_line, int start_column)
{
  TreeNode *tok;

  tok = new_node();
  tok->start_line = start_line;
  tok->start_column = start_column;
  tok->type = N_BRIVLA;
  /* Leave commas etc in for BRIVLA - you can use them in a
     fu'ivla? */
  tok->data.brivla.word = new_string(x);
  add_token(tok);


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
  char *p, *q;
  TreeNode *tok;

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
  /* Does word end in consonant (leave commas etc in for this)? */
  p = buf;
  while (*p) p++;
  p--;
  if (is_consonant(*p) || is_uppercase_consonant(*p)) {
    tok = new_node();
    tok->start_line = start_line;
    tok->start_column = start_column;
    tok->type = N_CMENE;
    tok->data.cmene.word = new_string(buf);
    add_token(tok);
  } else {
    int cluster_start;
    char *b2;

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
    b2 = buf2;
    
    cluster_start = any_clusters(buf2);
    if (cluster_start < 0) {
      while (*b2) {
        if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
      }
    } else {
      while (1) {
        switch (cluster_start) {
          case 0:
            add_brivla_token(b2, start_line, start_column);
            goto all_subtoks_done;
            break;
          case 1:
            /* Must be VCC..., i.e. something of selma'o A then a brivla */
            if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
            add_brivla_token(b2, start_line, start_column);
            goto all_subtoks_done;
            break;
          case 2:
            if (is_valid_lujvo(b2+2)) {
              do_a_cmavo(&b2, start_line, start_column);
            }              
            add_brivla_token(b2, start_line, start_column);
            goto all_subtoks_done;
            break;
          case 3:
            if (is_valid_lujvo(b2+3)) {
              do_a_cmavo(&b2, start_line, start_column);
            }              
            add_brivla_token(b2, start_line, start_column);
            goto all_subtoks_done;
            break;
          case 4:
            /* Take care with CVCV+lujvo/gismu, rather than the obvious
               CV'V+lujvo/gismu.  Also cope with CV+gismu, CV+CVCCVV,
               CV+CVCCCV.  Also, take care with attitudinal cmavo starting with
               a vowel appearing at the start.  */
            if (is_vowel(*b2) || is_cvc(b2)) {
              if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
              cluster_start = any_clusters(b2);
            } else if (is_cvv(b2)) {
              if (b2[2] == '\'') {
                if (is_valid_lujvo(b2+4)) {
                  if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
                }
                add_brivla_token(b2, start_line, start_column);
                goto all_subtoks_done;
              } else {
                if (is_valid_lujvo(b2+3)) {                  
                  if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
                }
                add_brivla_token(b2, start_line, start_column);
                goto all_subtoks_done;
              }
            } else {
              add_brivla_token(b2, start_line, start_column);
              goto all_subtoks_done;
            }
            break;
          default: /* at least 5 */
            if (!do_a_cmavo(&b2, start_line, start_column)) return 0;
            cluster_start = any_clusters(b2);
            break;
        }
      }

    all_subtoks_done:
      ;
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
  return;

}
