/***************************************
  $Header$

  Provide traceback capability when parse errors are encountered.
  ***************************************/

/* COPYRIGHT */

#include <stdio.h>
#include "trctabs.c"

static int reductions[1024];
static int n_reductions=0;
static int is_full_parse = 0;

/* command line option in main.c */
extern int show_backtrace;

static void
display_rule(int ruleno, int indent, int focus)
{
  int i, j;
  char **toknames = is_full_parse ? norm_toknames : norm_toknames;
  unsigned short *ruleindex = is_full_parse ? norm_ruleindex : norm_ruleindex;
  unsigned short *rulelhs = is_full_parse ? norm_rulelhs : norm_rulelhs;
  unsigned short *rulerhs = is_full_parse ? norm_rulerhs : norm_rulerhs;


  for (i=0; i<indent; i++) fputc(' ', stdout);
  printf("Rule %d : %s <- ", ruleno, toknames[rulelhs[ruleno]]);
  for (i=0, j=ruleindex[ruleno]; j<ruleindex[ruleno+1]; i++, j++) {
    if (i>0) printf(" ");
    printf("%s", toknames[rulerhs[j]]);
    if (i+1 == focus) {
      printf(" .");
    }
  }
  printf("\n");
}

void
report_trace_reduce(int stateno, int ruleno)
{
  reductions[n_reductions++] = ruleno;
}

void
report_trace_shift(int yychar, int yychar1)
{
  /* After a successful shift, reduction list clears */
  n_reductions = 0;
}

void
report_trace_error(short *yyss, short *yyssp)
{
  int i;
  short *s;
  unsigned short *stateindex = is_full_parse ? norm_stateindex : norm_stateindex;
  unsigned short *shiftrule = is_full_parse ? norm_shiftrule : norm_shiftrule;
  unsigned char *focus = is_full_parse ? norm_focus : norm_focus;
  unsigned short *shift_in_state = is_full_parse ? norm_shift_in_state : norm_shift_in_state;
  unsigned short *shift_in_state_index = is_full_parse ? norm_shift_in_state_index : norm_shift_in_state_index;
  char **toknames = is_full_parse ? norm_toknames : norm_toknames;

  if (!show_backtrace) return;

  printf("==============================================\n"
         "Rules reduced since misparsed token was read :\n"
         "==============================================\n");
  if (n_reductions == 0) {
    printf("  NONE");
  } else {
    for (i=0; i<n_reductions; i++) {
      display_rule(reductions[i], 2, 0);
    }
  }

  printf("=====================\n"
         "Jammed parser state :\n"
         "=====================\n");
  {
    int r1, r2;
    int r;
    int x, x_start;
    int is_first;
    r1 = stateindex[*yyssp], r2 = stateindex[*yyssp+1];
    for (r=r1; r<r2; r++) {
      display_rule(shiftrule[r], 2, focus[r]);
    }
  
    /* Print out which tokens would have been possible */
    printf("\nTypes of word that would have been valid here :\n  ");
    r1 = shift_in_state_index[*yyssp], r2 = shift_in_state_index[*yyssp+1];
    is_first = 1;
    for (r=r1; r<r2; r++) {
      printf("%s%s", is_first ? "" : " ", toknames[shift_in_state[r]]);
      is_first = 0;
    }
    printf("\n");
  }

  printf("=========================================================\n"
         "Pending parser states (innermost first, outermost last) :\n"
         "=========================================================\n");
  for (s=yyssp-1; s>=yyss; s--) {
    int r1, r2;
    int r;
    r1 = stateindex[*s], r2 = stateindex[*s+1];
    for (r=r1; r<r2; r++) {
      display_rule(shiftrule[r], 2, focus[r]);
    }
    printf("---------------------------\n");
  }
}
