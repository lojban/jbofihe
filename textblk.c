/***************************************
  $Header$

  Driver for producing plain text output, using blocks rather than
  free-flow.
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "output.h"

#define MAX_WIDTH 120
static int max_width = MAX_WIDTH;

#define BUFFER_SIZE 512

typedef enum {
  ST_OPEN,
  ST_TEXT,
  ST_CLOSE,
  ST_START
} States;

static States state;

/* Largest number of tags supported on a single sumti */
#define MAX_TAGS 32

/* Variables to hold block being built */
static int tags_used;
static char tag_text[MAX_TAGS][BUFFER_SIZE];
static char loj_text[BUFFER_SIZE];
static char eng_text[BUFFER_SIZE];

/* Variables to hold line being constructed */
static int current_width; /* The width of the line buffer currently occupied */
static char tag_line[MAX_TAGS][BUFFER_SIZE];
static int line_tags_used;
static char loj_line[BUFFER_SIZE];
static char eng_line[BUFFER_SIZE];


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
clear_line_buffer(void)
{
  int i;

  for (i=0; i<MAX_TAGS; i++) {
    tag_line[i][0] = 0;
  }
  line_tags_used = 0;

  loj_line[0] = 0;
  eng_line[0] = 0;

  current_width = 0;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
initialise(void)
{
  int i;

  state = ST_START;
  for (i=0; i<MAX_TAGS; i++) {
    tag_text[i][0] = 0;
  }
  loj_text[0] = 0;
  eng_text[0] = 0;
  tags_used = 0;

  clear_line_buffer();
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
flush_line(void)
{
  int i;
  
  if (current_width == 0) return;

  fputs(loj_line, stdout);
  fputs("\n", stdout);
  fputs(eng_line, stdout);
  fputs("\n", stdout);
  for (i=0; i<line_tags_used; i++) {
    fputs(tag_line[i], stdout);
    fputs("\n", stdout);
  }
  fputs("\n", stdout);
  clear_line_buffer();
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_epilog(void)
{
  flush_line();
  return;
}



/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

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

    flush_line();

    if (pending_eols > 1) {
      printf("\n------------------------------------------------------------\n");
    } else {
      printf("\n");
    }
    state = ST_OPEN;
    pending_eols = 0;
  }
}


/*++++++++++++++++++++++++++++++++++++++
  Append a string onto a buffer, and right pad with spaces to bring up
  to a specific width

  char *src

  char *dbuf

  int wid
  ++++++++++++++++++++++++++++++++++++++*/

static void
append_to_width(char *src, char *dbuf, int wid) {
  int len, toadd, i;
  char buffer[BUFFER_SIZE];
  len = strlen(src);
  toadd = wid - len;
  for (i=0; i<toadd; i++) {
    buffer[i] = ' ';
  }
  buffer[toadd] = '\0';
  strcat(dbuf, src);
  strcat(dbuf, buffer);
}

/*++++++++++++++++++++++++++++++
  Push current block onto line buffer
  ++++++++++++++++++++++++++++++*/

static void
flush_block(void)
{
  int i;
  int max_len, len;

  if (!loj_text[0] && !eng_text[0] && (tags_used == 0)) return; /* Nothing to flush */

  /* Calculate width of block */
  max_len = 0;
  len = strlen(loj_text);
  if (len > max_len) max_len = len;
  len = strlen(eng_text);
  if (len > max_len) max_len = len;
  for (i=0; i<tags_used; i++) {
    len = strlen(tag_text[i]);
    if (len > max_len) max_len = len;
  }
  
  if (max_len + current_width > max_width) {
    flush_line();
  }

  append_to_width(loj_text, loj_line, max_len);
  append_to_width(eng_text, eng_line, max_len);

  for (i=0; i<tags_used; i++) {
    if (i >= line_tags_used) {
      append_to_width("", tag_line[i], current_width);
    }
    append_to_width(tag_text[i], tag_line[i], max_len);
  }

  /* Space-pad the tag lines on words that don't generate enough
     tags. */
  for (i=tags_used; i<line_tags_used; i++) {
    append_to_width("", tag_line[i], max_len);
  }
  
  current_width += max_len;

  if (tags_used > line_tags_used) line_tags_used = tags_used;

  /* Clear the block buffers for the next lot */
  for (i=0; i<MAX_TAGS; i++) {
    tag_text[i][0] = 0;
  }
  loj_text[0] = 0;
  eng_text[0] = 0;

  tags_used = 0;
  
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

  if (type == BR_NONE) return;

  if (loj_text[0] || eng_text[0]) {
    flush_block();
  }

  clear_eols();

#if 0  
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
      printf("%d", subscript);
      break;
    case BR_SQUARE:
      printf("[");
      printf("%d$", subscript);
      break;
    case BR_BRACE:
      printf("\\{");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_ANGLE:
      printf("$\\langle$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_CEIL:
      printf("$\\lceil$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_FLOOR:
      printf("$\\lfloor$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;

    case BR_TRIANGLE:
      printf("\\TL{}");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
  }

  if (type != BR_NONE) {
    printf(" ");
  }

#endif

  state = ST_OPEN;

}


/*++++++++++++++++++++++++++++++++++++++
  

  BracketType type

  int subscript
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_close_bracket(BracketType type, int subscript)
{

  if (type == BR_NONE) return;

  if (loj_text[0] || eng_text[0]) {
    flush_block();
  }

#if 0

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
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_SQUARE:
      printf("]");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_BRACE:
      printf("\\}");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_ANGLE:
      printf("$\\rangle$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_CEIL:
      printf("$\\rceil$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
    case BR_FLOOR:
      printf("$\\rfloor$");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;

    case BR_TRIANGLE:
      printf("\\TR{}");
      printf("{}$^{\\IS{%d}}$", subscript);
      break;
  }

  if (type != BR_NONE) {
    printf(" ");
  }

#endif

  state = ST_CLOSE;

}



/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_lojban_text(char *text)
{

  if (eng_text[0]) {
    flush_block();
  }

  clear_eols();

  strcat(loj_text, text);
  strcat(loj_text, " ");

}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_special(char *text)
{
  if (!strcmp(text, "$LEFTARROW")) {
    strcat(eng_text, "<-");
  } else if (!strcmp(text, "$OPENQUOTE")) {
    strcat(eng_text, "\"");
  } else if (!strcmp(text, "$CLOSEQUOTE")) {
    strcat(eng_text,"\"");
  }
}

/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_translation(char *text)
{
  if (text[0] == '$') {
    write_special(text);
  } else {
    strcat(eng_text, text);
    strcat(eng_text, " ");
  }
}

/*+  +*/
static int first_tag;

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tags(void)
{
  if (loj_text[0] || eng_text[0]) {
    flush_block();
  }
  clear_eols();
  first_tag = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
end_tags(void)
{
  tags_used++;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tag(void)
{
  if (!first_tag) {
    tags_used++;
  }
  first_tag = 0;
}


/*++++++++++++++++++++++++++++++++++++++
  

  char *brivla

  char *place

  char *trans
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_tag_text(char *brivla, char *place, char *trans, int brac)
{
  char buffer[1024];

  if (brac) {
    sprintf(buffer, "%s%s (%s) ", brivla, place, trans);
  } else {
    sprintf(buffer, "%s%s %s ", brivla, place, trans);
  }
  strcat(tag_text[tags_used], buffer);
}


/*+  +*/
DriverVector text_block_driver = {
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
};
