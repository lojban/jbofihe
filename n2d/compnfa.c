/***************************************
  $Header$

  Routines for compressing the NFA by commoning-up equivalent states
  ***************************************/

/* Copyright (C) Richard P. Curnow  2001 */
/* LICENCE */

/*
  Attempt to compress the NFA by finding 'equivalent' states.  Two states are
  'equivalent' if they have exactly the same set of transitions out of them
  (i.e. for all possible tokens, the destination state matches between them),
  _and_ they have identical exit values (if the end of string occurs whilst the
  automaton is in them.)  A state is eliminated by finding all transitions that
  go into it, and repointing them to the equivalent one.  It is fairly obvious
  that after eliminating some states this way, other states may become
  equivalent, so the process has to be repeated until no further eliminations
  occur.
  
  Note, later on there is a compression pass done on the resulting DFA, which
  would (I think) subsume what this block does in terms of the final DFA size.
  However, having NFA compression reduces the runtime of the subset
  construction, which is useful as that is a major proportion of the total
  execution time.  Also, this block was written before the DFA compressor came
  to be, and it's a shame to remove it.
  
  */

#include "n2d.h"

struct StateRec {
  int idx;
  unsigned long hash;
};

/**************************** Comparison function ****************************/

static int
compare_recs(const void *a, const void *b)
{
  const struct StateRec *aa = (const struct StateRec *) a;
  const struct StateRec *bb = (const struct StateRec *) b;

  if (aa->hash < bb->hash) return -1;
  else if (aa->hash > bb->hash) return +1;
  else if (aa->idx < bb->idx) return -1;
  else if (aa->idx > bb->idx) return +1;
  else return 0;
}

/********************* Calculate a hash for an NFA state *********************/

static unsigned long
calc_hash(State *s)
{
  unsigned long result = 0UL;
  Stringlist *sl;
  int i;
  char *p;
  
  for (i=0; i<s->n_transitions; i++) {
    Translist *tl = s->ordered_trans[i];
    unsigned long contrib = (((unsigned long) tl->ds_ref << 8) | (tl->token & 0xff));
    result = ((result << 7) | (result >> 25)) ^ contrib;
  }

  /* Don't bother about exit values, never try to common together states that
     can be accepting (note, these are likely to be sparse in the NFA state
     set, in contrast to the DFA state set where they may be plentiful). */

  return result;

}

/*** Comparison function for the sort to canonicalise the transition order ***/

static int
compare_transitions(const void *a, const void *b)
{
  const Translist *aa = *(const Translist **) a;
  const Translist *bb = *(const Translist **) b;

  if      (aa->token < bb->token) return -1;
  else if (aa->token > bb->token) return +1;
  else if (aa->ds_ref < bb->ds_ref) return -1;
  else if (aa->ds_ref > bb->ds_ref) return +1;
  else return 0;
}

/*********** Set up canonical transitions structure for all states ***********/

static void
canonicalise_transitions(Block *b)
{
  int N = b->nstates;
  int i, j;

  for (i=0; i<N; i++) {
    int nt=0;
    Translist *tl;
    State *s = b->states[i];
    
    for (tl=s->transitions; tl; tl=tl->next) nt++;

    s->n_transitions = nt;
    s->ordered_trans = new_array(Translist *, nt);
    for (j=0, tl=s->transitions; tl; j++, tl=tl->next) {
      s->ordered_trans[j] = tl;
    }
    qsort(s->ordered_trans, nt, sizeof(Translist *), compare_transitions);
  }
}


/******************** Replace an NFA state by another one ********************/

static void
squash_state (Block *b, State *squashed_state, State *repl_state)
{
  int N = b->nstates;
  int i,j;
  int touched;
  
  for (i=0; i<N; i++) {
    /* Redirect transitions in all states */
    State *s = b->states[i];
    touched = 0;
    for (j=0; j<s->n_transitions; j++) {
      Translist *tl = s->ordered_trans[j];
      if (tl->ds_ref == squashed_state) {
        touched = 1;
        tl->ds_ref = repl_state;
      }
    }
    if (touched) {
      /* Re-sort the transitions into canonical order, as the pointers
         affect the order for multiple transitions on the same token */
      qsort(s->ordered_trans, s->n_transitions, sizeof(Translist *), compare_transitions);
    }
  }

  squashed_state->removed = 1;

  return;
}

/******* Rigorous check whether 2 NFA states have same exit transitions *******/

static int
check_same_transitions(State *s1, State *s2)
{
  int ok = 1;

  int n1, n2, i;
  n1 = s1->n_transitions;
  n2 = s2->n_transitions;
  if (n1 != n2) {
    /* Not much point looking further! */
    return 0;
  }

  for (i=0; i<n1; i++) {
    Translist *tl1 = s1->ordered_trans[i];
    Translist *tl2 = s2->ordered_trans[i];
    if ((tl1->token != tl2->token) ||
        (tl1->ds_ref != tl2->ds_ref)) {
      ok = 0;
      break;
    }
  }

  return ok;
}


/********** Try removing NFA states, return whether any were removed **********/

static int
try_removals(Block *b, struct StateRec *recs)
{
  int N = b->nstates;
  int i, n;
  int base, here;
  int removed_any = 0;
  State *base_state, *here_state;

  n = 0;
  for (i=0; i<N; i++) {
    /* Need to cope with states eliminated last time */
    if (!b->states[i]->removed) {
      recs[n].idx = i;
      recs[n].hash = calc_hash(b->states[i]);
      n++;
    }
  }

  qsort(recs, n, sizeof(struct StateRec), compare_recs);

  base = 0;
  base_state = b->states[recs[base].idx];
  while ((base < n) && (base_state->exitvals)) {
    base++;
    base_state = b->states[recs[base].idx];
  }
  
  here = base + 1;
  here_state = b->states[recs[here].idx];
  
  while (here < n) {
    if (recs[base].hash == recs[here].hash) {
      if (!(here_state->exitvals) &&
          check_same_transitions(base_state, here_state)) {

        if (report) {
          fprintf(report, "NFA state '%s' replaced by equivalent '%s'\n",
                  here_state->name, base_state->name);
        }
        
        squash_state(b, here_state, base_state);
        removed_any = 1;
      }

      here++;
      here_state = b->states[recs[here].idx];
    } else {
      base = here;
      base_state = here_state;
      while ((base < n) && (base_state->exitvals)) {
        base++;
        base_state = b->states[recs[base].idx];
      }
      here = base + 1;
      here_state = b->states[recs[here].idx];
    }
  }

  return removed_any;
}

/************** Lexicographic comparison of states by exit value **************/

static int
compare_states_by_exitval(const void *a, const void *b)
{
  const State *aa = *(const State **) a;
  const State *bb = *(const State **) b;
  const char *aaa = aa->exitvals->string;
  const char *bbb = bb->exitvals->string;

  return strcmp(aaa, bbb);
}

/******* Try to squash accepting states that have the same result value *******/

static void
compress_accepting_states(Block *b)
{
  int N = b->nstates;
  State **ac_states = new_array(State *, N);
  int n, i;
  int base, here;
  const char *base_str, *here_str;
  
  for (i=n=0; i<N; i++) {
    State *s = b->states[i];
    if (s->exitvals) {
      if (s->transitions) {
        fprintf(stderr, "Internal error : Didn't expect to find an accepting NFA state with transitions\n");
        exit(2);
      }
      if (s->exitvals->next) {
        fprintf(stderr, "Internal error : Didn't expect to find an accepting NFA state with more than one result\n");
        exit(2);
      } 

      ac_states[n++] = s;
    }
  }

  /* Sort into ascending order */
  if (n > 1) {
    qsort(ac_states, n, sizeof(State *), compare_states_by_exitval);

    base = 0;
    base_str = ac_states[base]->exitvals->string;
    here = 1;
    while (here < n) {
      here_str = ac_states[here]->exitvals->string;
      if (!strcmp(base_str, here_str)) {
        squash_state(b, ac_states[here], ac_states[base]);
        if (report) {
          fprintf(report, "Replacing accepting state '%s' by '%s'\n",
                  ac_states[here]->name, ac_states[base]->name);
        }

      } else {
        base = here;
        base_str = here_str;
      }

      here++;
    }
  } 

  free(ac_states);
}

/************************** Main callable interface **************************/

/* 'b' is the top level block for the NFA, which defines all the states
   we need to look at. */

void
compress_nfa(Block *b)
{
  int N = b->nstates;
  struct StateRec *recs = new_array(struct StateRec, N);
  int pass, any_removed;

  canonicalise_transitions(b);
  compress_accepting_states(b);
  pass = 1;

  do {
    if (report) fprintf(report, "\nPass %d removing NFA states\n", pass);
    any_removed = try_removals(b, recs);
    if (!any_removed) {
      if (report) fprintf(report, "- none removed on this pass\n\n");
    }
    pass++;
  } while (any_removed);

  free(recs);
}
