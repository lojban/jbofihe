/***************************************
  $Header$

  Driver for producing HTML output from the glosser.
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
  printf("<HTML>\n"
         "<HEAD>\n"
         "<TITLE>Lojban translation</TITLE>\n"
         "<BODY>\n");
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_epilog(void)
{
  printf("</BODY>\n");
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
    printf("\n<P><P>\n");
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

#define PRINT_SUB printf("<SUB><FONT SIZE=\"-3\">%d</FONT></SUB>", subscript)

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
      PRINT_SUB;
      break;
    case BR_SQUARE:
      printf("[");
      PRINT_SUB;
      break;
    case BR_BRACE:
      printf("{");
      PRINT_SUB;
      break;
    case BR_ANGLE:
      printf("&lt;");
      PRINT_SUB;
      break;
    case BR_CEIL:
      printf("(");
      PRINT_SUB;
      break;
    case BR_FLOOR:
      printf("(");
      PRINT_SUB;
      break;
    case BR_TRIANGLE:
      printf("&lt;&lt;");
      PRINT_SUB;
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
      PRINT_SUB;
      break;
    case BR_SQUARE:
      printf("]");
      PRINT_SUB;
      break;
    case BR_BRACE:
      printf("}");
      PRINT_SUB;
      break;
    case BR_ANGLE:
      printf("&gt;");
      PRINT_SUB;
      break;
    case BR_CEIL:
      printf(")");
      PRINT_SUB;
      break;
    case BR_FLOOR:
      printf(")");
      PRINT_SUB;
      break;
    case BR_TRIANGLE:
      printf("&gt;&gt;");
      PRINT_SUB;
      break;
  }

  state = ST_CLOSE;

}

/*++++++++++++++++++++++++++++++
  Make a string safe for setting with HTML.

  static char * make_htmlsafe

  char *s
  ++++++++++++++++++++++++++++++*/

static char *
make_htmlsafe(char *s)
{
  static char buf[2048];
  char *p, *q;
  p = s;
  q = buf;
  while (*p) {
    switch (*p) {
      case '&':
        strcpy(q, "&amp;");
        q += 5;
        break;
      case '<':
        strcpy(q, "&lt;");
        q += 4;
        break;
      case '>':
        strcpy(q, "&gt;");
        q += 4;
        break;
      default:
        *q++ = *p++;
        break;
    }
  }        
  *q = 0;

  return buf;
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
      printf("\n");
      break;
  }

  printf("<B>%s</B>", text);

  state = ST_TEXT;
}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_special(char *text)
{
  if (!strcmp(text, "$LEFTARROW")) {
    printf("&lt;-");
  } else if (!strcmp(text, "$OPENQUOTE")) {
    printf("<FONT SIZE=+2>&quot;</FONT>");
  } else if (!strcmp(text, "$CLOSEQUOTE")) {
    printf("<FONT SIZE=+2>&quot;</FONT>");
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
      printf("\n");
      break;
  }

  if (text[0] == '$') {
    write_special(text);
  } else {
    printf("<I>%s</I>", make_htmlsafe(text));
  }

  state = ST_TEXT;
}

/*+  +*/
static int first_tag;

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tags(void)
{
  printf("<U><FONT SIZE=-1>[");
  first_tag = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
end_tags(void)
{
  printf(":] </FONT></U>");
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

  int place

  char *trans
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_tag_text(char *brivla, char *place, char *trans, int brac)
{
  if (brac) {
    printf("%s%s (%s)\n", brivla, place, make_htmlsafe(trans));
  } else {
    printf("%s%s %s\n", brivla, place, make_htmlsafe(trans));
  }
}

static void write_partial_tag_text(char *t)/*{{{*/
{
  printf("%s", t);
}
/*}}}*/

DriverVector html_driver =/*{{{*/
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
