/***************************************

  $Id$

  Header file for all local fns etc.


  ***************************************/

/* COPYRIGHT */

#ifndef MTRAN_H
#define MTRAN_H    /*+ To stop multiple inclusions. +*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define new_string(s) strcpy((char *) malloc(1+strlen(s)), (s))
#define extend_string(s, x) strcat((char *) realloc(s, 1+strlen(s)+strlen(x)), x)
#define new(T) (T *) malloc(sizeof(T))
#define new_array(T, n) (T *) malloc(sizeof(T) * (n))

/* ================================================== */

/*+ Flag indicating whether to generate latex blocked output instead
  of text. +*/
extern int do_latex;

/* ================================================== */

void gather_cmavo (const char *x);
void gather_brivla (const char *x);
void gather_cmene (const char *x);
void gather_whitespace(void); 
void gather_newline(void);
void gather_fallthru(const char *x);
void gather_paren(const char *x);
void do_trans(void);
void do_output(void);

char * translate(char *word);
char * translate_unknown(char *w);

void output(const char *lojban, const char *trans, const char *selmao);
void output_newline(void);
void output_paren(const char *text);
void start_output(void);
void end_output(void);

#endif /* MTRAN_H */
