/***************************************
  $Header$

  Routines for compressing the NFA by commoning-up equivalent states
  ***************************************/

/* Copyright (C) Richard P. Curnow  2001 */
/* LICENCE */

/*
  Handle boolean expressions used to determine the final scanner result
  from the set of NFA accepting states that are simultaneously active
  at the end of the scan.
*/

#include "n2d.h"

enum ExprType {
  E_AND, E_OR, E_XOR, E_COND, E_NOT, E_WILD, E_SYMBOL
};

struct Symbol;

struct Expr {
  enum ExprType type;
  union {
    struct { struct Expr *c1, *c2; } and; 
    struct { struct Expr *c1, *c2; } or; 
    struct { struct Expr *c1, *c2; } xor; 
    struct { struct Expr *c1, *c2, *c3; } cond; 
    struct { struct Expr *c1; } not; 
    struct { int pad; } wild; 
    struct { struct Symbol *s; } symbol;
  } data;
};

struct Symbol {
  char *name;
  int is_expr;
  union {
    Expr *e;
    int val;
  } data;
};

struct SymbolList {
  struct SymbolList *next;
  struct Symbol *sym;
};

struct Result {
  char *result; /* The string to write to the output file */
  Expr *e;
};

typedef struct Result Result;
typedef struct Symbol Symbol;
typedef struct SymbolList SymbolList;

static SymbolList *symbols = NULL;

static Result *results = NULL;
static int n_results = 0;
static int max_results = 0;

/*++++++++++++++++++++
  ++++++++++++++++++++*/

static void
add_new_symbol(Symbol *s)
{
  SymbolList *nsl = new(SymbolList);
  nsl->sym = s;
  nsl->next = symbols;
  symbols = nsl;
}
  
/*++++++++++++++++++++
  ++++++++++++++++++++*/
static void
grow_results(void)
{
  if (n_results == max_results) {
    max_results += 32;
    results = resize_array(Result, results, max_results);
  }
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_wild_expr(void)
{
  Expr *r = new(Expr);
  r->type = E_WILD;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_not_expr(Expr *c)
{
  Expr *r = new(Expr);
  r->type = E_NOT;
  r->data.not.c1 = c;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_and_expr(Expr *c1, Expr *c2)
{
  Expr *r = new(Expr);
  r->type = E_AND;
  r->data.and.c1 = c1;
  r->data.and.c2 = c2;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_or_expr(Expr *c1, Expr *c2)
{
  Expr *r = new(Expr);
  r->type = E_OR;
  r->data.or.c1 = c1;
  r->data.or.c2 = c2;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_xor_expr(Expr *c1, Expr *c2)
{
  Expr *r = new(Expr);
  r->type = E_XOR;
  r->data.xor.c1 = c1;
  r->data.xor.c2 = c2;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
Expr *
new_cond_expr(Expr *c1, Expr *c2, Expr *c3)
{
  Expr *r = new(Expr);
  r->type = E_COND;
  r->data.cond.c1 = c1;
  r->data.cond.c2 = c2;
  r->data.cond.c3 = c3;
  return r; 
}

/*++++++++++++++++++++
  ++++++++++++++++++++*/
static Symbol *  
find_symbol_or_create(char *sym_name)
{
  int i;
  Symbol *s;
  SymbolList *sl;
  for (sl=symbols; sl; sl=sl->next) {
    s = sl->sym;
    if (!strcmp(s->name, sym_name)) {
      return s;
    }
  }
  
  s = new(Symbol);
  add_new_symbol(s);
  s->is_expr = 0; /* Until proven otherwise */
  s->name = new_string(sym_name);
  return s;
}

/*****************************************************************/
/* Return expr for symbol name if it already exist, else create. */
/*****************************************************************/

Expr *
new_sym_expr(char *sym_name)
{
  Expr *r;
  Symbol *s;

  s = find_symbol_or_create(sym_name);
  
  r = new(Expr);
  r->type = E_SYMBOL;
  r->data.symbol.s = s;
  return r; 
}

/*++++++++++++++++++++
  Add a result defn.  If the expr is null, it means build a single expr corr.
  to the value of the symbol with the same name as the result string.
  ++++++++++++++++++++*/

void
define_result(char *string, Expr *e)
{
  int i;
  Result *r;

#if 0
  for (i=0; i<n_results; i++) {
    if (!strcmp(string, results[i].result)) {
      fprintf(stderr, "Cannot have multiple definition for result '%s'\n", string);
      exit(2);
    }
  }
#endif

  grow_results();
  r = &results[n_results++];
  r->result = new_string(string);
  if (e) {
    r->e = e;
  } else {
    Expr *ne;
    ne = new_sym_expr(string);
    r->e = ne;
  }

  return;
}

/*++++++++++++++++++++
  Define an entry in the symbol table.
  ++++++++++++++++++++*/
  
void
define_symbol(char *name, Expr *e)
{
  Symbol *s;
  s = find_symbol_or_create(name);
  s->data.e = e;
  s->is_expr = 1;
  return;
}
/*++++++++++++++++++++
  Define an entry in the symbol table, and a result with the same name.
  ++++++++++++++++++++*/
  
void
define_symresult(char *name, Expr *e)
{
  define_symbol(name, e);
  define_result(name, e);
  return;
}

/*++++++++++++++++++++
  Flag indicating whether any results evaluated so far have evaluated true.
  (Used for implementing wildcard expression).
  ++++++++++++++++++++*/

static int any_results_so_far;

/*++++++++++++++++++++
  Clear all symbol values.
  ++++++++++++++++++++*/

void
clear_symbol_values(void)
{
  SymbolList *sl;
  for (sl=symbols; sl; sl=sl->next) {
    Symbol *s = sl->sym;
    if (0 == s->is_expr) {
      s->data.val = 0;
    }
  }
  any_results_so_far = 0;
}

/*++++++++++++++++++++
  Set the value of a symbol
  ++++++++++++++++++++*/

void
set_symbol_value(char *sym_name)
{
  Symbol *s;

  s = find_symbol_or_create(sym_name);
  if (s->is_expr) {
    fprintf(stderr, "Cannot set value for symbol '%s', it is defined by an expression\n");
    exit(2);
  } else {
    s->data.val = 1;
  }
}

/*++++++++++++++++++++
  Evaluate the value of an expr
  ++++++++++++++++++++*/
  
static int
eval(Expr *e)
{
  switch (e->type) {
    case E_AND:
      return eval(e->data.and.c1) && eval(e->data.and.c2);
    case E_OR:
      return eval(e->data.or.c1) || eval(e->data.or.c2);
    case E_XOR:
      return eval(e->data.xor.c1) ^ eval(e->data.xor.c2);
    case E_COND:
      return eval(e->data.cond.c1) ? eval(e->data.cond.c2) : eval(e->data.cond.c3);
    case E_NOT:
      return !eval(e->data.not.c1);
    case E_WILD:
      return any_results_so_far;
    case E_SYMBOL:
      {
        Symbol *s = e->data.symbol.s;
        if (s->is_expr) {
          return eval(s->data.e);
        } else {
          return s->data.val;
        }
      }
    default:
      fprintf(stderr, "Can't get here!\n");
      exit(2);
  }
}

/*++++++++++++++++++++
  Evaluate the result which holds given the symbols that are set
  ++++++++++++++++++++*/

int
evaluate_result(char **result)
{
  int i;
  int matched = -1;
  for (i=0; i<n_results; i++) {
    if (eval(results[i].e)) {
      if (any_results_so_far) {
        *result = NULL;
        return 0;
      } else {
        any_results_so_far = 1;
        matched = i;
      }
    }
  }

  if (matched < 0) {
    *result = NULL;
    return 1;
  } else {
    *result = results[matched].result;
    return 1;
  }
}

