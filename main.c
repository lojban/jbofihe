/***************************************
  $Header$

  Top level file.
  ***************************************/

/* COPYRIGHT */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodes.h"
#include "functions.h"
#include "version.h"

/* The node into which the parser inserts the top node of the parse
   tree */
TreeNode *top;

extern int yydebug;

extern int yyparse(void);

extern DriverVector latex_driver, latex_block_driver;
extern DriverVector textout_driver, text_block_driver;
extern DriverVector html_driver;

static int had_syntax_error;

/* Optional parameters to be read by backends */
int opt_output_width;

/* Flag to show dictionary defects */
int show_dictionary_defects;

void yyerror(char *s) {
  fprintf(stderr, "--------------------\n");
  fprintf(stderr, "%s\n", s);
  print_last_toks();
  fprintf(stderr, "--------------------\n");
  had_syntax_error = 1;
}

int
main (int argc, char **argv)
{
  char *filename = NULL;
  FILE *in = NULL;
  
  int result;
  int debug;
  int token_lists;
  int show_tree;
  int full_tree;
  int gloss;
  int block;
  int latex;
  int textout;
  int htmlout;
  int show_memory;

  debug = 0;
  token_lists = 0;
  show_tree = 0;
  full_tree = 0;
  gloss = 0;
  latex = 0;
  textout = 0;
  htmlout = 0;
  block = 0;
  show_memory = 0;
  opt_output_width = 80;

  show_dictionary_defects = 0;

  while (++argv, --argc) {
    if (!strcmp(*argv, "-d")) {
      debug = 1;
    } else if (!strcmp(*argv, "-v")) {
      fprintf(stderr, "jbofihe version %s\n", version_string);
      exit(0);
    } else if (!strcmp(*argv, "-k")) {
      token_lists = 1;
    } else if (!strcmp(*argv, "-g")) {
      gloss = 1;
    } else if (!strcmp(*argv, "-t")) {
      show_tree = 1;
    } else if (!strcmp(*argv, "-tf")) {
      show_tree = 1;
      full_tree = 1;
    } else if (!strcmp(*argv, "-l")) {
      latex = 1;
    } else if (!strcmp(*argv, "-x")) {
      textout = 1;
    } else if (!strcmp(*argv, "-h")) {
      htmlout = 1;
    } else if (!strcmp(*argv, "-b")) {
      block = 1;
    } else if (!strcmp(*argv, "-m")) {
      show_memory = 1;
    } else if (!strcmp(*argv, "-dd")) {
      show_dictionary_defects = 1;
    } else if (!strncmp(*argv, "-w", 2)) {
      if (strlen(*argv) > 2) {
        opt_output_width = atoi(*argv + 2);
      } else {
        /* Get next argument */
        --argc, ++argv;
        opt_output_width = atoi(*argv);
      }
    } else if (!strncmp(*argv, "-", 1)) {
      fprintf(stderr, "Unrecognized command line option %s\n", *argv);
      exit(1);
    } else {
      filename = *argv;
    }
  }

  lex2_initialise();

  if (filename) {
    in = fopen(filename, "rb");
    if (!in) {
      fprintf(stderr, "Could not open %s for input\n", filename);
      exit(1);
    }
  }

  parse_file(in ? in : stdin);

  if (in) fclose(in);

  if (token_lists) {
    fprintf(stderr, "\nToken list before preprocessing\n\n");
    show_tokens();
  }

  preprocess_tokens();

  if (token_lists) {
    fprintf(stderr, "\n\nToken list after preprocessing\n\n");
    show_tokens();
  }

#if 1
  yydebug = debug;
  had_syntax_error = 0;

  result = yyparse();

  if (result == 0 && !had_syntax_error) {
    expand_bahe_ui(top);

    error_scan(top);

    if (show_tree) {
      if (!full_tree) {
        compress_singletons(top);
      }

      print_parse_tree(top);

    } else if (latex || textout || htmlout) { 
      
      terms_processing(top);
      do_conversions(top);
      tense_processing(top);
      connectives_processing(top);
      relative_clause_processing(top);

      add_bracketing_tags(top);

      if (latex) {
        do_output(top, block ? &latex_block_driver : &latex_driver);
      } else if (textout) {
        do_output(top, block ? &text_block_driver: &textout_driver);
      } else if (htmlout) {
        do_output(top, &html_driver);
      }

    } else {

      compress_singletons(top);
      print_bracketed_text(top, gloss);

    }

  }
#endif

  if (show_memory) {
    print_memory_statistics();
  }
  
  return 0;
}
