/***************************************
  $Header$

  Header file for bad cmene checking function
  
  ***************************************/

/* Copyright (C) Richard P. Curnow  2001 */
/* LICENCE */

#ifndef BCCHECK_H
#define BCCHECK_H

/* Return 1 if a proposed cmene in 'word' is bad.  'split' is 1 if the word
 * breaks up (as opposed to just being bad but not breaking).  'ladoi' gives
 * the position of the la/la'i/doi, and 'tail' gives the position of the
 * remainder of the word. */
extern int is_bad_cmene(char *word, int *split, char **ladoi, char **tail);

#endif /* BCCHECK_H */

