/***************************************
  $Header$

  Main routine for mini-translater
  ***************************************/

/* COPYRIGHT */

#include "cm.h"
#include "version.h"

extern FILE *yyin;

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
    } else if (!strcmp(*argv, "-w")) {
      ++argv, --argc;
      width = atoi(*argv);
    } else if (!strcmp(*argv, "-v")) {
      fprintf(stderr, "cmafihe version %s\n", version_string);
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
