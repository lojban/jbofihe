/***************************************
  $Header$

  Look for particular constructions in the token sequence and insert
  markers before them.  This provides the workaround to the lojban
  grammar not being purely LR(1).  e.g. when you see "gi'e", you have
  to know whether a "bo" follows later, maybe after a <stag>, to be
  able to decide whether to shift or reduce.  A mini-parser that
  recognizes <stag> is invoked from in here.
  ***************************************/

/* COPYRIGHT */

#include "nodes.h"
#include "rpc.tab.h"
#include "functions.h"
#include "stag.h"

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o JA and back up to where the 'jek'
  non-terminal will start.  Insert a START_JEK token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_jek(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == JA) {
      /* Backup over any SE and NA preceding and insert START_JEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NA) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_JEK;
      marker->data.marker.text = new_string("START_JEK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }

  }
  
}

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o GIhA and back up to where the 'gihek'
  non-terminal will start.  Insert a START_GIHEK token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_gihek(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == GIhA) {
      /* Backup over any SE and NA preceding and insert START_GIHEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NA) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_GIHEK;
      marker->data.marker.text = new_string("START_GIHEK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }

  }
  
}

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o A and back up to where the 'ek'
  non-terminal will start.  Insert a START_EK token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_ek(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == A) {
      /* Backup over any SE and NA preceding and insert START_EK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NA) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_EK;
      marker->data.marker.text = new_string("START_EK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }

  }
  
}

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o GUhA and back up to where the 'guhek'
  non-terminal will start.  Insert a START_GUHEK token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_guhek(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == GUhA) {
      /* Backup over any SE preceding and insert START_GUHEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER; 
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_GUHEK;
      marker->data.marker.text = new_string("START_GUHEK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }

  }
  
}

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o JOIK or BIhI and back up to where the
  'joik' non-terminal will start.  Insert a START_JOIK token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_joik(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == JOI) {
      /* Backup over any SE preceding and insert START_GUHEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_JOIK;
      marker->data.marker.text = new_string("START_JOIK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    } else if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == BIhI) {
      /* Backup over any SE preceding and insert START_GUHEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == GAhO) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_JOIK;
      marker->data.marker.text = new_string("START_JOIK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }
  }
  
}

/*++++++++++++++++++++++++++++++
  Look for cmavo of selma'o GI and GA and try to insert a START_GEK at
  the right place...
  ++++++++++++++++++++++++++++++*/

static void
categorize_gek(TreeNode *head) {
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {

    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == GA) {
      /* Backup over any SE preceding and insert START_GEK */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }

      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_GEK;
      marker->data.marker.text = new_string("START_GEK");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;

    } else if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == GI) {

      /* Try to back-up over 'joik' */
      int state;
      
      enum Tokens
      {T_OTHER=0, T_GAhO=1, T_NAI=2, T_JOI=3,
       T_BIhI=4, T_SE=5, T_START_JOIK=6} 
      tok;
            
      int states[7][11] =
      {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
       { 5, -1, -1, -1, -1, -1, -1, -1,  6,  6,  6},
       { 1, -1, -1, -1, -1,  7, -1, -1, -1, -1, -1},
       { 2,  2, -1, -1, -1,  8, -1,  8, -1, -1, -1},
       { 3,  3, -1, -1, -1,  9, -1,  9, -1, -1, -1},
       {-1, -1,  4,  4, -1, -1, -1, -1, 10, 10, -1},
       {-1, -1, -2, -2, -2, -1, -2, -1, -1, -1, -1}};

      state = 0;
      y = x->prev;
      
      do {

        if (y->type == N_CMAVO) {
          switch (y->data.cmavo.selmao) {
            case GAhO: tok = T_GAhO; break;
            case NAI: tok = T_NAI; break;
            case BIhI: tok = T_BIhI; break;
            case JOI: tok = T_JOI; break;
            case SE: tok = T_SE; break;
            default: tok = T_OTHER; break;
          }
        } else if (y->type == N_MARKER) {
          if (y->data.marker.tok == START_JOIK) {
            tok = T_START_JOIK;
          } else {
            tok = T_OTHER;
          }
        } else {
          tok = T_OTHER;
        }

#if 0
        printf("state=%d, token=%d\n", state, tok);
#endif

        state = states[tok][state];

#if 0
        printf("new state=%d\n", state);
#endif

        y = y->prev;

        if (state == -1) {
          y = NULL;
          break;
        }
        if (state == -2) {
          break;
        }

      } while (1);

      if (y != NULL) {
        marker = new_node();
        marker->type = N_MARKER;
        marker->start_line = y->start_line;
        marker->start_column = y->start_column;
        marker->data.marker.tok = START_GEK;
        marker->data.marker.text = new_string("START_GEK");
        marker->prev = y;
        marker->next = y->next;
        y->next->prev = marker;
        y->next = marker;
      }

    }
  }
}


/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o BAI and back up to where the 'stag'
  non-terminal will start.  Insert a START_BAI token there.
  ++++++++++++++++++++++++++++++*/

static void
categorize_bai(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == BAI) {
      /* Backup over any SE and NAhE preceding and insert START_BAI */
      y = x->prev;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == SE) {
        y = y->prev;
      }
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NAhE) {
        y = y->prev;
      }
      /* Need to insert new token after y */
      marker = new_node();
      marker->type = N_MARKER;
      marker->start_line = y->start_line;
      marker->start_column = y->start_column;
      marker->data.marker.tok = START_BAI;
      marker->data.marker.text = new_string("START_BAI");
      marker->prev = y;
      marker->next = y->next;
      y->next->prev = marker;
      y->next = marker;
    }

  }
  
}

/*++++++++++++++++++++++++++++++
  Look for any cmavo of selma'o NAhE and see whether one of a set of
  things follows it.  Insert marker before it appropriately.
  ++++++++++++++++++++++++++++++*/

static void
categorize_nahe(TreeNode *head)
{
  TreeNode *x, *y, *marker;
  enum {NEED_BO, NEED_TIME, NEED_SPACE, NEED_NONE} result;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == NAhE) {
      
      y = x->next;

      if (y->type == N_CMAVO) {
        switch (y->data.cmavo.selmao) {
          case BO:
            result = NEED_BO;
            break;
            
          case ZI:
          case PU:
          case ZEhA:
          case TAhE:
          case ZAhO:
            result = NEED_TIME;
            break;

          case VA:
          case FAhA:
          case VEhA:
          case VIhA:
          case FEhE:
          case MOhI:
            result = NEED_SPACE;
            break;

          default:
            result = NEED_NONE;
            break;
        }
      } else if (y->type == N_MARKER) {
        if (y->data.marker.tok == NUMBER_ROI) {
          result = NEED_TIME;
        } else {
          result = NEED_NONE;
        }
      } else {
        result = NEED_NONE;
      }

      if (result != NEED_NONE) {
        marker = new_node();
        marker->start_line = x->start_line;
        marker->start_column = x->start_column;
        marker->type = N_MARKER;
        switch (result) {
          case NEED_BO:
            marker->data.marker.tok = NAhE_BO;
            marker->data.marker.text = new_string("NAhE_BO");
            break;
          case NEED_TIME:
            marker->data.marker.tok = NAhE_time;
            marker->data.marker.text = new_string("NAhE_time");
            break;
          case NEED_SPACE:
            marker->data.marker.tok = NAhE_space;
            marker->data.marker.text = new_string("NAhE_space");
            break;
          case NEED_NONE:
            /* If prevents us getting here, but we put the branch in
               to shut the compiler up */
            break;
        }
        marker->next = x;
        marker->prev = x->prev;
        x->prev->next = marker;
        x->prev = marker;
      }
    }
  }
}


typedef enum {
  FOUND_BO,
  FOUND_KE,
  FOUND_OTHER
} BOKE_Lookahead;

static TreeNode *stag_marker;
static int stag_scan_complete;
static BOKE_Lookahead boke_look;

static int send_first_token;
static int first_token;

extern int stag_parse(void);

/*++++++++++++++++++++++++++++++++++++++
  Error printing function for stag parser

  char *s
  ++++++++++++++++++++++++++++++++++++++*/

void
stag_error(char *s)
{
#if 0
  fprintf(stderr, "%s\n", s);
  /* We're not interested in seeing errors from this, we expect there
     to be many of them */
#endif
}

/*++++++++++++++++++++++++++++++++++++++
  This is the 'yylex' function for the stag recognizer

  int staglex
  ++++++++++++++++++++++++++++++++++++++*/

int
stag_lex(void)
{

  int result;

  if (stag_scan_complete) {
    return 0;
  }

  if (send_first_token) {
    send_first_token = 0;
    return first_token;
  }
  
  /* No checking yet for whether we get to the end of the token
     list. */

  while (stag_marker->type != N_CMAVO) {
    stag_marker = stag_marker->next;
  }

  switch (stag_marker->data.cmavo.selmao) {
    case BO: result = STAG_BO; break;
    case KE: result = STAG_KE; break;
    case BAI: result = STAG_BAI; break;
    case BIhI: result = STAG_BIhI; break;
    case BU: result = STAG_BU; break;
    case BY: result = STAG_BY; break;
    case CAhA: result = STAG_CAhA; break;
    case CUhE: result = STAG_CUhE; break;
    case FAhA: result = STAG_FAhA; break;
    case FEhE: result = STAG_FEhE; break;
    case FOI: result = STAG_FOI; break;
    case GAhO: result = STAG_GAhO; break;
    case JA: result = STAG_JA; break;
    case JOI: result = STAG_JOI; break;
    case LAU: result = STAG_LAU; break;
    case KI: result = STAG_KI; break;
    case MOhI: result = STAG_MOhI; break;
    case NA: result = STAG_NA; break;
    case NAhE: result = STAG_NAhE; break;
    case NAI: result = STAG_NAI; break;
    case PA: result = STAG_PA; break;
    case PU: result = STAG_PU; break;
    case ROI: result = STAG_ROI; break;
    case SE: result = STAG_SE; break;
    case TAhE: result = STAG_TAhE; break;
    case TEI: result = STAG_TEI; break;
    case VA: result = STAG_VA; break;
    case VEhA: result = STAG_VEhA; break;
    case VIhA: result = STAG_VIhA; break;
    case ZAhO: result = STAG_ZAhO; break;
    case ZEhA: result = STAG_ZEhA; break;
    case ZI: result = STAG_ZI; break;
    default: result = STAG_OTHER; break;
  }

  stag_marker = stag_marker->next;

  return result;
  
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

void
stag_lookahead_bo (void)
{
  boke_look = FOUND_BO;
  stag_scan_complete = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

void
stag_lookahead_ke (void)
{
  boke_look = FOUND_KE;
  stag_scan_complete = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  

  static BOKE_Lookahead stag_scan

  TreeNode *x
  ++++++++++++++++++++++++++++++++++++++*/


static BOKE_Lookahead
stag_scan (TreeNode *x, int scan_type)
{
  stag_marker = x;
  stag_scan_complete = 0;
  send_first_token = 1;
  first_token = scan_type;
  if (stag_parse()) {
    return FOUND_OTHER;
  } else {
    return boke_look;
  }
}

/*++++++++++++++++++++++++++++++
  This function looks ahead across any intervening stag, to see
  whether there is a BO or KE following before the next construct.
  ++++++++++++++++++++++++++++++*/

static BOKE_Lookahead
lookahead(TreeNode *start, int scan_type)
{

  return stag_scan(start, scan_type);

#if 0
  do {
    if (start->type == N_CMAVO) {
      switch (start->data.cmavo.selmao) {
        case BO:
          return FOUND_BO;
          break;

        case KE:
          return FOUND_KE;
          break;
          
          /* This is the full closure of 'stag', augmented with gihek
             etc so that we can start scanning from START_GIHEK and so
             on, which is the point where we need the lookahead token
             for the grammar to be LR(1).

             */
        case BAI:
        case BIhI:
        case BU:
        case BY:
        case CAhA:
        case CUhE:
        case FAhA:
        case FEhE:
        case FOI:
        case GAhO:
        case JA:
        case JOI:
        case LAU:
        case KI:
        case MOhI:
        case NA:
        case NAhE:
        case NAI:
        case PA:
        case PU:
        case ROI:
        case SE:
        case TAhE:
        case TEI:
        case VA:
        case VEhA:
        case VIhA:
        case ZAhO:
        case ZEhA:
        case ZI:
          break; /* Nothing to do, iterate */

        default:
          /* Anything else means it's not a BO or KE lookahead
             construction */
          return FOUND_OTHER;
          break;
      }
    } else if (start->type == N_BRIVLA ||
               start->type == N_CMENE ||
               start->type == N_LOhU ||
               start->type == N_ZOI ||
               start->type == N_ZO ||
               start->type == N_GARBAGE) {
      /* Anything else means it's not a BO or KE lookahead
         construction */
      return FOUND_OTHER;
      
    }
    start = start->next;

  } while(1);
#endif
}

/*++++++++++++++++++++++++++++++
  Deal with inserting I_BO
  ++++++++++++++++++++++++++++++*/

static void
categorize_ibo(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == I) {
      y = x->next;
      switch (lookahead(y, JJ_STAG)) {
        case FOUND_BO:
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = x->start_line;
          marker->start_column = x->start_column;
          marker->data.marker.tok = I_BO;
          marker->data.marker.text = new_string("I_BO");
          /* Insert before x */
          marker->prev = x->prev;
          marker->next = x;
          x->prev->next = marker;
          x->prev = marker;
          break;
        case FOUND_KE:
          /* Fall through - in this case we're not interested in whether KE follows */
        case FOUND_OTHER:
          break;
      }

    }
  }  
}

/*++++++++++++++++++++++++++++++
  Deal with inserting I_BO
  ++++++++++++++++++++++++++++++*/

static void
categorize_ijekjoik(TreeNode *head)
{
  TreeNode *x, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if ((x->type == N_CMAVO) &&
        (x->data.cmavo.selmao == I)) {

      if ((x->prev->type == N_MARKER) &&
          (x->prev->data.marker.tok == I_BO)) {
        /* Nothing to do  */
      } else {
        if ((x->next->type == N_MARKER) &&
            ((x->next->data.marker.tok == START_JEK) ||
             (x->next->data.marker.tok == START_JOIK))) {
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = x->start_line;
          marker->start_column = x->start_column;
          marker->data.marker.tok = I_JEKJOIK;
          marker->data.marker.text = new_string("I_JEKJOIK");
          /* Insert before x */
          marker->prev = x->prev;
          marker->next = x;
          x->prev->next = marker;
          x->prev = marker;
        }
      }
    }
  }  
}

/*++++++++++++++++++++++++++++++
  Deal with inserting JEK_BO and JEK_KE
  ++++++++++++++++++++++++++++++*/

static void
categorize_jek_kebo(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_MARKER &&
        x->data.marker.tok == START_JEK) {

      if (x->prev->type == N_CMAVO &&
          x->prev->data.cmavo.selmao == I) {
        /* Don't do anything , it's I je or I joik etc and inserting
           JEK_KE tokens will screw up parsing of I construct */
      } else {
        y = x->next;
        while (y->type != N_CMAVO ||
               y->data.cmavo.selmao != JA) {
          y = y->next;
        }
        y = y->next;
        if (y->type == N_CMAVO &&
            y->data.cmavo.selmao == NAI) {
          y = y->next;
        }
        switch (lookahead(y, JUST_STAG)) {
          case FOUND_BO:
            marker = new_node();
            marker->type = N_MARKER;
            marker->start_line = x->start_line;
            marker->start_column = x->start_column;
            marker->data.marker.tok = JEK_BO;
            marker->data.marker.text = new_string("JEK_BO");
            /* Insert before x */
            marker->prev = x->prev;
            marker->next = x;
            x->prev->next = marker;
            x->prev = marker;
            break;
          case FOUND_KE:
            marker = new_node();
            marker->type = N_MARKER;
            marker->start_line = x->start_line;
            marker->start_column = x->start_column;
            marker->data.marker.tok = JEK_KE;
            marker->data.marker.text = new_string("JEK_KE");
            /* Insert before x */
            marker->prev = x->prev;
            marker->next = x;
            x->prev->next = marker;
            x->prev = marker;
            break;
          case FOUND_OTHER:
            break;
        }
      }
    }
  }  
}


/*++++++++++++++++++++++++++++++
  Deal with inserting EK_BO and EK_KE
  ++++++++++++++++++++++++++++++*/

static void
categorize_ek_kebo(TreeNode *head)
{
  TreeNode *x, *y, *z, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == A) {

      y = x->next;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NAI) {
        y = y->next;
      }
      z = x->prev;
      while (z->type != N_MARKER ||
             z->data.marker.tok != START_EK) {
        z = z->prev;
      }
      switch (lookahead(y, JUST_STAG)) {
        case FOUND_BO:
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = z->start_line;
          marker->start_column = z->start_column;
          marker->data.marker.tok = EK_BO;
          marker->data.marker.text = new_string("EK_BO");
          /* Insert before z */
          marker->prev = z->prev;
          marker->next = z;
          z->prev->next = marker;
          z->prev = marker;
          break;
        case FOUND_KE:
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = z->start_line;
          marker->start_column = z->start_column;
          marker->data.marker.tok = EK_KE;
          marker->data.marker.text = new_string("EK_KE");
          /* Insert before z */
          marker->prev = z->prev;
          marker->next = z;
          z->prev->next = marker;
          z->prev = marker;
          break;
        case FOUND_OTHER:
          break;
      }
    }
  }
}  

/*++++++++++++++++++++++++++++++
  Deal with inserting JOIK_BO and JOIK_KE
  ++++++++++++++++++++++++++++++*/

static void
categorize_joik_kebo(TreeNode *head)
{
  TreeNode *x, *y, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_MARKER &&
        x->data.marker.tok == START_JOIK) {

      if (x->prev->type == N_CMAVO &&
          x->prev->data.cmavo.selmao == I) {
        /* Don't do anything - this will screw up the parsing of the I
           construct */
      } else {
        y = x->next;
        switch (lookahead(y, JOIK_STAG)) {
          case FOUND_BO:
            marker = new_node();
            marker->type = N_MARKER;
            marker->start_line = x->start_line;
            marker->start_column = x->start_column;
            marker->data.marker.tok = JOIK_BO;
            marker->data.marker.text = new_string("JOIK_BO");
            /* Insert before x */
            marker->prev = x->prev;
            marker->next = x;
            x->prev->next = marker;
            x->prev = marker;
            break;
          case FOUND_KE:
            marker = new_node();
            marker->type = N_MARKER;
            marker->start_line = x->start_line;
            marker->start_column = x->start_column;
            marker->data.marker.tok = JOIK_KE;
            marker->data.marker.text = new_string("JOIK_KE");
            /* Insert before x */
            marker->prev = x->prev;
            marker->next = x;
            x->prev->next = marker;
            x->prev = marker;
            break;
          case FOUND_OTHER:
            break;
        }
      }
    }
  }
}  

/*++++++++++++++++++++++++++++++
  Deal with inserting GIHEK_BO and GIHEK_KE
  ++++++++++++++++++++++++++++++*/

static void
categorize_gihek_kebo(TreeNode *head)
{
  TreeNode *x, *y, *z, *marker;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == GIhA) {

      y = x->next;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == NAI) {
        y = y->next;
      }
      z = x->prev;
      while (z->type != N_MARKER ||
             z->data.marker.tok != START_GIHEK) {
        z = z->prev;
      }

      switch (lookahead(y, JUST_STAG)) {
        case FOUND_BO:
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = z->start_line;
          marker->start_column = z->start_column;
          marker->data.marker.tok = GIHEK_BO;
          marker->data.marker.text = new_string("GIHEK_BO");
          /* Insert before z */
          marker->prev = z->prev;
          marker->next = z;
          z->prev->next = marker;
          z->prev = marker;
          break;
        case FOUND_KE:
          marker = new_node();
          marker->type = N_MARKER;
          marker->start_line = z->start_line;
          marker->start_column = z->start_column;
          marker->data.marker.tok = GIHEK_KE;
          marker->data.marker.text = new_string("GIHEK_KE");
          /* Insert before z */
          marker->prev = z->prev;
          marker->next = z;
          z->prev->next = marker;
          z->prev = marker;
          break;
        case FOUND_OTHER:
          break;
      }
    }
  }
}  

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

void
categorize_number_mai(TreeNode *head) {

  TreeNode *x, *y, *marker;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == MAI) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) {
          goto done;
        } else if (y->type == N_MARKER) {
          y = y->prev;
        } else if (y->type == N_CMAVO) {
          switch (y->data.cmavo.selmao) {
            case PA:
            case BU:
            case BY:
            case LAU:
            case TEI:
            case FOI:
              count++;
              y = y->prev;
              break;
              
            default:
              goto done;
          }
        } else if (y->type == N_BU) {
          count++;
          y = y->prev;
        } else {
          goto done;
        }
      }

        done:

      if (count > 0) {
        marker = new_node();
        marker->type = N_MARKER;
        marker->start_line = y->start_line;
        marker->start_column = y->start_column;
        marker->data.marker.tok = NUMBER_MAI;
        marker->data.marker.text = new_string("NUMBER_MAI");
        marker->prev = y;
        marker->next = y->next;
        y->next->prev = marker;
        y->next = marker;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++
  Find each ROI with at least one lerfu_word or PA before it.
  Backtrack over the sequence of such tokens and insert NUMBER_ROI at
  the start.
  ++++++++++++++++++++++++++++++*/

void
categorize_number_roi(TreeNode *head) {

  TreeNode *x, *y, *marker;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == ROI) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) goto done;

        if (y->type == N_MARKER) {
          y = y->prev;
        } else if (y->type == N_CMAVO) {
          switch (y->data.cmavo.selmao) {
            case PA:
            case BU:
            case BY:
            case LAU:
            case TEI:
            case FOI:
              count++;
              y = y->prev;
              break;
              
            default:
              goto done;
          }
        } else {
          goto done;
        }
      }

        done:

      if (count > 0) {
        marker = new_node();
        marker->type = N_MARKER;
        marker->start_line = y->start_line;
        marker->start_column = y->start_column;
        marker->data.marker.tok = NUMBER_ROI;
        marker->data.marker.text = new_string("NUMBER_ROI");
        marker->prev = y;
        marker->next = y->next;
        y->next->prev = marker;
        y->next = marker;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++
  Find each instance of MOI which has at least one lerfu_word or PA
  token before it.  Backtrack over the sequence of such tokens and
  insert NUMBER_MOI at the start.
  ++++++++++++++++++++++++++++++*/

void
categorize_number_moi(TreeNode *head) {

  TreeNode *x, *y, *marker;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == MOI) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) goto done;

        if (y->type == N_MARKER) {
          y = y->prev;
        } else if (y->type == N_CMAVO) {
          switch (y->data.cmavo.selmao) {
            case PA:
            case BU:
            case BY:
            case LAU:
            case TEI:
            case FOI:
              count++;
              y = y->prev;
              break;
              
            default:
              goto done;
          }
        } else {
          goto done;
        }
      }

        done:

      if (count > 0) {
        marker = new_node();
        marker->type = N_MARKER;
        marker->start_line = y->start_line;
        marker->start_column = y->start_column;
        marker->data.marker.tok = NUMBER_MOI;
        marker->data.marker.text = new_string("NUMBER_MOI");
        marker->prev = y;
        marker->next = y->next;
        y->next->prev = marker;
        y->next = marker;
      }
    }
  }
}

/*++++++++++++++++++++++++++++++++++++++
  This function looks at particular types of token and makes them more
  specific depending on what comes further on in the token stream.
  ++++++++++++++++++++++++++++++++++++++*/

void
categorize_tokens(TreeNode *head)
{
  categorize_jek(head);
  categorize_gihek(head);
  categorize_ek(head);
  categorize_guhek(head);
  categorize_joik(head);
  categorize_gek(head);
  categorize_ibo(head);
  categorize_ijekjoik(head);
  categorize_jek_kebo(head);
  categorize_ek_kebo(head);
  categorize_joik_kebo(head);
  categorize_gihek_kebo(head);
  categorize_number_mai(head);
  categorize_number_roi(head);
  categorize_number_moi(head);
  categorize_bai(head);
  categorize_nahe(head);
}
