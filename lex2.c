/***************************************
  $Header$

  Higher level lexing functions - group tokens together to implement
  functions below the level of the bison grammar.  Provide the yylex
  function.

  The order in which the lexing functions have to be performed comes
  from the top of grammar.300 - acknowledgements to the Logical
  Language Group who generated that file.

  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "functions.h"
#include "cmavotab.h"
#include "nodes.h"

#define YYSTYPE TreeNode *

#include "rpc.tab.h"

int last_tok_line;
int last_tok_column;

static TreeNode toks = {&toks,&toks};
static TreeNode *next_tok;

/*++++++++++++++++++++++++++++++++++++++
  Initialise the token list.
  ++++++++++++++++++++++++++++++++++++++*/

void
lex2_initialise(void)
{
  toks.next = toks.prev = &toks;
  next_tok = toks.next;
}


/*++++++++++++++++++++++++++++++++++++++
  Add a new token to the token stream parsed from the file.

  struct token *tok
  ++++++++++++++++++++++++++++++++++++++*/

void
add_token(TreeNode *node)
{
  node->next = &toks;
  node->prev = toks.prev;
  toks.prev->next = node;
  toks.prev = node;
}

/*++++++++++++++++++++++++++++++
  Increment number of EOLs following last token
  ++++++++++++++++++++++++++++++*/

void
mark_eol(void)
{
  ++toks.prev->eols;
}

/*++++++++++++++++++++++++++++++++++++++
  Delete a token
  ++++++++++++++++++++++++++++++++++++++*/

void
delete_node(TreeNode *x)
{
  /* Ought to release memory inside the node depending on type. */
  Free(x);

}

/*++++++++++++++++++++++++++++++++++++++
  Display sequence of tokens
  ++++++++++++++++++++++++++++++++++++++*/

void
show_tokens(void)
{
  TreeNode *x;
  int code;

  for (x=toks.next;
       x!=&toks;
       x=x->next) {
    
    switch (x->type) {

      case N_GARBAGE:
        printf("GAR : %s\n", x->data.garbage.word);
        break;

      case N_MARKER:
        printf("MAR : %s\n", x->data.marker.text);
        break;

      case N_CMAVO:
        code = x->data.cmavo.code;
        printf("CMV : %s [%s]\n", cmavo_table[code].cmavo, cmavo_table[code].meaning);
        break;

      case N_ZOI:
        printf("ZOI : %s\n", x->data.zoi.text);
        break;
        
      case N_ZO:
        printf("ZO  : %s\n", x->data.zo.text);
        break;

      case N_LOhU:
        printf("LOhU  : %s\n", x->data.lohu.text);
        break;
        
      case N_BU:
        printf("BU : %s\n", x->data.bu.word);
        break;

      case N_BRIVLA:
        printf("BRV : %s\n", x->data.brivla.word);
        break;

      case N_CMENE:
        printf("CMN : %s\n", x->data.cmene.word);
        break;
        
      case N_NONTERM:
        assert(0);
        break;

    }
    
  }

}

/*++++++++++++++++++++++++++++++
  Take an inclusive range of nodes and return a text string formed from them.
  Nodes should be primitive lexer tokens - can extend this later.
  ++++++++++++++++++++++++++++++*/

static char *
build_string_from_nodes(TreeNode *start, TreeNode *end)
{
  char *result;
  TreeNode *y;
  int len;

  len = 0;
  for (y=start; ; y = y->next) {
    switch (y->type) {
      case N_GARBAGE:
        len += strlen(y->data.garbage.word);
        break;
      case N_CMAVO:
        len += strlen(cmavo_table[y->data.cmavo.code].cmavo);
        break;
      case N_BRIVLA:
        len += strlen(y->data.brivla.word);
        break;
      case N_CMENE:
        len += strlen(y->data.cmene.word);
        break;
        
      case N_NONTERM:
      case N_MARKER:
      case N_ZOI:
      case N_ZO:
      case N_LOhU:
      case N_BU:
        assert(0);
        break;
        
    }

    if (y == end) break;
    
  }

  result = (char *) Malloc(1+len);
  result[0] = 0;
  for (y=start; ; y = y->next) {
    switch (y->type) {
      case N_GARBAGE:
        strcat(result, y->data.garbage.word);
        break;
      case N_CMAVO:
        strcat(result, cmavo_table[y->data.cmavo.code].cmavo);
        break;
      case N_BRIVLA:
        strcat(result, y->data.brivla.word);
        break;
      case N_CMENE:
        strcat(result, y->data.cmene.word);
        break;
        
      case N_NONTERM:
      case N_MARKER:
      case N_ZOI:
      case N_ZO:
      case N_LOhU:
      case N_BU:
        assert(0);
        break;
        
    }
    
    if (y == end) break;
    
  }
  
  return result;

}


/*++++++++++++++++++++++++++++++++++++++
  Do the ZOI processing
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_zoi(void)
{
  TreeNode *x, *delim, *start, *end, *cdelim, *node;

  for (x=toks.next;
       x!=&toks;
       ) {

    if ((x->type == N_CMAVO) &&
        cmavo_table[x->data.cmavo.code].selmao == ZOI) {

      /* Matched */
      delim = x->next;
      start = delim->next;
      if (delim == &toks || start == &toks) {
        fprintf(stderr, "EOF inside ZOI bracketing\n");
        exit(1); /* Ought to clean up better */
      }
      end = start;
      do {
        if (end == &toks) {
          fprintf(stderr, "EOF inside ZOI bracketing\n");
          exit(1); /* Ought to clean up better */
        }

        /* Try to match delimiter */
        if (((delim->type == N_CMAVO) &&
             (end->type == N_CMAVO) && 
             (end->data.cmavo.code == delim->data.cmavo.code)) ||
            
            ((delim->type == N_GARBAGE) &&
             (end->type == N_GARBAGE) &&
             (!strcmp(delim->data.garbage.word, end->data.garbage.word))) ||

            ((delim->type == N_BRIVLA) &&
             (end->type == N_BRIVLA) &&
             (!strcmp(delim->data.brivla.word, end->data.brivla.word))) ||

            ((delim->type == N_CMENE) &&
             (end->type == N_CMENE) &&
             (!strcmp(delim->data.cmene.word, end->data.cmene.word)))) {
          cdelim = end;
          end = end->prev;
            

          node = new_node();
          node->type = N_ZOI;
          node->data.zoi.term = new_string("'");
          node->data.zoi.text = build_string_from_nodes(start, end);

          /* Do list plumbing */
          x->type = node->type;
          x->data = node->data;
          x->next = cdelim->next;
          cdelim->next->prev = x;

          /* Just lose all the intervening nodes */
          break;
 
        } else {
          end = end->next;
        }

      } while(1);

    }

    x = x->next;
  }


}


/*++++++++++++++++++++++++++++++++++++++
  zo processing
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_zo(void)
{
  TreeNode *x, *y, *nt;

  for (x = toks.next;
       x != &toks;
       x = x->next) {
    
    if ((x->type == N_CMAVO) &&
        cmavo_table[x->data.cmavo.code].selmao == ZO) {

      y = x->next;
      if (y == &toks) {
        fprintf(stderr, "Cannot have ZO as the last token in the text\n");
        exit(1);
      }

      nt = new_node();
      nt->type = N_ZO;

      switch (y->type) {
        case N_CMAVO:
          nt->data.zo.text = new_string(cmavo_table[y->data.cmavo.code].cmavo);
          break;

        case N_GARBAGE:
          nt->data.zo.text = new_string(y->data.garbage.word);
          break;

        case N_BRIVLA:
          nt->data.zo.text = new_string(y->data.brivla.word);
          break;

        case N_CMENE:
          nt->data.zo.text = new_string(y->data.cmene.word);
          break;

        case N_ZOI:
          nt->data.zo.text = new_string(y->data.zoi.text);
          break;

        case N_NONTERM:
        case N_ZO:
        case N_LOhU:
        case N_MARKER:
        case N_BU:
          assert(0);
          break;

      }

      x->type = nt->type;
      x->data = nt->data;
      y->next->prev = x;
      x->next = y->next;

    }

  }
}


/*++++++++++++++++++++++++++++++++++++++
  lo'u processing
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_lohu(void)
{
  TreeNode *x, *y;
  TreeNode *start, *end, *term;

  for (x = toks.next;
       x != &toks;
       x = x->next) {

    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == LOhU) {
      y = start = x->next;
      do {
        if (y == &toks) {
          fprintf(stderr, "Unterminated LOhU .. LEhU construction\n");
          exit(1);
        }
        if (y->type == N_CMAVO &&
            y->data.cmavo.selmao == LEhU) {
          term = y;
          end = term->prev;
          break;
        }
        y = y->next;
      } while (1);

      x->type = N_LOhU;
      x->data.lohu.text = build_string_from_nodes(start, end);

      term->next->prev = x;
      x->next = term->next;

      /* Lose nodes in range start .. end */
    }
  }
}


/*++++++++++++++++++++++++++++++++++++++
  SI processing
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_si(void)
{
  TreeNode *x, *y, *nt;
  TreeNode *a;

  for (x = toks.next;
       x != &toks;
       x = nt) {

    nt = x->next;

    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == SI) {

      y = x->prev;

      if (y != &toks) {
        a = y->prev;
        a->next = nt;
        nt->prev = a;
        delete_node(x);
        delete_node(y);
      }
    }        
  }
}


/*++++++++++++++++++++++++++++++++++++++
  SU processing
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_su(void)
{
  /* Do later */

}

/*++++++++++++++++++++++++++++++
  ZEI processing.
  ++++++++++++++++++++++++++++++*/

static void
handle_zei(void)
{



}


/*++++++++++++++++++++++++++++++++++++++
  BAhE processing - look for any BAhE, and absorb it into the token
  that follows.
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_bahe(void)
{
  TreeNode *x, *y, *nt;

  for (x = toks.next;
       x != &toks;
       x = nt) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == BAhE) {
      
      y = x->next;
      nt = x->next;

      if ((y != &toks) &&
          !(y->type == N_CMAVO && y->data.cmavo.selmao == FAhO)) {
        y->bahe = x;
        /* Unlink x from the main token list */
        x->prev->next = y;
        y->prev = x->prev;
      } else {
        /* BAhE at end of text (EOF or FAhO) is an error */
      }
      
    } else {
      nt = x->next;
    }

  }

}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
handle_bu(void)
{
  TreeNode *x, *y, *nt;

  for (x = toks.next;
       x != &toks;
       x = nt) {

    nt = x->next;

    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == BU) {
      y = x->prev;
      x->type = N_BU;
      switch (y->type) {
        case N_CMAVO:
          x->data.bu.word = new_string(cmavo_table[y->data.cmavo.code].cmavo);
          break;

        case N_GARBAGE:
          x->data.bu.word = new_string(y->data.garbage.word);
          break;

        case N_BRIVLA:
          x->data.bu.word = new_string(y->data.brivla.word);
          break;

        case N_CMENE:
          x->data.bu.word = new_string(y->data.cmene.word);
          break;

        case N_ZOI:
          x->data.bu.word = new_string(y->data.zoi.text);
          break;

        case N_NONTERM:
        case N_ZO:
        case N_LOhU:
        case N_MARKER:
        case N_BU:
          assert(0);
          break;
      }

      /* Unlink y from the chain */
      y->prev->next = x;
      x->prev = y->prev;
      delete_node(y);
      
    }
  }
}


/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/


static int
is_indicator_cmavo(TreeNode *x)
{
  if (x->type == N_CMAVO) {
    if ((x->data.cmavo.selmao == UI) ||
        (x->data.cmavo.selmao == CAI) ||
        (x->data.cmavo.selmao == Y) ||
        (x->data.cmavo.selmao == DAhO) ||
        (x->data.cmavo.selmao == FUhO)) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
advance_indicator(TreeNode **x)
{
  while (is_indicator_cmavo(*x)) {
    (*x) = (*x)->next;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Go through looking for all UI and CAI cmavo.  If a NAI immediately
  follows, pull the NAI onto the 'parent' cmavo as a property.  If a
  CAI follows UI [NAI], pull that onto the UI as a property too.  If a
  NAI follows the CAI, discard it and give a warning - I don't know
  what that's supposed to mean at the moment.
  ++++++++++++++++++++++++++++++++++++++*/

static void
pair_off_indicator_suffixes(void)
{
  TreeNode *x, *y, *z, *w, *v, *nt;

  for (x = toks.next;
       x != &toks;
       x = nt) {

    y = x->next;

    if (is_indicator_cmavo(x)) {
      if ((y != &toks) &&
          (y->type == N_CMAVO) &&
          (y->data.cmavo.selmao == NAI)) {
        z = y->next;
        /* Create the property, we aren't interested in its body so
           void it */
        (void) prop_neg_indicator(x, YES);
        /* Drop the y node out */
        x->next = z;
        z->prev = x;
        /* y just dropped altogether */
        nt = z;

      } else {
        nt = y;
      }

      z = nt;
      
      /* See if CAI follows */
      if ((z != &toks) &&
          (z->type == N_CMAVO) &&
          (z->data.cmavo.selmao == CAI)) {
        char *tok;
        XCaiIndicator *xci;
        xci = prop_cai_indicator(x, YES);
        tok = cmavo_table[z->data.cmavo.code].cmavo;
        if (!strcmp(tok, "cai")) {
          xci->code = CC_CAI;
        } else if (!strcmp(tok, "sai")) {
          xci->code = CC_SAI;
        } else if (!strcmp(tok, "ru'e")) {
          xci->code = CC_RUhE;
        } else if (!strcmp(tok, "cu'i")) {
          xci->code = CC_CUhI;
        } else {
          abort();
        }
        
        w = z->next;
        /* Drop z */
        x->next = w;
        w->prev = x;
        nt = w;
        
        /* Handle NAI coming after CAI */
        if ((w != &toks) &&
            (w->type == N_CMAVO) &&
            (w->data.cmavo.selmao == NAI)) {
          switch (xci->code) {
            case CC_CAI:
              xci->code = CC_CAINAI;
              break;
            case CC_SAI:
              xci->code = CC_SAINAI;
              break;
            case CC_RUhE:
              xci->code = CC_RUhENAI;
              break;
            case CC_CUhI:
              /* Nothing to do, this is the neutral case so you
                 can't invert it */
              break;
            default:
              break;
          }
          
          /* Drop w */
          v = w->next;
          x->next = v;
          v->prev = x;
          nt = v;
        }
      }
    } else {
      nt = y;
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  Indicators processing.

  Look for any sequence matching the indicators non-terminal, and
  attach to the preceding node.
  ++++++++++++++++++++++++++++++++++++++*/

static void
handle_indicators(void)
{

  TreeNode *x, *nt, *target, *start, *end;
  int first=1;

  pair_off_indicator_suffixes();

  for (x = toks.next;
       x != &toks;
       x = nt) {

    if (!first &&
        is_indicator_cmavo(x)) {
      target = x->prev;
      start = x;
      advance_indicator(&x); /* So x now looks at the first token
                                beyond the end of the indicator string */
      nt = x;
      end = nt->prev;

      /* Unlink the indicator string from the main token list .. */
      nt->prev = target;
      target->next = nt;

      /* .. and link it onto the indicators list of the target */
      start->prev = end->next = (TreeNode *) &target->ui_next;
      target->ui_next = start;
      target->ui_prev = end;
    } else {
      nt = x->next;
    }

    first = 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Called to preprocess the token stream.  This involves processing the
  various erasure and quoting constructions, as well as marking certain
  tokens that need to cope with more than 1 token lookahead for the
  parser to work (notably things were KE and BO come several tokens
  later and are required to disambiguate constructs).
  ++++++++++++++++++++++++++++++++++++++*/

void
preprocess_tokens(void)
{

  /* 2a. Look for 'zoi'.  Following word becomes delimiter.  Search
     for matching delimiter.  Intervening text becomes semantic value.
     Remove intervening tokens from list.  Parser wants single token
     of selma'o ZOI. */

  handle_zoi();

  /* 2b. Look for 'zo' and group following word into it. */
  handle_zo();

  /* 2c. Look for lo'u ... le'u and eliminate internal data */
  handle_lohu();

  /* 2d. Done. */

  /* 2e. Remove any token followed by SI and the SI itself. */
  handle_si();

  /* 2f. SA - too vague, don't implement */

  /* 2g. Remove anything from SU backwards up to NIhO, LU, TUhE, TO inclusive */
  handle_su();

  handle_zei();

  handle_bahe();

  handle_bu();

  handle_indicators();

  categorize_tokens(&toks);

  return;
}

/*+ External variable referenced by parser. +*/
extern TreeNode *yylval;

extern YYLTYPE yylloc;

/*++++++++++++++++++++++++++++++
  Discard tokens after a parse error.  Advance to one of a designated
  set of tokens or to one of a fall-back set, whichever comes first.
  
  Unfortunately, it is practically impossible to use bison's automatic
  token discarding mechanism.  To make that work, you need to have rules
  of the form

  nonterm : thing TERMINATOR

  so that you can enhance this to

  nonterm : thing TERMINATOR
          | error TERMINATOR

  However, the Lojban grammar allows just about every terminator to be
  elided.  Having 'error' at the end of a rule causes Bison to reduce
  the rule, and then to just keep reducing to the outermost rule
  because the next (offending) token can usually not be shifted in any
  of the intermediate contexts.  This is obviously useless - you want
  to reject the smallest enclosing block of material around the error
  and try to resume parsing at the next sentence of whatever.

  The 'code' argument to this allows the elidable terminator to be
  supplied, so that it can be sought for if present and parsing can
  resume ASAP.

  ++++++++++++++++++++++++++++++*/

void
error_advance(int code)
{

  next_tok = next_tok->next;

  do {
    if ((next_tok == &toks) || /* End of file */
        ((next_tok->type == N_CMAVO || next_tok->type == N_MARKER) &&
         ((next_tok->data.cmavo.selmao == I) ||
          (next_tok->data.cmavo.selmao == I_BO) ||
          (next_tok->data.cmavo.selmao == NIhO) ||
          (next_tok->data.cmavo.selmao == LIhU) ||
          (next_tok->data.cmavo.selmao == TOI) ||
          (next_tok->data.cmavo.selmao == TUhU) ||
          (next_tok->data.cmavo.selmao == FAhO) ||
          ((code != 0) && (next_tok->data.cmavo.selmao == code))))) {
      break;
    }

    next_tok = next_tok->next;
    
  } while (1);

  next_tok = next_tok->prev;

}

/*++++++++++++++++++++++++++++++++++++++
  Look-alike for the yylex function

  int yylex
  ++++++++++++++++++++++++++++++++++++++*/


int
yylex(void)
{
  static int returned_EOF = 0;

  if (returned_EOF) {
    return 0;
  }

  next_tok = next_tok->next;

  if (next_tok == &toks) { /* End of file condition */
    returned_EOF = 1;
    return EOF_MARK;
  } else {
    /* Return a real token */

    last_tok_line = yylloc.first_line = next_tok->start_line;
    last_tok_column = yylloc.last_line = next_tok->start_line;
    yylloc.first_column = next_tok->start_column;
    yylloc.last_column = next_tok->start_column;

    switch (next_tok->type) {

      case N_MARKER:
        yylval = next_tok;
        return yylval->data.marker.tok;
        break;

      case N_GARBAGE:
        /* Needs its own parser token value */
        return GARBAGE;
        break;

      case N_CMAVO:
        if (next_tok->data.cmavo.selmao == FAhO) {
          returned_EOF = 1;
          return EOF_MARK;
        } else {
          yylval = next_tok;
          return yylval->data.cmavo.selmao;
        }
        break;

      case N_ZOI:
        yylval = next_tok;
        return ZOI;
        break;

      case N_ZO:
        yylval = next_tok;
        return ZO;
        break;

      case N_LOhU:
        yylval = next_tok;
        return LOhU;
        break;

      case N_BU:
        yylval = next_tok;
        return BU;
        break;

      case N_BRIVLA:
        yylval = next_tok;
        return BRIVLA;
        break;

      case N_CMENE:
        yylval = next_tok;
        return CMENE;
        break;

      case N_NONTERM:
        assert(0);
        break;
    }

  }

  assert(0);
  return 0;

}


/*++++++++++++++++++++++++++++++++++++++
  Print the last N tokens parsed
  ++++++++++++++++++++++++++++++++++++++*/

void
print_last_toks(void)
{
  TreeNode *x;
  int i, code;

  fprintf(stderr, "Last tokens parsed :\n");
  for (x = next_tok->prev, i = 0;
       i < 8 && x != &toks;
       i++, x = x->prev) {
    switch (x->type) {

      case N_GARBAGE:
        fprintf(stderr,"%s (line %d, col %d)\n", x->data.garbage.word, x->start_line, x->start_column);
        break;

      case N_MARKER:
        fprintf(stderr,"MARKER : %s\n", x->data.marker.text);
        break;

      case N_CMAVO:
        code = x->data.cmavo.code;
        fprintf(stderr,"%s (line %d, col %d)\n", cmavo_table[code].cmavo, x->start_line, x->start_column);
        break;

      case N_ZOI:
        fprintf(stderr,"zoi %s\n", x->data.zoi.text);
        break;
        
      case N_ZO:
        fprintf(stderr,"zo %s\n", x->data.zo.text);
        break;

      case N_LOhU:
        fprintf(stderr,"lo'u %s\n", x->data.lohu.text);
        break;

      case N_BU:
        fprintf(stderr,"bu %s\n", x->data.bu.word);
        break;
        
      case N_BRIVLA:
        fprintf(stderr,"%s (line %d, col %d)\n", x->data.brivla.word, x->start_line, x->start_column);
        break;

      case N_CMENE:
        fprintf(stderr,"%s (line %d, col %d)\n", x->data.cmene.word, x->start_line, x->start_column);
        break;

      case N_NONTERM:
        assert(0);
        break;

    }
  }

}
