/**********************************************************************
  $Header$

  Grammar definition for input files defining an NFA

 *********************************************************************/

/* COPYRIGHT */

%{
#include "n2d.h"

static Block *curblock = NULL; /* Current block being built */
static State *curstate = NULL; /* Current state being worked on */
static struct Define *curdefine = NULL; /* Current definition being worked on */

/* Prefix set by prefix command */
char *prefix = NULL;

State *get_curstate(void) { return curstate; }

%}

%union {
    char *s;
    int i;
    Stringlist *sl;
}


%token STRING STATE TOKENS PREFIX ARROW PIPE BLOCK ENDBLOCK COLON EQUAL
%token DEFINE
%type<s> STRING transition
%type<sl> transition_seq

%%

all : decl_seq ;

decl_seq : /* empty */ | decl_seq decl ;

decl : block_decl | tokens_decl | prefix_decl | define_decl ;

/* Don't invalidate curstate at the end, this is the means of working out the
   starting state of the NFA */
block_decl : block1 block2 { fixup_state_refs(curblock); curblock = NULL; } ;

block1 : BLOCK STRING { curblock = lookup_block($2, CREATE_MUST_NOT_EXIST); curstate = NULL; } ;

block2 : instance_decl_seq state_decl_seq ENDBLOCK ;

prefix_decl : PREFIX STRING { prefix = $2; };

tokens_decl : TOKENS token_seq ;

define_decl : DEFINE STRING { curdefine = create_def($2); }
              EQUAL string_pipe_seq
            ;

token_seq : token_seq token | token ;

string_pipe_seq : string_pipe_seq PIPE STRING { add_tok_to_def(curdefine, $3); }
                |                      STRING { add_tok_to_def(curdefine, $1); }
                ;

token : STRING { (void) lookup_token($1, CREATE_MUST_NOT_EXIST); }

instance_decl_seq : /* empty */ | instance_decl_seq instance_decl ;

state_decl_seq : /* empty */ | state_decl_seq state_decl ;

state_decl : STATE STRING { curstate = lookup_state(curblock, $2, CREATE_OR_USE_OLD); } sdecl_seq ;

sdecl_seq : /* empty */ | sdecl_seq sdecl ;

sdecl : transition_decl | exit_decl ;

instance_decl : STRING COLON STRING { instantiate_block(curblock, $3 /* master_block_name */, $1 /* instance_name */ ); } ;

transition_decl : epsilon ARROW STRING { add_transitions(curstate, NULL, $3); } 
                | transition_seq ARROW STRING { add_transitions(curstate, $1, $3); }
                ;

epsilon : /* empty */

transition_seq : transition { $$ = add_token(NULL, $1); }
               | transition_seq PIPE transition { $$ = add_token($1, $3); } ;

transition : STRING ;

exit_decl : EQUAL STRING { add_exit_value(curstate, $2); } ;


