/***************************************
  $Header$

  Header file for LaTeX code.
  ***************************************/

/* COPYRIGHT */

#ifndef LATEX_H
#define LATEX_H    /*+ To stop multiple inclusions. +*/

#include "nodes.h"

extern void latex_write_epilog(void);
extern char *make_texsafe(char *s);
extern void latex_write_open_bracket(BracketType type, int subscript);
extern void latex_write_close_bracket(BracketType type, int subscript);

#endif /* LATEX_H */
