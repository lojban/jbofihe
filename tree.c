/***************************************
  $Header$

  Functions to manipulate and print the parse tree.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nonterm.h"
#include "functions.h"
#include "cmavotab.h"
#include "rpc_tab.h"

/*++++++++++++++++++++++++++++++
  Work through the tree and eliminate any nodes that only have single
  children, by raising their children up to their status.
  ++++++++++++++++++++++++++++++*/

void
compress_singletons(TreeNode *x)
{
  int i, n;
  TreeNode *child, *grandchild;

  if (x->type == N_NONTERM) {
    n = x->data.nonterm.nchildren;
    for (i=0; i<n; i++) {
      child = x->data.nonterm.children[i];
      while ((child->type == N_NONTERM) &&
             (child->data.nonterm.nchildren == 1)) {
        grandchild = child->data.nonterm.children[0];
        free_node(child);
        child = x->data.nonterm.children[i] = grandchild;
      }
      compress_singletons(child);
    }
  }
}


/*++++++++++++++++++++++++++++++
  Work through the tree and eliminate any nodes that only have single
  children, by raising their children up to their status.  However,
  particular types of node are not compressed - those which are required
  for producing particular formatted outputs.
  ++++++++++++++++++++++++++++++*/

void
compress_singletons_but_preserve(TreeNode *x)
{
  int i, n;
  TreeNode *child, *grandchild;

  if (x->type == N_NONTERM) {
    n = x->data.nonterm.nchildren;
    for (i=0; i<n; i++) {
      child = x->data.nonterm.children[i];
      while ((child->type == N_NONTERM) &&
             (child->data.nonterm.nchildren == 1)) {
        grandchild = child->data.nonterm.children[0];
        free_node(child);
        child = x->data.nonterm.children[i] = grandchild;
      }
      compress_singletons(child);
    }
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
print_tree(TreeNode *x, int indent)
{
  int i, n;

  if (x->type == N_NONTERM) {
    n = x->data.nonterm.nchildren;
    for (i=0; i<n; i++) {
      print_tree(x->data.nonterm.children[i], indent+1);
    }
  }
  /* Print node itself */
  if (x->type != N_MARKER) {
    for (i=indent; i>0; i--) {
      if (i == 1) {
        printf("+-");
      } else {
        printf("| ");
      }
    }
  }
  switch (x->type) {
    case N_NONTERM:
      printf("%s\n", nonterm_names[x->data.nonterm.type]);
      break;

    case N_MARKER:
      break; /* No need to display */

    case N_CMAVO:
      printf("CMAVO : %s\n", cmavo_table[x->data.cmavo.code].cmavo);
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
      printf("BRIVLA : %s\n", x->data.brivla.word);
      break;

    case N_CMENE:
      printf("CMENE : %s\n", x->data.cmene.word);
      break;

    default:
      printf("OTHER?\n");
      break;
  }

}


/*++++++++++++++++++++++++++++++
  Give a rough print-out of the parse tree.
  ++++++++++++++++++++++++++++++*/

void
print_parse_tree(TreeNode *x)
{
  print_tree(x, 0);
}

/*++++++++++++++++++++++++++++++

  ++++++++++++++++++++++++++++++*/

typedef enum {
  B_OPEN,
  B_TEXT,
  B_CLOSE
} State;

#define SPACE_B4_TEXT \
    do { \
    if (*state == B_TEXT || *state == B_CLOSE) { \
      printf(" "); \
    } \
    *state = B_TEXT; \
    } while (0)

static int pending_eols = 0;

static int do_glossing = 0;

static void
clear_eols(State *state)
{
  int i;
  if (pending_eols > 0) {
    for (i=0; i<pending_eols; i++) {
      printf("\n");
    }
    *state = B_OPEN;
    pending_eols = 0;
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

typedef enum {
  CV_NORMAL,
  CV_2,
  CV_3,
  CV_4,
  CV_5
} Conversion;

static void
print_bracketed(TreeNode *x, int depth, State *state, Conversion conv)
{
  int i, n;
  TreeNode *y, *c1, *c2;
  Conversion child_conv;
  char *english;
  char buffer[2048];

  if (x->type == N_NONTERM) {
    clear_eols(state);
    if (*state == B_TEXT || *state == B_CLOSE) {
      printf(" ");
    }
    switch (depth % 4) {
      case 0: printf("(%d", depth/4); break;
      case 1: printf("["); break;
      case 2: printf("{"); break;
      case 3: printf("<"); break;
    }
    *state = B_OPEN;

    n = x->data.nonterm.nchildren;
    for (i=0; i<n; i++) {

      if (i > 0) {
        c1 = x->data.nonterm.children[i-1];
        c2 = x->data.nonterm.children[i];
        if (c1->type == N_CMAVO &&
            c1->data.cmavo.selmao == SE) {
          /* Attempt conversion of this value */
          if (!strcmp(cmavo_table[c1->data.cmavo.code].cmavo, "se")) {
            child_conv = CV_2;
          } else if (!strcmp(cmavo_table[c1->data.cmavo.code].cmavo, "te")) {
            child_conv = CV_3;
          } else if (!strcmp(cmavo_table[c1->data.cmavo.code].cmavo, "ve")) {
            child_conv = CV_4;
          } else if (!strcmp(cmavo_table[c1->data.cmavo.code].cmavo, "xe")) {
            child_conv = CV_5;
          } else {
            child_conv = CV_NORMAL;
          }

          print_bracketed(x->data.nonterm.children[i], depth + 1, state, child_conv);

        } else {
          print_bracketed(x->data.nonterm.children[i], depth + 1, state, CV_NORMAL);
        }
      } else {
        print_bracketed(x->data.nonterm.children[i], depth + 1, state, CV_NORMAL);
      }
    }

    switch (depth % 4) {
      case 0: printf(")%d",depth/4); break;
      case 1: printf("]"); break;
      case 2: printf("}"); break;
      case 3: printf(">"); break;
    }
    *state = B_CLOSE;

  } else {

    clear_eols(state);

    if (x->bahe ||
        (x->ui_next != (TreeNode *) &x->ui_next)) {

      if (*state == B_TEXT || *state == B_CLOSE) {
        printf(" ");
      }
      switch (depth % 4) {
        case 0: printf("(%d",depth/4); break;
        case 1: printf("["); break;
        case 2: printf("{"); break;
        case 3: printf("<"); break;
      }
      *state = B_OPEN;
    }

    if (x->bahe) {
      printf("%s", cmavo_table[x->bahe->data.cmavo.code].cmavo);
      *state = B_TEXT;
    }

    switch (x->type) {

      case N_CMAVO:
        SPACE_B4_TEXT;
        printf("%s", cmavo_table[x->data.cmavo.code].cmavo);
        if (do_glossing) {
          if (x->data.cmavo.selmao == BAI) {
            english = NULL;
            switch (conv) {
              case CV_NORMAL:
                english = translate(cmavo_table[x->data.cmavo.code].cmavo);
                break;
              case CV_2:
                strcpy(buffer, "se");
                strcat(buffer, cmavo_table[x->data.cmavo.code].cmavo);
                english = translate(buffer);
                break;
              case CV_3:
                strcpy(buffer, "te");
                strcat(buffer, cmavo_table[x->data.cmavo.code].cmavo);
                english = translate(buffer);
                break;
              case CV_4:
                strcpy(buffer, "ve");
                strcat(buffer, cmavo_table[x->data.cmavo.code].cmavo);
                english = translate(buffer);
                break;
              case CV_5:
                strcpy(buffer, "xe");
                strcat(buffer, cmavo_table[x->data.cmavo.code].cmavo);
                english = translate(buffer);
                break;
            }
            if (english) {
              printf(" /%s*/", english);
            }
          } else {
            english = translate(cmavo_table[x->data.cmavo.code].cmavo);
            if (english) {
              printf(" /%s/", english);
            }
          }
        }
        break;

      case N_ZOI:
        SPACE_B4_TEXT;
        printf("zoi %s %s %s", x->data.zoi.term, x->data.zoi.text, x->data.zoi.term);
        break;
        
      case N_ZO:
        SPACE_B4_TEXT;
        printf("zo %s", x->data.zo.text);
        break;

      case N_LOhU:
        if (*state == B_TEXT || *state == B_CLOSE) {
          printf(" ");
        }
        switch ((1+depth) % 4) {
          case 0: printf("(%d",(1+depth)/4); break;
          case 1: printf("["); break;
          case 2: printf("{"); break;
          case 3: printf("<"); break;
        }
        *state = B_OPEN;
        
        SPACE_B4_TEXT;
        printf("lo'u %s le'u", x->data.lohu.text);

        switch ((1+depth) % 4) {
          case 0: printf(")%d", (1+depth)/4); break;
          case 1: printf("]"); break;
          case 2: printf("}"); break;
          case 3: printf(">"); break;
        }
        *state = B_CLOSE;

        break;

      case N_BU:
        SPACE_B4_TEXT;
        printf("%s bu", x->data.bu.word);
        break;
        
      case N_BRIVLA:
        SPACE_B4_TEXT;
        printf("%s", x->data.brivla.word);
        if (do_glossing) {
          english = NULL;
          if (conv == CV_NORMAL) {
            english = translate(x->data.brivla.word);
            if (!english) {
              english = translate_unknown(x->data.brivla.word, 1);
            }
            if (english) {
              printf(" /%s/", english);
            }
          } else {
            switch (conv) {
              case CV_NORMAL:
                abort();
              case CV_2:
                strcpy(buffer, x->data.brivla.word);
                strcat(buffer, "2");
                english = translate(buffer);
                break;
              case CV_3:
                strcpy(buffer, x->data.brivla.word);
                strcat(buffer, "3");
                english = translate(buffer);
                break;
              case CV_4:
                strcpy(buffer, x->data.brivla.word);
                strcat(buffer, "4");
                english = translate(buffer);
                break;
              case CV_5:
                strcpy(buffer, x->data.brivla.word);
                strcat(buffer, "5");
                english = translate(buffer);
                break;
            }
            if (english) {
              printf(" /%s*/", english);
            } else {
              english = translate(x->data.brivla.word);
              if (!english) {
                english = translate_unknown(x->data.brivla.word, 1);
              }
              if (english) {
                printf(" /%s/", english);
              }
            }
          }
        }
        break;
        
      case N_CMENE:
        SPACE_B4_TEXT;
        printf("%s", x->data.cmene.word);

        break;
        
      default:
        break;
    }
    
    if (x->ui_next != (TreeNode *) &x->ui_next) {
      for (y=x->ui_next; y != (TreeNode *) &x->ui_next; y=y->next) {
        SPACE_B4_TEXT;
        printf("%s", cmavo_table[y->data.cmavo.code].cmavo);
      }
      *state = B_TEXT;
    }

    if (x->bahe ||
        (x->ui_next != (TreeNode *) &x->ui_next)) {

      switch (depth % 4) {
        case 0: printf(")%d",depth/4); break;
        case 1: printf("]"); break;
        case 2: printf("}"); break;
        case 3: printf(">"); break;
      }
      *state = B_CLOSE;
    }

    pending_eols = x->eols;

  }
}


/*++++++++++++++++++++++++++++++
  Print out the text with bracketing as per the LLG version
  ++++++++++++++++++++++++++++++*/

void
print_bracketed_text(TreeNode *top, int gloss)
{
  State state = B_OPEN;

  do_glossing = gloss;

  print_bracketed(top, 0, &state, CV_NORMAL);
  printf("\n");
}



/*++++++++++++++++++++++++++++++
  Go through the tree and find any nodes to which a BAhE or UI-chain
  were attached by the lexer.  Expand the node into a nonterminal and
  push all the things in as children.
  ++++++++++++++++++++++++++++++*/

static void
expand_bahe_ui_internal(TreeNode *x, int pos)
{
  TreeNode *nn, *y, *c;
  int n, i, j, n_ui, n_b, ui_start;

  y = x->data.nonterm.children[pos];

  if (y->type == N_NONTERM) {
    n = y->data.nonterm.nchildren;
    for (i=0; i<n; i++) {
      expand_bahe_ui_internal(y, i);
    }
  } else {
    if (y->bahe || (y->ui_next != (TreeNode *) &y->ui_next)) {
      nn = new_node();
      nn->type = N_NONTERM;

      if (y->bahe) {
        n_b = 1;
      } else {
        n_b = 0;
      }

      n_ui = 0;
      for (c=y->ui_next; c != (TreeNode *) &y->ui_next; c = c->next) {
        n_ui++;
      }

      nn->data.nonterm.type = AUGMENTED;
      nn->data.nonterm.brackets = BR_NONE;
      nn->data.nonterm.nchildren = n_b + 1 + n_ui;
      nn->data.nonterm.children = new_array(TreeNode *, nn->data.nonterm.nchildren);
      
      j = 0;

      if (y->bahe) {
        nn->data.nonterm.children[j++] = y->bahe;
        y->bahe = NULL;
      }

      nn->data.nonterm.children[j++] = y;

      ui_start = j;
      for (c=y->ui_next; c != (TreeNode *) &y->ui_next; c = c->next) {
        nn->data.nonterm.children[j++] = c;
      }

      y->ui_next = y->ui_prev = (TreeNode *) &y->ui_next;

      x->data.nonterm.children[pos] = nn;

        /* And handle the case that the UI chain itself may have BAhE
           tokens in front of some of its members */
      for (j=ui_start; j<nn->data.nonterm.nchildren; j++) {
        expand_bahe_ui_internal(nn, j);
      }

    }
  }

}

/*++++++++++++++++++++++++++++++
  Externally callable version
  ++++++++++++++++++++++++++++++*/

void
expand_bahe_ui(TreeNode *top)
{
  int i, n;
  
  if (top->type == N_NONTERM) {
    n = top->data.nonterm.nchildren;
    for (i=0; i<n; i++) {
      expand_bahe_ui_internal(top, i);
    }
  }
}

/*++++++++++++++++++++++++++++++

  ++++++++++++++++++++++++++++++*/

