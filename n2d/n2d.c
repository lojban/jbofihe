/***************************************
  $Header$

  Main program for NFA to DFA table builder program.
  ***************************************/

/* COPYRIGHT */

/*
  Convert a nondeterminstic finite automaton (NFA) into a deterministic finite
  automaton (DFA).

  The NFA is defined in terms of a set of states, with transitions between the
  states.  The transitions may occur on any one of a set of symbols (specified
  with | characters between the options), or may be 'epsilon' transitions, i.e.
  occurring without consumption of any input.  A state may have multiple
  transitions for the same input symbol (hence 'nondeterministic').  The final
  state encountered within the final block defined in the input file is taken
  to be the start state of the whole NFA.  A state may be entered more than
  once in the file; the transitions in the multiple definitions are combined to
  give the complete transition set.  A state may have an exit value assigned
  (with =); this is the return value of the automaton if the end of string is
  encountered when in that state.  (If the resulting DFA can be in multiple
  exiting NFA states when the end of string is reached, the result is all the
  associated NFA exit values or'd together, so it is best to use distinct bits
  for NFA exit values unless it is known that is safe not to in a particular
  case.) The input grammar allows a BLOCK <name> ... ENDBLOCK construction +
  block instantiation.  This allows common parts of the NFA state machine to be
  reused in multiple places as well as aiding structuring and readability.  See
  morf_nfa.in for an example of the input grammar, and morf.c for a
  (non-trivial) example of how to build the automaton around the tables that
  this script generates.
*/

#include <ctype.h>
#include "n2d.h"

static Block **blocks = NULL;
static int nblocks = 0;
static int maxblocks = 0;

static char **toktable=NULL;
static int ntokens = 0;
static int maxtokens = 0;

struct Abbrev {
  char *lhs; /* Defined name */
  char **rhs; /* Token/define */
  int nrhs;
  int maxrhs;
};

static struct Abbrev *abbrevtable=NULL;
static int nabbrevs = 0;
static int maxabbrevs = 0;

static char *defresult = "0";

/* ================================================================= */

void
define_defresult(char *string)
{
  defresult = string;
}

/* ================================================================= */

static void
grow_tokens(void)
{
  maxtokens += 32;
  toktable = resize_array(char *, toktable, maxtokens);
}

/* ================================================================= */

static int
create_token(char *name)
{
  int result;
  if (ntokens == maxtokens) {
    grow_tokens();
  }
  result = ntokens++;
  toktable[result] = new_string(name);
  return result;
}

/* ================================================================= */

int
lookup_token(char *name, int create)
{
  int found = -1;
  int i;
  for (i=0; i<ntokens; i++) {
    if (!strcmp(toktable[i], name)) {
      found = i;
      break;
    }
  }

  switch (create) {
    case USE_OLD_MUST_EXIST:
      if (found < 0) {
        fprintf(stderr, "Token '%s' was never declared\n", name);
        exit(1);
      }        
      break;
    case CREATE_MUST_NOT_EXIST:
      if (found >= 0) {
        fprintf(stderr, "Token '%s' already declared\n", name);
        exit(1);
      } else {
        found = create_token(name);
      }
      break;
    case CREATE_OR_USE_OLD:
      if (found < 0) {
        found = create_token(name);
      }
      break;
  }
  
  return found;
}

/* ================================================================= */

static void
grow_abbrevs(void)
{
  maxabbrevs += 32;
  abbrevtable = resize_array(struct Abbrev, abbrevtable, maxabbrevs);
}

/* ================================================================= */

struct Abbrev *
create_abbrev(char *name)
{
  struct Abbrev *result;
  if (nabbrevs == maxabbrevs) {
    grow_abbrevs();
  }
  result = abbrevtable + (nabbrevs++);
  result->lhs = new_string(name);
  result->nrhs = result->maxrhs = 0;
  result->rhs = 0;
  return result;
}

/* ================================================================= */

void
add_tok_to_abbrev(struct Abbrev *abbrev, char *tok)
{
  if (abbrev->nrhs == abbrev->maxrhs) {
    abbrev->maxrhs += 8;
    abbrev->rhs = resize_array(char *, abbrev->rhs, abbrev->maxrhs);
  }

  abbrev->rhs[abbrev->nrhs++] = new_string(tok);
}

/* ================================================================= */

static struct Abbrev *
lookup_abbrev(char *name, int create)
{
  int found = -1;
  int i;
  struct Abbrev *result = NULL;
  /* Scan table in reverse order.  If a name has been redefined,
     make sure the most recent definition is picked up. */
  for (i=nabbrevs-1; i>=0; i--) {
    if (!strcmp(abbrevtable[i].lhs, name)) {
      found = i;
      result = abbrevtable + found;
      break;
    }
  }

  switch (create) {
    case CREATE_MUST_NOT_EXIST:
      if (found >= 0) {
        fprintf(stderr, "Abbreviation '%s' already declared\n", name);
        exit(1);
      } else {
        result = create_abbrev(name);
      }
      break;
    case CREATE_OR_USE_OLD:
      if (found < 0) {
        result = create_abbrev(name);
      }
      break;
  }
  
  return result;
}

/* ================================================================= */

static void
grow_blocks(void)
{
  maxblocks += 32;
  blocks = resize_array(Block*, blocks, maxblocks);
}

/* ================================================================= */

static Block *
create_block(char *name)
{
  Block *result;
  if (nblocks == maxblocks) {
    grow_blocks();
  }
  result = blocks[nblocks++] = new(Block);
  result->name = new_string(name);
  result->states = NULL;
  result->nstates = 0;
  result->maxstates = 0;
  result->subcount = 1;
  return result;
}

/* ================================================================= */


Block *
lookup_block(char *name, int create)
{
  Block *found = NULL;
  int i;
  for (i=0; i<nblocks; i++) {
    if (!strcmp(blocks[i]->name, name)) {
      found = blocks[i];
      break;
    }
  }

  switch (create) {
    case USE_OLD_MUST_EXIST:
      if (!found) {
        fprintf(stderr, "Could not find block '%s' to instantiate\n", name);
        exit(1);
      }        
      break;
    case CREATE_MUST_NOT_EXIST:
      if (found) {
        fprintf(stderr, "Already have a block called '%s', cannot redefine\n", name);
        exit(1);
      } else {
        found = create_block(name);
      }
      break;
    case CREATE_OR_USE_OLD:
      if (!found) {
        found = create_block(name);
      }
      break;
  }
  
  return found;
}

/* ================================================================= */
  
static void
grow_states(Block *b)
{
  b->maxstates += 32;
  b->states = resize_array(State*, b->states, b->maxstates);
}

/* ================================================================= */

static State *
create_state(Block *b, char *name)
{
  State *result;
  if (b->nstates == b->maxstates) {
    grow_states(b);
  }
  result = b->states[b->nstates++] = new(State);
  result->name = new_string(name);
  result->parent = b;
  result->index = b->nstates - 1;
  result->transitions = NULL;
  result->exitvals = NULL;
  result->ordered_trans = NULL;
  result->n_transitions = 0;
  result->removed = 0;
  return result;
}

/* ================================================================= */

State *
lookup_state(Block *b, char *name, int create)
{
  State *found = NULL;
  int i;
  for (i=0; i<b->nstates; i++) {
    if (!strcmp(b->states[i]->name, name)) {
      found = b->states[i];
      break;
    }
  }

  switch (create) {
    case USE_OLD_MUST_EXIST:
      if (!found) {
        fprintf(stderr, "Could not find a state '%s' in block '%s' to transition to\n", name, b->name);
        exit(1);
      }        
      break;
    case CREATE_MUST_NOT_EXIST:
      if (found) {
        fprintf(stderr, "Warning : already have a state '%s' in block '%s'\n", name, b->name);
      } else {
        found = create_state(b, name);
      }
      break;
    case CREATE_OR_USE_OLD:
      if (!found) {
        found = create_state(b, name);
      }
      break;
  }
  
  return found;
}

/* ================================================================= */
  
Stringlist *
add_token(Stringlist *existing, char *token)
{
  Stringlist *result = new(Stringlist);
  if (token) {
    result->string = new_string(token);
  } else {
    result->string = NULL;
  }
  result->next = existing;
  return result;
}

/* ================================================================= */
/* Add a single transition to the state.  Allow definitions to be
   recursive */

static void
add_transition(State *curstate, char *str, char *destination)
{
  struct Abbrev *abbrev;
  abbrev = (str) ? lookup_abbrev(str, USE_OLD_MUST_EXIST) : NULL;
  if (abbrev) {
    int i;
    for (i=0; i<abbrev->nrhs; i++) {
      add_transition(curstate, abbrev->rhs[i], destination);
    }
  } else {
    Translist *tl;
    tl = new(Translist);
    tl->next = curstate->transitions;
    /* No problem with aliasing, these strings are read-only and have
       lifetime = until end of program */
    tl->token = (str) ? lookup_token(str, USE_OLD_MUST_EXIST) : -1;
    tl->ds_name = destination;
    curstate->transitions = tl;
  }
}

/* ================================================================= */

void
add_transitions(State *curstate, Stringlist *tokens, char *destination)
{
  Stringlist *sl;
  struct Abbrev *abbrev;
  for (sl=tokens; sl; sl=sl->next) {
    add_transition(curstate, sl->string, destination);
  }
}

/* ================================================================= */

State *
add_transitions_to_internal(Block *curblock, State *addtostate, Stringlist *tokens)
{
  char buffer[1024];
  State *result;
  sprintf(buffer, "#%d", curblock->subcount++);
  result = lookup_state(curblock, buffer, CREATE_MUST_NOT_EXIST);
  add_transitions(addtostate, tokens, result->name);
  return result;
}


/* ================================================================= */

void
add_exit_value(State *curstate, char *value)
{
  Stringlist *sl;
  sl = new(Stringlist);
  sl->string = value;
  sl->next = curstate->exitvals;
  curstate->exitvals = sl;
}

/* ================================================================= */

void
instantiate_block(Block *curblock, char *block_name, char *instance_name)
{
  Block *master = lookup_block(block_name, USE_OLD_MUST_EXIST);
  char namebuf[1024];
  int i;
  for (i=0; i<master->nstates; i++) {
    State *s = master->states[i];
    State *new_state;
    Translist *tl;
    Stringlist *sl, *ex;
    
    strcpy(namebuf, instance_name);
    strcat(namebuf, ".");
    strcat(namebuf, s->name);
    
    /* In perverse circumstances, we might already have a state called this */
    new_state = lookup_state(curblock, namebuf, CREATE_OR_USE_OLD);
    
    for (tl=s->transitions; tl; tl=tl->next) {
      Translist *new_tl = new(Translist);
      new_tl->token = tl->token;
      strcpy(namebuf, instance_name);
      strcat(namebuf, ".");
      strcat(namebuf, tl->ds_name);
      new_tl->ds_name = new_string(namebuf);
      new_tl->ds_ref = NULL;
      new_tl->next = new_state->transitions;
      new_state->transitions = new_tl;
    }
    
    ex = NULL;
    for (sl=s->exitvals; sl; sl=sl->next) {
      Stringlist *new_sl = new(Stringlist);
      new_sl->string = sl->string;
      new_sl->next = ex;
      ex = new_sl;
    }
    new_state->exitvals = ex;
        
    
  }
}

/* ================================================================= */

void
fixup_state_refs(Block *b)
{
  int i;
  for (i=0; i<b->nstates; i++) {
    State *s = b->states[i];
    Translist *tl;
    for (tl=s->transitions; tl; tl=tl->next) {
      tl->ds_ref = lookup_state(b, tl->ds_name, CREATE_OR_USE_OLD);
    }
  }
}

/* ================================================================= */

/* Bitmap to contain epsilon closure for NFA */

static unsigned long **eclo;


/* ================================================================= */

static inline const int
round_up(const int x) {
  return (x+31)>>5;
}

/* ================================================================= */

static inline void
set_bit(unsigned long *x, int n)
{
  int r = n>>5;
  unsigned long m = 1UL<<(n&31);
  x[r] |= m;
}

/* ================================================================= */

static inline int
is_set(unsigned long *x, int n)
{
  int r = n>>5;
  unsigned long m = 1UL<<(n&31);
  return !!(x[r] & m);
}

/* ================================================================= */
/* During the algorithm to transitively close the epsilon closure table,
   maintain a stack of indices that have to be rescanned.  This avoids the slow
   approach of repeatedly rescanning the whole table until no changes are
   found. */

typedef struct IntPair {
  struct IntPair *next;
  int i;
  int j;
} IntPair;

static IntPair *freelist=NULL;
static IntPair *stack=NULL;

/* ================================================================= */

static void
push_pair(int i, int j)
{
  static const int grow_by = 32;
  IntPair *np;
  
  if (!freelist) {
    IntPair *ip = new_array(IntPair, grow_by);
    int x;
    for (x=1; x<grow_by; x++) {
      ip[x].next = &ip[x-1];
    }
    ip[0].next = NULL;
    freelist = &ip[grow_by-1];
  }
  np = freelist;
  freelist = freelist->next;
  np->next = stack;
  stack = np;
  np->i = i;
  np->j = j;
}


/* ================================================================= */

static int
pop_pair(int *i, int *j) {
  IntPair *ip;
  if (!stack) {
    return 0;
  } else {
    ip = stack;
    *i = ip->i;
    *j = ip->j;
    stack = ip->next;
    ip->next = freelist;
    freelist = ip;
    return 1;
  }
}

/* ================================================================= */

static void
generate_epsilon_closure(Block *b)
{
  int i, j, N;
  
  N = b->nstates;
  eclo = new_array(unsigned long*, N);
  for (i=0; i<N; i++) {
    eclo[i] = new_array(unsigned long, round_up(N));
    for (j=0; j<round_up(N); j++) {
      eclo[i][j] = 0;
    }
  }

  /* Determine initial immediate transitions */
  for (i=0; i<N; i++) {
    State *s = b->states[i];
    Translist *tl;
    int from_state = s->index;
    set_bit(eclo[from_state], from_state); /* Always reflexive */
    
    for (tl=s->transitions; tl; tl=tl->next) {
      if (tl->token < 0) { /* epsilon trans */
        int to_state = tl->ds_ref->index;
        set_bit(eclo[from_state], to_state);
        push_pair(from_state, to_state);
      }
    }
  }

  /* Now keep on processing until the table is transitively closed */
  while (pop_pair(&i, &j)) {
    int k;
    for (k=0; k<N; k++) {
      if (is_set(eclo[j], k) && !is_set(eclo[i], k)) {
        set_bit(eclo[i], k);
        push_pair(i,k);
      }
    }
  }
}

/* ================================================================= */

static void
print_nfa(Block *b)
{
  int i, j, N;
  N = b->nstates;
  for (i=0; i<N; i++) {
    State *s = b->states[i];
    Translist *tl;
    Stringlist *sl;
    fprintf(stderr, "NFA state %d = %s\n", i, s->name);
    for (tl=s->transitions; tl; tl=tl->next) {
      fprintf(stderr, "  [%s] -> %s\n",
              (tl->token >= 0) ? toktable[tl->token] : "(epsilon)",
              tl->ds_name);
    }
    if (s->exitvals) {
      int first = 1;
      fprintf(stderr, "  Exit value : ");
      for (sl=s->exitvals; sl; sl=sl->next) {
        fprintf(stderr, "%s%s",
                first ? "" : "|",
                s->exitvals->string);
      }
      fprintf(stderr, "\n");
    }
    fprintf(stderr, "  Epsilon closure :\n    (self)\n");
    for (j=0; j<N; j++) {
      if (i!=j && is_set(eclo[i], j)) {
        fprintf(stderr, "    %s\n", b->states[j]->name);
      }
    }
    
    fprintf(stderr, "\n");
  }

}

/* ================================================================= */

/* Indexed [from_state][token][to_state], flag set if there is
   a transition from from_state to to_state, via token then zero or more
   epsilon transitions */

static unsigned long ***transmap;

/* Index [from_nfa_state][token], flag set if there is a transition
   to any destination nfa state for that token. */
static unsigned long **anytrans;

/* ================================================================= */

static void
build_transmap(Block *b)
{
  int N = b->nstates;
  int Nt = ntokens;
  int i, j, k, m;
  
  transmap = new_array(unsigned long **, N);
  anytrans = new_array(unsigned long *, N);
  for (i=0; i<N; i++) {
    transmap[i] = new_array(unsigned long *, Nt);
    anytrans[i] = new_array(unsigned long, round_up(Nt));
    for (j=0; j<round_up(Nt); j++) {
      anytrans[i][j] = 0UL;
    }
    for (j=0; j<Nt; j++) {
      transmap[i][j] = new_array(unsigned long, round_up(N));
      for (k=0; k<round_up(N); k++) {
        transmap[i][j][k] = 0UL;
      }
    }
  }

  for (i=0; i<N; i++) {
    State *s = b->states[i];
    Translist *tl;
    for (tl=s->transitions; tl; tl=tl->next) {
      if (tl->token >= 0) {
        int dest = tl->ds_ref->index;
        for (m=0; m<round_up(N); m++) {
          unsigned long x = eclo[dest][m];
          transmap[i][tl->token][m] |= x;
          if (!!x) set_bit(anytrans[i], tl->token);
        }
      }
    }
  }

  
}

/* ================================================================= */

static DFANode **dfas;
static int ndfa=0;
static int maxdfa=0;

static int had_ambiguous_result = 0;

/* ================================================================= */

/* Implement an array of linked lists to access DFA states directly.  The
 * hashes are given by folding the signatures down to single bytes. */

struct DFAList {
  struct DFAList *next;
  DFANode *dfa;
};

#define DFA_HASHSIZE 256
static struct DFAList *dfa_hashtable[DFA_HASHSIZE];

/* ================================================================= */

static void
grow_dfa(void)
{ 
  maxdfa += 32;
  dfas = resize_array(DFANode*, dfas, maxdfa);
}

/* ================================================================= */

static unsigned long
fold_signature(unsigned long sig)
{
  unsigned long folded;
  folded = sig ^ (sig >> 16);
  folded ^= (folded >> 8);
  folded &= 0xff;
  return folded;
}


/* ================================================================= */
/* Simple linear search.  Use 'signatures' to get rapid rejection
   of any DFA state that can't possibly match */

static int
find_dfa(unsigned long *nfas, int N)
{
  int res=-1;
  int i, j;
  unsigned long signature = 0UL;
  unsigned long folded_signature;
  struct DFAList *dfal;

  for (j=0; j<round_up(N); j++) {
    signature ^= nfas[j];
  }
  folded_signature = fold_signature(signature);
  
  for(dfal=dfa_hashtable[folded_signature]; dfal; dfal = dfal->next) {
    DFANode *dfa = dfal->dfa;
    int matched;

    if (signature != dfa->signature) continue;
    
    matched=1;

    for (j=0; j<round_up(N); j++) {
      if (nfas[j] != dfa->nfas[j]) {
        matched = 0;
        break;
      }
    }
    if (matched) {
      return dfa->index;
    }
  }
  return -1;
}

/* ================================================================= */

static int
add_dfa(Block *b, unsigned long *nfas, int N, int Nt)
{
  int j;
  int result = ndfa;
  int had_exitvals;
  int this_result_unambiguous;
  
  Stringlist *ex;
  unsigned long signature = 0UL, folded_signature;
  struct DFAList *dfal;

  fprintf(stderr, "Adding DFA state %d\n", ndfa);
  fflush(stderr);
  
  if (maxdfa == ndfa) {
    grow_dfa();
  }

  dfas[ndfa] = new(DFANode);
  dfas[ndfa]->nfas = new_array(unsigned long, round_up(N));
  dfas[ndfa]->map = new_array(int, Nt);
  dfas[ndfa]->index = ndfa;
  dfas[ndfa]->defstate = -1;

  for (j=0; j<round_up(N); j++) {
    unsigned long x = nfas[j];
    signature ^= x;
    dfas[ndfa]->nfas[j] = x;
  }
  dfas[ndfa]->signature = signature;
  
  folded_signature = fold_signature(signature);
  dfal = new(struct DFAList);
  dfal->dfa = dfas[ndfa];
  dfal->next = dfa_hashtable[folded_signature];
  dfa_hashtable[folded_signature] = dfal;

  ex = NULL;
  had_exitvals = 0;
  clear_symbol_values();
  for (j=0; j<N; j++) {
    if (is_set(dfas[ndfa]->nfas, j)) {
      Stringlist *sl;
      State *s = b->states[j];
      for (sl = s->exitvals; sl; sl = sl->next) {
        Stringlist *new_sl;
        new_sl = new(Stringlist);
        new_sl->string = sl->string;
        new_sl->next = ex;
        ex = new_sl;

        set_symbol_value(sl->string);
        had_exitvals = 1;
      }
    }
  }
  
  this_result_unambiguous = evaluate_result(&dfas[ndfa]->result);
  dfas[ndfa]->nfa_sl = ex;

  if (!this_result_unambiguous) {
    Stringlist *sl;
    fprintf(stderr, "WARNING : Ambiguous exit state abandoned for DFA state %d\n", ndfa);
    fprintf(stderr, "NFA exit tags applying in this stage :\n");
    for (sl = ex; sl; sl = sl->next) {
      fprintf(stderr, "  %s\n", sl->string);
    }
    had_ambiguous_result = 1;
  }
        
  ndfa++;
  return result;
}

/* ================================================================= */

static void
clear_nfas(unsigned long *nfas, int N)
{
  int i;
  for (i=0; i<round_up(N); i++) {
    nfas[i] = 0;
  }
}

/* ================================================================= */

static void
build_dfa(Block *b, int start_index)
{
  unsigned long **nfas;
  int i;
  int N, Nt;
  int next_to_do;
  int *found_any;
  int rup_N;

  for (i=0; i<DFA_HASHSIZE; i++) dfa_hashtable[i] = NULL;
  
  N = b->nstates;
  rup_N = round_up(N);
  Nt = ntokens;
  
  /* Add initial state */
  nfas = new_array(unsigned long *, Nt);
  for (i=0; i<Nt; i++) {
    nfas[i] = new_array(unsigned long, round_up(N));
  }
  clear_nfas(nfas[0], N);
  for (i=0; i<round_up(N); i++) {
    nfas[0][i] |= eclo[start_index][i];
  }
  add_dfa(b, nfas[0], N, Nt);
  next_to_do = 0;
  found_any = new_array(int, Nt);

  /* Now the heart of the program : the subset construction to turn the NFA
     into a DFA.  This is a major performance hog in the program, so there are
     lots of tricks to speed this up (particularly, hoisting intermediate
     pointer computations out of the loop to assert the fact that there is no
     aliasing between the arrays.) */

  while (next_to_do < ndfa) {

    int t; /* token index */
    int j0, j0_5, j1, j, mask, k, m;
    int idx;
    unsigned long *current_nfas;
    unsigned long block_bitmap;
    
    for (j=0; j<Nt; j++) {
      clear_nfas(nfas[j], N);
      found_any[j] = 0;
    }

    current_nfas = dfas[next_to_do]->nfas;
    for (j0=0; j0<rup_N; j0++) { /* Loop over NFA states which may be in this DFA state */
      block_bitmap = current_nfas[j0];
      if (!block_bitmap) continue;
      j0_5 = j0 << 5;
      for (mask=1UL, j1=0; j1<32; mask<<=1, j1++) {
        j = j0_5 + j1;
        if (block_bitmap & mask) { /* Is NFA state in DFA */
          unsigned long **transmap_j = transmap[j];
          unsigned long *anytrans_j = anytrans[j];
          for (t=0; t<Nt; t++) { /* Loop over transition symbols */
            unsigned long *transmap_t;
            unsigned long *nfas_t;
            unsigned long found_any_t;
            if (!is_set(anytrans_j, t)) continue;
            transmap_t = transmap_j[t];
            nfas_t = nfas[t];
            found_any_t = found_any[t];
            for (k=0; k<rup_N; k++) { /* Loop over destination NFA states */
              unsigned long x;
              x = transmap_t[k];
              nfas_t[k] |= x;
              found_any_t |= !!x;
            }
            found_any[t] = found_any_t;
          }
        }
      }
    }
          
    for (t=0; t<Nt; t++) {
      if (found_any[t]) {
        idx = find_dfa(nfas[t], N);
        if (idx < 0) {
          idx = add_dfa(b, nfas[t], N, Nt);
        }
      } else {
        idx = -1;
      }
      dfas[next_to_do]->map[t] = idx;
    }

    next_to_do++;
  }

  free(found_any);
  for (i=0; i<Nt; i++) free(nfas[i]);
  free(nfas);
}

/* ================================================================= */

static void
print_dfa(Block *b)
{
  int N = b->nstates;
  int Nt = ntokens;
  
  int i, j, j0, j0_5, j1, t;
  unsigned long mask;
  unsigned long current_nfas;
  int rup_N = round_up(N);
  Stringlist *ex;
  
  for (i=0; i<ndfa; i++) {
    fprintf(stderr, "DFA state %d\n", i);
    if (dfas[i]->nfas) {
      fprintf(stderr, "  NFA states :\n");
      for (j0=0; j0<rup_N; j0++) {
        current_nfas = dfas[i]->nfas[j0];
        if (!current_nfas) continue;
        j0_5 = j0<<5;
        for (j1=0, mask=1UL; j1<32; mask<<=1, j1++) {
          if (current_nfas & mask) {
            fprintf(stderr, "    %s\n", b->states[j0_5 + j1]->name);
          }
        }
      }
      fprintf(stderr, "\n");
    }
    fprintf(stderr, "  Transitions :\n");
    for (t=0; t<Nt; t++) {
      int dest = dfas[i]->map[t];
      if (dest >= 0) {
        fprintf(stderr, "    %s -> %d\n", toktable[t], dest);
      }
    }
    if (dfas[i]->defstate >= 0) {
      fprintf(stderr, "  Use state %d as basis (%d fixups)\n",
              dfas[i]->defstate, dfas[i]->best_diff);
    }
    if (dfas[i]->result) {
      fprintf(stderr, "  Exit value : %s\n", dfas[i]->result);
    }
    
    fprintf(stderr, "\n");
  }
}

/* ================================================================= */
/* Emit the exit value table. */

static void
print_exitval_table(Block *b)
{
  int N = b->nstates;
  int Nt = ntokens;
  int n, i, j;
  extern char *prefix;
  char ucprefix[1024];

  if (prefix) {
    printf("static short %s_exitval[] = {\n", prefix);
  } else {
    printf("static short exitval[] = {\n");
  }
  for (i=0; i<ndfa; i++) {
    printf("%s", (dfas[i]->result) ? dfas[i]->result : defresult);
    putchar ((i<(ndfa-1)) ? ',' : ' ');
    printf(" /* State %d */\n", i);
  }
  printf("};\n\n");
}

/* ================================================================= */
/* Print out the state/transition table uncompressed, i.e. every
   token has an array entry in every state.  This is fast to access
   but quite wasteful on memory with many states and many tokens. */

static void
print_uncompressed_tables(Block *b)
{
  int N = b->nstates;
  int Nt = ntokens;
  int n, i, j;
  extern char *prefix;
  char ucprefix[1024];

  n = 0;
  if (prefix) {
    printf("static short %s_trans[] = {", prefix);
  } else {
    printf("static short trans[] = {");
  }
  for (i=0; i<ndfa; i++) {
    for (j=0; j<Nt; j++) {
      if (n>0) putchar (',');
      if (n%8 == 0) {
        printf("\n  ");
      } else {
        putchar(' ');
      }
      n++;
      printf("%4d", dfas[i]->map[j]);
    }
  }

  printf("\n};\n\n");

  if (prefix) {
    char *p;
    strcpy(ucprefix, prefix);
    for (p=ucprefix; *p; p++) {
      *p = toupper(*p);
    }
    printf("#define NEXT_%s_STATE(s,t) %s_trans[%d*(s)+(t)]\n",
           ucprefix, prefix, Nt);
  } else {
    printf("#define NEXT_STATE(s,t) trans[%d*(s)+(t)]\n", Nt);
  }
}

/* ================================================================= */

static int
check_include_char(int this_state, int token)
{
  if (dfas[this_state]->defstate >= 0) {
    return (dfas[this_state]->map[token] !=
            dfas[dfas[this_state]->defstate]->map[token]);
  } else {
    return (dfas[this_state]->map[token] >= 0);
  }
}

/* ================================================================= */
/* Print state/transition table in compressed form.  This is more
   economical on storage, but requires a bisection search to find
   the next state for a given current state & token */

static void
print_compressed_tables(Block *b)
{
  int N = b->nstates;
  int *basetab = new_array(int, ndfa+1);
  int Nt = ntokens;
  int n, i, j;
  extern char *prefix;


  n = 0;
  if (prefix) {
    printf("static unsigned char %s_token[] = {", prefix);
  } else {
    printf("static unsigned char token[] = {");
  }
  for (i=0; i<ndfa; i++) {
    for (j=0; j<Nt; j++) {
      if (check_include_char(i, j)) {
        if (n>0) putchar (',');
        if (n%8 == 0) {
          printf("\n  ");
        } else {
          putchar(' ');
        }
        n++;
        printf("%3d", j);
      }
    }
  }
  printf("\n};\n\n");

  n = 0;
  if (prefix) {
    printf("static short %s_nextstate[] = {", prefix);
  } else {
    printf("static short nextstate[] = {");
  }
  for (i=0; i<ndfa; i++) {
    basetab[i] = n;
    for (j=0; j<Nt; j++) {
      if (check_include_char(i, j)) {
        if (n>0) putchar (',');
        if (n%8 == 0) {
          printf("\n  ");
        } else {
          putchar(' ');
        }
        n++;
        printf("%5d", dfas[i]->map[j]);
      }
    }
  }
  printf("\n};\n\n");
  basetab[ndfa] = n;

  n = 0;
  if (prefix) {
    printf("static unsigned short %s_base[] = {", prefix);
  } else {
    printf("static unsigned short base[] = {");
  }
  for (i=0; i<=ndfa; i++) {
    if (n>0) putchar (',');
    if (n%8 == 0) {
      printf("\n  ");
    } else {
      putchar(' ');
    }
    n++;
    printf("%5d", basetab[i]);
  }
  printf("\n};\n\n");
  
  n = 0;
  if (prefix) {
    printf("static short %s_defstate[] = {", prefix);
  } else {
    printf("static short defstate[] = {");
  }
  for (i=0; i<ndfa; i++) {
    if (n>0) putchar (',');
    if (n%8 == 0) {
      printf("\n  ");
    } else {
      putchar(' ');
    }
    n++;
    printf("%5d", dfas[i]->defstate);
  }
  printf("\n};\n\n");

  
  free(basetab);
}

/* ================================================================= */

void yyerror (char *s)
{
  extern int lineno;
  fprintf(stderr, "%s at line %d\n", s, lineno);
}

/* ================================================================= */

int yywrap(void) { return -1; }

/* ================================================================= */

int main (int argc, char **argv)
{
  int result;
  State *start_state;
  Block *main_block;
  result = yyparse();
  if (result > 0) exit(1);

  start_state = get_curstate(); /* The last state to be current in the input file is the entry state of the NFA */
  main_block = start_state->parent;
  generate_epsilon_closure(main_block);
  print_nfa(main_block);
  compress_nfa(main_block);
  build_transmap(main_block);
  build_dfa(main_block, start_state->index);
  fprintf(stderr, "--------------------------------\n"
                  "DFA structure before compression\n"
                  "--------------------------------\n");
  print_dfa(main_block);
  
  ndfa = compress_dfa(dfas, ndfa, ntokens);

  compress_transition_table(dfas, ndfa, ntokens);

  fprintf(stderr, "-------------------------------\n"
                  "DFA structure after compression\n"
                  "-------------------------------\n");
  print_dfa(main_block);

  if (had_ambiguous_result) {
    fprintf(stderr, "No output written, there were ambiguous exit values for accepting states\n");
    exit(2);
  }
  
  print_exitval_table(main_block);
  print_compressed_tables(main_block);
#if 0
  print_uncompressed_tables(main_block);
#endif
  
  return result;
}
