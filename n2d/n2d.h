/***************************************
  $Header$

  Header file for NFA->DFA conversion utility.
  ***************************************/

/* COPYRIGHT */

#ifndef N2D_H
#define N2D_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define new(T) ((T *) malloc(sizeof(T)))
#define new_array(T,N) ((T *) malloc((N) * sizeof(T)))
#define resize_array(T,arr,newN) ((T *) ((arr) ? realloc(arr,(newN)*sizeof(T)) : malloc((newN)*sizeof(T))))
#define new_string(s) strcpy((char *)malloc((strlen(s)+1)*sizeof(char)),s)

struct State;
struct Block;

typedef struct Translist {
  struct Translist *next;
  int token;
  char *ds_name;
  struct State *ds_ref;
} Translist;

typedef struct Stringlist {
  struct Stringlist *next;
  char *string;
} Stringlist;
  
typedef struct State {
  char *name;
  int index; /* Array index in containing block */
  struct Block *parent;
  Translist *transitions;
  Stringlist *exitvals;
} State;

typedef struct Block {
  char *name;
  State **states;
  int nstates;
  int maxstates;
} Block;

/* Constants for 'create' args */  
#define USE_OLD_MUST_EXIST 0
#define CREATE_MUST_NOT_EXIST 1
#define CREATE_OR_USE_OLD 2

State *get_curstate(void);

int lookup_token(char *name, int create);
Block *lookup_block(char *name, int create);
State *lookup_state(Block *in_block, char *name, int create);
Stringlist * add_token(Stringlist *existing, char *token);
void add_transitions(State *curstate, Stringlist *tokens, char *destination);
void add_exit_value(State *curstate, char *value);
void instantiate_block(Block *curblock, char *block_name, char *instance_name);
void fixup_state_refs(Block *b);

#endif /* N2D_H */

