/***************************************
  $Header$

  Top level file.
  ***************************************/

/* COPYRIGHT */


#include <stdio.h>
#include <string.h>
#include "nodes.h"
#include "functions.h"

/* The node into which the parser inserts the top node of the parse
   tree */
TreeNode *top;

extern int yydebug;

extern int yyparse(void);

extern DriverVector latex_driver, latex_block_driver, textout_driver, html_driver;

static int had_syntax_error;

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

  debug = 0;
  token_lists = 0;
  show_tree = 0;
  full_tree = 0;
  gloss = 0;
  latex = 0;
  textout = 0;
  htmlout = 0;
  block = 0;

  while (++argv, --argc) {
    if (!strcmp(*argv, "-d")) {
      debug = 1;
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
    }
  }

  lex2_initialise();
  parse_file(stdin);
  
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
    if (!full_tree && !(latex||textout||htmlout)) {
      compress_singletons(top);
    }

    do_conversions(top);

    if (latex || textout || htmlout) {
      terms_processing(top);
      tense_processing(top);
      connectives_processing(top);
    }

    if (show_tree) {
      print_parse_tree(top);
    }

    if (latex || textout || htmlout) {
      add_bracketing_tags(top);

      if (latex) {
        do_output(top, block ? &latex_block_driver : &latex_driver);
      } else if (textout) {
        do_output(top, &textout_driver);
      } else if (htmlout) {
        do_output(top, &html_driver);
      }

    } else {
      print_bracketed_text(top, gloss);
    }

  }
#endif

  print_memory_statistics();
  
  return 0;
}
