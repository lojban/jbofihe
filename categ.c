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

#include <assert.h>
#include "nodes.h"
#include "rpc_tab.h"
#include "functions.h"
#include "stag_dfa.h"

static void insert_marker_after(TreeNode *where, int toktype, char *tokname)/*{{{*/
{
  TreeNode *marker;
  marker = new_node();
  marker->type = N_MARKER;
  marker->start_line = where->start_line;
  marker->start_column = where->start_column;
  marker->data.marker.tok = toktype;
  marker->data.marker.text = new_string(tokname);
  marker->prev = where;
  marker->next = where->next;
  where->next->prev = marker;
  where->next = marker;
}
/*}}}*/
static void insert_marker_before(TreeNode *where, int toktype, char *tokname)/*{{{*/
{
  TreeNode *marker;
  marker = new_node();
  marker->type = N_MARKER;
  marker->start_line = where->start_line;
  marker->start_column = where->start_column;
  marker->data.marker.tok = toktype;
  marker->data.marker.text = new_string(tokname);
  marker->next = where;
  marker->prev = where->prev;
  where->prev->next = marker;
  where->prev = marker;
}
/*}}}*/
static int cmtest(TreeNode *x, int type)/*{{{*/
{
  if ((x->type == N_CMAVO) &&
      (x->data.cmavo.selmao == type)) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/
static int mkrtest(TreeNode *x, int type)/*{{{*/
{
  if ((x->type == N_MARKER) &&
      (x->data.marker.tok == type)) {
    return 1;
  } else {
    return 0;
  }
}
/*}}}*/

static void categorize_jek(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, JA)) {
      /* Backup over any SE and NA preceding and insert PRIVATE_START_JEK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      if (cmtest(y, NA)) y = y->prev;
      /* Need to insert new token after y */
      insert_marker_after(y, PRIVATE_START_JEK, "PRIVATE_START_JEK");
    }

  }
  
}
/*}}}*/
static void categorize_gihek(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, GIhA)) {
      /* Backup over any SE and NA preceding and insert PRIVATE_START_GIHEK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      if (cmtest(y, NA)) y = y->prev;
      /* Need to insert new token after y */
      insert_marker_after(y, PRIVATE_START_GIHEK, "PRIVATE_START_GIHEK");
    }

  }
  
}
/*}}}*/
static void categorize_ek(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, A)) {
      /* Backup over any SE and NA preceding and insert PRIVATE_START_EK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      if (cmtest(y, NA)) y = y->prev;
      /* Need to insert new token after y */
      insert_marker_after(y, PRIVATE_START_EK, "PRIVATE_START_EK");
    }

  }
  
}
/*}}}*/
static void categorize_guhek(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, GUhA)) {
      /* Backup over any SE preceding and insert PRIVATE_START_GUHEK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      /* Need to insert new token after y */
      insert_marker_after(y, PRIVATE_START_GUHEK, "PRIVATE_START_GUHEK");
    }

  }
  
}
/*}}}*/
static void categorize_joik(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, JOI)) {
      /* Backup over any SE preceding and insert PRIVATE_START_JOIK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      insert_marker_after(y, PRIVATE_START_JOIK, "PRIVATE_START_JOIK");

      y = x->next;
      if (cmtest(y, NAI)) y = y->next;
      insert_marker_before(y, PRIVATE_END_JOIK, "PRIVATE_END_JOIK");

    } else if (cmtest(x, BIhI)) {
      /* Backup over any SE preceding and insert PRIVATE_START_JOIK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      if (cmtest(y, GAhO)) y = y->prev;
      insert_marker_after(y, PRIVATE_START_JOIK, "PRIVATE_START_JOIK");
      
      y = x->next;
      if (cmtest(y, NAI)) y = y->next;
      if (cmtest(y, GAhO)) y = y->next;
      insert_marker_before(y, PRIVATE_END_JOIK, "PRIVATE_END_JOIK");
    }
  }
  
}
/*}}}*/
static void categorize_gek(TreeNode *head) {/*{{{*/
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {

    if (cmtest(x, GA)) {
      /* Backup over any SE preceding and insert PRIVATE_START_GEK */
      y = x->prev;
      if (cmtest(y, SE)) y = y->prev;
      insert_marker_after(y, PRIVATE_START_GEK, "PRIVATE_START_GEK");

    } else if (cmtest(x, GI)) {
      
      if (mkrtest(x->prev, PRIVATE_END_TENSE)) {
        TreeNode *z;
        for (z=x->prev; z!=head; z=z->prev) {
          if (mkrtest(z, PRIVATE_START_TENSE)) {
            /* Insert START_GEK before z */
            insert_marker_before(z, PRIVATE_START_GEK, "PRIVATE_START_GEK");
            break;
          }
        }

      } else {
        /* categorize_joik is done before this, so just match the markers */
        TreeNode *z;
        z = x->prev;
        if (mkrtest(z, PRIVATE_END_JOIK)) {
          for (z=z->prev; z!=head; z=z->prev) {
            if (mkrtest(z, PRIVATE_START_JOIK)) {
              /* Insert START_GEK before z */
              insert_marker_before(z, PRIVATE_START_GEK, "PRIVATE_START_GEK");
              break;
            }
          }
        }
      }
    }
  }
}
/*}}}*/
static void categorize_nahe(TreeNode *head)/*{{{*/
{
  TreeNode *x;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, NAhE) && cmtest(x->next, BO)) {
      insert_marker_before(x, PRIVATE_NAhE_BO, "PRIVATE_NAhE_BO");
    }
  }
}
/*}}}*/

typedef enum BOKE_Lookahead {/*{{{*/
  FOUND_BO,
  FOUND_KE,
  FOUND_OTHER
} BOKE_Lookahead;
/*}}}*/
enum StagTokens {/*{{{*/
  /* **MUST** be sorted into same order as Tokens decl. in stag_nfa.in */
  ST_BAI,
  ST_BU,
  ST_BY,
  ST_CAhA,
  ST_CUhE,
  ST_FAhA,
  ST_FEhE,
  ST_FOI,
  ST_KI,
  ST_LAU,
  ST_MOhI,
  ST_NAhE,
  ST_NAI,
  ST_PA,
  ST_PU,
  ST_ROI,
  ST_SE,
  ST_TAhE,
  ST_TEI,
  ST_VA,
  ST_VEhA,
  ST_VIhA,
  ST_ZAhO,
  ST_ZEhA,
  ST_ZI
};
/*}}}*/
static int map_stag_token(int tok_in)/*{{{*/
{
  /* Convert global selma'o token numbers into the ones used by the stag DFA */
  switch(tok_in) {
    case BAI: return (int) ST_BAI;
    case BY: return (int) ST_BY;
    case CAhA: return (int) ST_CAhA;
    case CUhE: return (int) ST_CUhE;
    case FAhA: return (int) ST_FAhA;
    case FEhE: return (int) ST_FEhE;
    case FOI: return (int) ST_FOI;
    case KI: return (int) ST_KI;
    case LAU: return (int) ST_LAU;
    case MOhI: return (int) ST_MOhI;
    case NAhE: return (int) ST_NAhE;
    case NAI: return (int) ST_NAI;
    case PA: return (int) ST_PA;
    case PU: return (int) ST_PU;
    case ROI: return (int) ST_ROI;
    case SE: return (int) ST_SE;
    case TAhE: return (int) ST_TAhE;
    case TEI: return (int) ST_TEI;
    case VA: return (int) ST_VA;
    case VEhA: return (int) ST_VEhA;
    case VIhA: return (int) ST_VIhA;
    case ZAhO: return (int) ST_ZAhO;
    case ZEhA: return (int) ST_ZEhA;
    case ZI: return (int) ST_ZI;
    default: return -1;
  }
}
/*}}}*/
static void categorize_tenses(TreeNode *head)/*{{{*/
{
  TreeNode *x, *next_tok, *y, *last_ok_token;
  int curstate, nextstate, token;
  for (x=head->next; x!=head; x=next_tok) {
    curstate = 0;
    last_ok_token = NULL;
    for (y=x; y!= head; y=y->next) {
      token = (y->type == N_CMAVO) ? map_stag_token(y->data.cmavo.selmao) : 
              (y->type == N_BU) ? ST_BU : -1;
      nextstate = stag_next_state(curstate, token);
      if (nextstate >= 0) {
        if (stag_exitval[nextstate]) {
          last_ok_token = y;
        }
        curstate = nextstate;
      } else {
        if (last_ok_token) {
          /* Insert markers */
          insert_marker_before(x, PRIVATE_START_TENSE, "PRIVATE_START_TENSE");
          insert_marker_after(last_ok_token, PRIVATE_END_TENSE, "PRIVATE_END_TENSE");
          next_tok = last_ok_token->next->next; /* marker->next */
        } else {
          next_tok = x->next; /* Default case : failure */
        }
        goto next_outer_loop;
      }
    }
    if (y == head) {
      /* Run to end of text */
      insert_marker_before(x, PRIVATE_START_TENSE, "PRIVATE_START_TENSE");
      insert_marker_after(last_ok_token, PRIVATE_END_TENSE, "PRIVATE_END_TENSE");
      next_tok = last_ok_token->next->next; /* marker->next */
    }

next_outer_loop:
    (void) 0;
  }
}
/*}}}*/
static TreeNode *straddle_ek(TreeNode *x) /*{{{*/
{
  /* Expect to be looking at the start of an ek, then scan over it */
  if (mkrtest(x, PRIVATE_START_EK)) {
    x = x->next;
    if (cmtest(x, NA)) x = x->next;
    if (cmtest(x, SE)) x = x->next;
    assert(cmtest(x, A)); /* If this is wrong, categorize_ek is borked */
    x = x->next;
    if (cmtest(x, NAI)) return x->next;
    else                return x;
  } else {
    return NULL;
  }
}
/*}}}*/
static TreeNode *straddle_jek(TreeNode *x) /*{{{*/
{
  /* Expect to be looking at the start of a jek, then scan over it */
  if (mkrtest(x, PRIVATE_START_JEK)) {
    x = x->next;
    if (cmtest(x, NA)) x = x->next;
    if (cmtest(x, SE)) x = x->next;
    assert(cmtest(x, JA)); /* If this is wrong, categorize_jek is borked */
    x = x->next;
    if (cmtest(x, NAI)) return x->next;
    else                return x;
  } else {
    return NULL;
  }
}
/*}}}*/
static TreeNode *straddle_gihek(TreeNode *x) /*{{{*/
{
  /* Expect to be looking at the start of an gihek, then scan over it */
  if (mkrtest(x, PRIVATE_START_GIHEK)) {
    x = x->next;
    if (cmtest(x, NA)) x = x->next;
    if (cmtest(x, SE)) x = x->next;
    assert(cmtest(x, GIhA)); /* If this is wrong, categorize_gihek is borked */
    x = x->next;
    if (cmtest(x, NAI)) return x->next;
    else                return x;
  } else {
    return NULL;
  }
}
/*}}}*/
static TreeNode *straddle_joik(TreeNode *x)/*{{{*/
{
  /* It would be good to have some assertions in here.  Just rely on
   * categorize_joik doing its job. */
  if (mkrtest(x, PRIVATE_START_JOIK)) {
    TreeNode *z;
    for (z = x->next; ; z = z->next) {
      if (mkrtest(z, PRIVATE_END_JOIK)) return z->next;
    }
  } else {
    return NULL;
  }
}
/*}}}*/
static BOKE_Lookahead get_lookahead_result(TreeNode *x, TreeNode **end)/*{{{*/
{
  if (cmtest(x, BO)) {
    *end = x;
    return FOUND_BO;
  } else if (cmtest(x, KE)) {
    *end = x;
    return FOUND_KE;
  } else {
    *end = x;
    return FOUND_OTHER;
  }
}
/*}}}*/
static BOKE_Lookahead lookahead(TreeNode *start, TreeNode **end)/*{{{*/
{
  TreeNode *x, *y;
  /* Assume the initial connective has already been skipped in the caller. */
  x = start;
  while (mkrtest(x, PRIVATE_START_TENSE)) {
    do {
      x = x->next;
    } while (!mkrtest(x, PRIVATE_END_TENSE));
    /* Advance over jek or joik.  If not looking at connective, we've
     * fallen off the end of the construction and can check for BO or KE */
    x = x->next;
    if (mkrtest(x, PRIVATE_START_JEK)) y = straddle_jek(x);
    else if (mkrtest(x, PRIVATE_START_JOIK)) y = straddle_joik(x);
    else return get_lookahead_result(x, end);
    
    assert(y); /* We pre-checked the connective types before the straddle calls,
                  so y must be defined. */
    if (!mkrtest(y, PRIVATE_START_TENSE)) {
      /* the stag construct ran out at 'x'.  If we got here, 'x' must have started
       * a connective, so it's going to be a syntax error later, but certainly not
       * BO or KE */
      *end = x;
      return FOUND_OTHER;
    } else {
      /* Otherwise, set x to be start of next tense and iterate again. */
      x = y;
    }
  }
  /* Did not get a tense at the start of the scan */
  return get_lookahead_result(x, end);
}
/*}}}*/
static void categorize_ibo(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y, *z, *nx;

  for (x = head->next; x!=head; ) {
    if (cmtest(x, I)) {
      /* Checking for I [jek|joik] [stag] BO */
      y = x->next;
      z = straddle_jek(y);
      if (!z) z = straddle_joik(y);
      
      switch (lookahead(z?z:y, &nx)) {
        case FOUND_BO:
          insert_marker_before(x, PRIVATE_I_BO, "PRIVATE_I_BO");
          break;
        case FOUND_KE:
          /* Fall through - in this case we're not interested in whether KE follows */
        case FOUND_OTHER:
          break;
      }

      x = nx;

    } else {
      x = x->next;
    }
  }  
}
/*}}}*/
static void categorize_ijekjoik(TreeNode *head)/*{{{*/
{
  TreeNode *x;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, I)) {

      if (mkrtest(x->prev, PRIVATE_I_BO)) {
        /* Nothing to do  */
      } else {
        if (mkrtest(x->next, PRIVATE_START_JEK) ||
            mkrtest(x->next, PRIVATE_START_JOIK)) {
          insert_marker_before(x, PRIVATE_I_JEKJOIK, "PRIVATE_I_JEKJOIK");
        }
      }
    }
  }  
}
/*}}}*/
static void categorize_jek_kebo(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y, *nx;

  for (x = head->next; x!=head; ) {
    if (mkrtest(x, PRIVATE_START_JEK)) {

      if (cmtest(x->prev, I) || mkrtest(x->prev, PRIVATE_END_TENSE)) {
        /* Don't do anything , it's I je or I joik etc and inserting
           PRIVATE_JEK_KE tokens will screw up parsing of I construct */
        x = x->next;
      } else {
        y = straddle_jek(x);
        switch (lookahead(y, &nx)) {
          case FOUND_BO:
            insert_marker_before(x, PRIVATE_JEK_BO, "PRIVATE_JEK_BO");
            break;
          case FOUND_KE:
            insert_marker_before(x, PRIVATE_JEK_KE, "PRIVATE_JEK_KE");
            break;
          case FOUND_OTHER:
            break;
        }
        x = nx;
      }
    } else {
      x = x->next;
    }
  }  
}
/*}}}*/
static void categorize_ek_kebo(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y, *nx;

  for (x = head->next; x!=head; ) {
    if (mkrtest(x, PRIVATE_START_EK)) {
      y = straddle_ek(x);
      switch (lookahead(y, &nx)) {
        case FOUND_BO:
          insert_marker_before(x, PRIVATE_EK_BO, "PRIVATE_EK_BO");
          break;
        case FOUND_KE:
          insert_marker_before(x, PRIVATE_EK_KE, "PRIVATE_EK_KE");
          break;
        case FOUND_OTHER:
          break;
      }
      x = nx;
    } else {
      x = x->next;
    }
  }
}  
/*}}}*/
static void categorize_joik_kebo(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y, *nx;

  for (x = head->next; x!=head; ) {
    if (mkrtest(x, PRIVATE_START_JOIK)) {
      if (cmtest(x->prev, I) || mkrtest(x->prev, PRIVATE_END_TENSE)) {
        /* Don't do anything - this will screw up the parsing of the I
           construct */
        x = x->next;
      } else {
        y = straddle_joik(x);
        switch (lookahead(y, &nx)) {
          case FOUND_BO:
            insert_marker_before(x, PRIVATE_JOIK_BO, "PRIVATE_JOIK_BO");
            break;
          case FOUND_KE:
            insert_marker_before(x, PRIVATE_JOIK_KE, "PRIVATE_JOIK_KE");
            break;
          case FOUND_OTHER:
            break;
        }
        x = nx;
      }
    } else {
      x = x->next;
    }
  }
}  
/*}}}*/
static void categorize_gihek_kebo(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y, *nx;

  for (x = head->next; x!=head;) {
    if (mkrtest(x, PRIVATE_START_GIHEK)) {
      y = straddle_gihek(x);

      switch (lookahead(y, &nx)) {
        case FOUND_BO:
          insert_marker_before(x, PRIVATE_GIHEK_BO, "PRIVATE_GIHEK_BO");
          break;
        case FOUND_KE:
          insert_marker_before(x, PRIVATE_GIHEK_KE, "PRIVATE_GIHEK_KE");
          break;
        case FOUND_OTHER:
          break;
      }
      x = nx;
    } else {
      x = x->next;
    }
  }
}  
/*}}}*/
void categorize_number_mai(TreeNode *head) {/*{{{*/

  TreeNode *x, *y;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, MAI)) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) {
          goto done;
        } else if (y->type == N_MARKER) {
          break;
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
        insert_marker_after(y, PRIVATE_NUMBER_MAI, "PRIVATE_NUMBER_MAI");
      }
    }
  }
}
/*}}}*/
void categorize_number_roi(TreeNode *head) {/*{{{*/

  TreeNode *x, *y;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, ROI)) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) goto done;

        if (y->type == N_MARKER) {
          goto done;
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
        insert_marker_after(y, PRIVATE_NUMBER_ROI, "PRIVATE_NUMBER_ROI");
      }
    }
  }
}
/*}}}*/
void categorize_number_moi(TreeNode *head) {/*{{{*/

  TreeNode *x, *y;
  int count;

  for (x = head->next; x!=head; x=x->next) {
    if (cmtest(x, MOI)) {
      y = x->prev;
      count = 0;
      while (1) {
        if (y == head) goto done;

        if (y->type == N_MARKER) {
          break;
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
        insert_marker_after(y, PRIVATE_NUMBER_MOI, "PRIVATE_NUMBER_MOI");
      }
    }
  }
}
/*}}}*/
static void mark_cmavo_before_free(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO) {
      y = x->next;
      if (y->type == N_CMAVO) {
        switch (y->data.cmavo.selmao) {
          case SEI:
          case SOI:
          case COI:
          case DOI:
          case PRIVATE_NUMBER_MAI:
          case TO:
          case XI:
            x->data.cmavo.followed_by_free = 1;
            break;
          default:
            x->data.cmavo.followed_by_free = 0;
            break;
        }
      } else {
        x->data.cmavo.followed_by_free = 0;
      }
    }
  }
  return;

}
/*}}}*/
static void categorize_naku(TreeNode *head)/*{{{*/
{
  TreeNode *x, *y;

  for (x = head->next; x!=head; x=x->next) {
    if (x->type == N_CMAVO &&
        x->data.cmavo.selmao == NA) {
      y = x->next;
      if (y->type == N_CMAVO &&
          y->data.cmavo.selmao == KU) {
        
        insert_marker_before(x, PRIVATE_NA_KU, "PRIVATE_NA_KU");
      }
    }
  }  
}
/*}}}*/

void categorize_tokens(TreeNode *head)/*{{{*/
{
  categorize_tenses(head);
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
  categorize_nahe(head);
  categorize_naku(head);
  mark_cmavo_before_free(head);
}
/*}}}*/

