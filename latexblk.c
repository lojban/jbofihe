/***************************************
  $Header$

  Driver for producing LaTeX output, using blocks rather than
  free-flow.
  ***************************************/

/* COPYRIGHT */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "output.h"
#include "latex.h"

typedef enum {
  ST_OPEN,
  ST_TEXT,
  ST_CLOSE,
  ST_START
} States;

static States state;

static char tag_text[4096];
static char loj_text[4096];
static char eng_text[4096];

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
initialise(void)
{
  state = ST_START;
  tag_text[0] = 0;
  loj_text[0] = 0;
  eng_text[0] = 0;

}

/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_prologue(void)
{
  printf("\\documentclass[10pt]{article}\n"
         "\\usepackage{geometry}\n"
         "\\ifx\\pdftexversion\\underfined\n"
         "\\usepackage[dvips]{graphicx}\n"
         "\\else\n"
         "\\usepackage[pdftex]{graphicx}\n"
         "\\pdfcompresslevel=9\n"
         "\\fi"
         "\\def\\rmdefault{phv}\n"
         "\\def\\mddefault{mc}\n"
         "\\def\\bfdefault{bc}\n"
         "\\geometry{left=0.75in,top=0.5in,bottom=0.5in,right=0.75in,noheadfoot}\n"
         "\\pagestyle{empty}\n"
         "\\setlength{\\parindent}{0pt}\n"
         "\\setlength{\\parskip}{1ex plus0.5ex minus0.5ex}\n"
         "\\setlength{\\tabcolsep}{0pt}\n"
         "\\emergencystretch=200pt\n"
         "\\font\\zd = pzdr at 10pt\n"
         );
  printf("\\DeclareSymbolFont{AMSa}{U}{msa}{m}{n}\n"
         "\\DeclareMathDelimiter\\ulcorner{4}{AMSa}{\"70}{AMSa}{\"70}\n"
         "\\DeclareMathDelimiter\\urcorner{5}{AMSa}{\"71}{AMSa}{\"71}\n"
         "\\DeclareMathDelimiter\\llcorner{4}{AMSa}{\"78}{AMSa}{\"78}\n"
         "\\DeclareMathDelimiter\\lrcorner{5}{AMSa}{\"79}{AMSa}{\"79}\n");
  printf("\\begin{document}\n"
         "\\small\n");
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
    if (pending_eols > 1) {
      printf("\n\n\\vspace{2ex}");
    } else {
      printf("\n\n");
    }
    state = ST_OPEN;
    pending_eols = 0;
  }
}

/*++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++*/

static void
flush_block(void)
{
  char *p;
  printf("\\begin{tabular}[t]{l}{\\bf %s}\\\\{\\it %s}\\\\[-3pt]",
         loj_text, eng_text);
  printf("{\\it\\scriptsize{}");
  for (p=tag_text; *p; p++) {
    if (*p == '\n') {
      printf("}\\\\[-3pt]{\\it\\scriptsize{}");
    } else {
      putchar(*p);
    }
  }
  printf("}\\end{tabular}\n");

  tag_text[0] = 0;
  loj_text[0] = 0;
  eng_text[0] = 0;

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
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_SQUARE:
      printf("[");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_BRACE:
      printf("\\{");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_ANGLE:
      printf("$\\langle$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_CEIL:
      printf("$\\lceil$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_FLOOR:
      printf("$\\lfloor$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;

    case BR_TRIANGLE:
      printf("{\\normalsize{} $\\triangleleft$}");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
  }

  if (type != BR_NONE) {
    printf(" ");
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

  if (type == BR_NONE) return;

  if (loj_text[0] || eng_text[0]) {
    flush_block();
  }

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
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_SQUARE:
      printf("]");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_BRACE:
      printf("\\}");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_ANGLE:
      printf("$\\rangle$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_CEIL:
      printf("$\\rceil$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
    case BR_FLOOR:
      printf("$\\rfloor$");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;

    case BR_TRIANGLE:
      printf("{\\normalsize{} $\\triangleright$}");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
  }

  if (type != BR_NONE) {
    printf(" ");
  }

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

  strcat(loj_text, " ");
  strcat(loj_text, text);

}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_special(char *text)
{
  if (!strcmp(text, "$LEFTARROW")) {
    strcat(eng_text, "{\\reflectbox{\\zd\\char233}}");
  } else if (!strcmp(text, "$OPENQUOTE")) {
    strcat(eng_text, "``");
  } else if (!strcmp(text, "$CLOSEQUOTE")) {
    strcat(eng_text,"''");
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
    strcat(eng_text, " ");
    strcat(eng_text, make_texsafe(text));
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
  strcat(tag_text, "[");
  first_tag = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
end_tags(void)
{
  strcat(tag_text, ":]");
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tag(void)
{
  if (!first_tag) {
    strcat(tag_text, ", ");
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
    sprintf(buffer, "%s%s\n(%s)", brivla, place, make_texsafe(trans));
  } else {
    sprintf(buffer, "%s%s\n%s", brivla, place, make_texsafe(trans));
  }
  strcat(tag_text, buffer);
}


/*+  +*/
DriverVector latex_block_driver = {
  initialise,
  write_prologue,
  latex_write_epilog,
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
