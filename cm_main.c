/***************************************
  $Header$

  Main routine for mini-translater
  ***************************************/

/* COPYRIGHT */

#include "cm.h"
#include "version.h"

int
yywrap(void)
{
  return 1;
}

int
main (int argc, char **argv)
{
  ofmt = OF_TEXT;
  width = 80;

  while (++argv,--argc) {
    if (!strcmp(*argv, "-l")) {
      ofmt = OF_LATEX;
    } else if (!strcmp(*argv, "-b")) {
      ofmt = OF_TEXTBLK;
    } else if (!strcmp(*argv, "-w")) {
      ++argv, --argc;
      width = atoi(*argv);
    } else if (!strcmp(*argv, "-v")) {
      fprintf(stderr, "cmafihe version %s\n", version_string);
      exit(0);
    }      
  }

  yylex();

  do_trans();

  start_output();
  do_output();
  end_output();
  output_newline();
  return 0;
}
