/***************************************
  $Header$

  Driver for producing plain text output from the glosser.
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "output.h"

typedef enum {
  ST_OPEN,
  ST_TEXT,
  ST_CLOSE,
  ST_START
} States;

static States state;

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
initialise(void)
{
  state = ST_START;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_prologue(void)
{
  return;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_epilog(void)
{
  printf("\n");
  return;
}


/* Number of end of lines that are pending.  (These are only inserted
   when we have closed a sequence of close brackets, i.e. before the
   next open bracket or ordinary text.) */
static int pending_eols = 0;

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
clear_eols(void)
{
  if (pending_eols > 0) {
    printf("\n\n");
    state = ST_OPEN;
    pending_eols = 0;
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
set_eols(int eols)
{
  pending_eols += eols;
}


/*++++++++++++++++++++++++++++++++++++++
  

  BracketType type

  int subscript
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_open_bracket(BracketType type, int subscript)
{
  clear_eols();
  
  switch (state) {
    case ST_START:
    case ST_OPEN:
      break;

    case ST_TEXT:
    case ST_CLOSE:
      printf(" ");
      break;
  }

  switch (type) {
    case BR_NONE:
      break;
    case BR_ROUND:
      printf("(");
      break;
    case BR_SQUARE:
      printf("[");
      break;
    case BR_BRACE:
      printf("{");
      break;
    case BR_ANGLE:
      printf("<");
      break;
    case BR_CEIL:
      printf("^");
      break;
    case BR_FLOOR:
      printf("^");
      break;

    case BR_TRIANGLE:
      printf("<<");
      break;
  }

  state = ST_OPEN;

}


/*++++++++++++++++++++++++++++++++++++++
  

  BracketType type

  int subscript
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_close_bracket(BracketType type, int subscript)
{

  switch (state) {
    case ST_START:
    case ST_CLOSE:
    case ST_TEXT:
      break;

    case ST_OPEN:
      printf(" ");
      break;
  }

  switch (type) {
    case BR_NONE:
      break;
    case BR_ROUND:
      printf(")");
      break;
    case BR_SQUARE:
      printf("]");
      break;
    case BR_BRACE:
      printf("}");
      break;
    case BR_ANGLE:
      printf(">");
      break;
    case BR_CEIL:
      printf("^");
      break;
    case BR_FLOOR:
      printf("^");
      break;

    case BR_TRIANGLE:
      printf(">>");
      break;
  }

  state = ST_CLOSE;

}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_lojban_text(char *text)
{

  switch (state) {
    case ST_START:
    case ST_OPEN:
      break;
      
    case ST_TEXT:
    case ST_CLOSE:
      printf(" ");
      break;
  }

  printf("%s", text);
  fflush(stdout);

  state = ST_TEXT;
}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_special(char *text)
{
  if (!strcmp(text, "$LEFTARROW")) {
    printf("<-");
  } else if (!strcmp(text, "$OPENQUOTE")) {
    printf("``");
  } else if (!strcmp(text, "$CLOSEQUOTE")) {
    printf("''");
  }
}

/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_translation(char *text)
{

  switch (state) {
    case ST_START:
    case ST_OPEN:
      break;
      
    case ST_TEXT:
    case ST_CLOSE:
      printf(" ");
      break;
  }

  if (text[0] == '$') {
    write_special(text);
  } else {
    printf("/%s/", text);
  }

  state = ST_TEXT;
  /* fflush(stdout); */
}

/*+  +*/
static int first_tag;

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tags(void)
{
  printf("[");
  first_tag = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
end_tags(void)
{
  printf(":]");
  state = ST_CLOSE;
  return;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/


static void
start_tag(void)
{
  if (!first_tag) {
    printf(", ");
  }
  first_tag = 0;
}

/*++++++++++++++++++++++++++++++++++++++
  

  char *brivla

  char *place

  char *trans

  int brac
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_tag_text(char *brivla, char *place, char *trans, int brac)
{
  if (brac) {
    printf("%s%s (%s)", brivla, place, trans);
  } else {
    printf("%s%s %s", brivla, place, trans);
  }
}

static void write_partial_tag_text(char *t)/*{{{*/
{
  printf("%s", t);
}
/*}}}*/

DriverVector textout_driver = /*{{{*/
{
  initialise,
  write_prologue,
  write_epilog,
  write_open_bracket,
  write_close_bracket,
  set_eols,
  write_lojban_text,
  write_translation,
  start_tags,
  end_tags,
  start_tag,
  write_tag_text,
  write_partial_tag_text
};/*}}}*/

