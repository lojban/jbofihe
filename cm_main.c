/***************************************
  $Header$

  Main routine for mini-translater
  ***************************************/

/* Copyright 1998-2001 Richard P. Curnow */
/* Help options added by Björn Gohla */
/* LICENCE */

#include "cm.h"
#include "version.h"

extern FILE *yyin;
extern int yylex(void);

int
yywrap(void)
{
  return 1;
}

int
main (int argc, char **argv)
{
  char *filename = NULL;
  FILE *in = NULL;
  
  ofmt = OF_TEXT;
  width = 80;

  while (++argv,--argc) {
    if (!strcmp(*argv, "-l")) {
      ofmt = OF_LATEX;
    } else if (!strcmp(*argv, "-b")) {
      ofmt = OF_TEXTBLK;
#ifdef PLIST
    } else if (!strcmp(*argv, "-p")) {
      ofmt = OF_PLIST;
#endif 
    } else if (!strcmp(*argv, "-w")) {
      ++argv, --argc;
      width = atoi(*argv);
    } else if (!strcmp(*argv, "-v")) {
      fprintf(stderr, "cmafihe version %s\n", version_string);
      exit(0);
    } else if ( !strcmp(*argv, "-h") || !strcmp(*argv, "--help") ) {
      fprintf(stderr, "cmafihe, gloss lojban text without verifying\n"
                      "usage: cmafihe [-b [-w WIDTH] | -p | -l | -v] [FILENAME]\n"
                      "no options : output inline ascii\n"
                      "-b         : output blocked ascii with optional WIDTH, default %i\n"
                      "-l         : output blocked latex code\n"
#ifdef PLIST
                      "-p         : output GNUStep property list with vocabulary\n"
#endif
                      "-v         : version\n", width);
      exit(0);
    } else {
      filename = *argv;
    }
  }

  if (filename) {
    in = fopen(filename, "rb");
    if (!in) {
      fprintf(stderr, "Could not open %s for input\n", filename);
      exit(1);
    }
  }

  if (in) {
    yyin = in;
  }
  
  yylex();

  if (in) {
    fclose(in);
  }

  do_trans();

  start_output();
  do_output();
  end_output();
  output_newline();
  return 0;
}
