/***************************************
  $Header$

  Driver for producing LaTeX output from the glosser.
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
         "\\font\\zd = pzdr at 10pt\n"
         );
  printf("\\DeclareSymbolFont{AMSa}{U}{msa}{m}{n}\n"
         "\\DeclareMathDelimiter\\ulcorner{4}{AMSa}{\"70}{AMSa}{\"70}\n"
         "\\DeclareMathDelimiter\\urcorner{5}{AMSa}{\"71}{AMSa}{\"71}\n"
         "\\DeclareMathDelimiter\\llcorner{4}{AMSa}{\"78}{AMSa}{\"78}\n"
         "\\DeclareMathDelimiter\\lrcorner{5}{AMSa}{\"79}{AMSa}{\"79}\n");
  printf("\\begin{document}\n");
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

void
latex_write_epilog(void)
{
  printf("\\end{document}\n");
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
  double xes;

  if (pending_eols > 0) {
    if (pending_eols > 1) {
      xes = 1.5;
    } else {
      xes = 1.0;
    }
    printf("\n\n\\vspace{%.2fex}", xes);
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

void
latex_write_open_bracket(BracketType type, int subscript)
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
      printf("{\\large{} $\\triangleleft$}");
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

void
latex_write_close_bracket(BracketType type, int subscript)
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
      printf("{\\large{} $\\triangleright$}");
      printf("{}$^{\\textit{\\scriptsize %d}}$", subscript);
      break;
  }

  if (type != BR_NONE) {
    printf(" ");
  }

  state = ST_CLOSE;

}

/*++++++++++++++++++++++++++++++
  Make a string safe for setting with TeX.  Don't make $, \ and {/} safe
  though, so that translations can include escape sequences.

  static char * make_texsafe

  char *s
  ++++++++++++++++++++++++++++++*/

char *
make_texsafe(char *s)
{
  static char buf[2048];
  char *p, *q;
  p = s;
  q = buf;
  while (*p) {
    switch (*p) {
      case '^':
      case '&':
#if 0
      case '\\':
      case '$':
#endif
      case '{':
      case '}':
      case '%':
      case '_':
      case '#':
        *q++ = '\\';
        *q++ = *p++;
        break;
      case '<':
      case '>':
        *q++ = '$';
        *q++ = *p++;
        *q++ = '$';
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

  printf("\\mbox{\\textbf{%s}\\/}", text);

  state = ST_TEXT;
}


/*++++++++++++++++++++++++++++++++++++++
  

  char *text
  ++++++++++++++++++++++++++++++++++++++*/

static void
write_special(char *text)
{
  if (!strcmp(text, "$LEFTARROW")) {
    printf("{\\reflectbox{\\zd\\char233}}");
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
      printf("\n");
      break;
  }

  if (text[0] == '$') {
    write_special(text);
  } else {
    printf("\\textsl{\\small{}%s}", make_texsafe(text));
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
  printf("\\textsl{\\footnotesize{}[}");
  first_tag = 1;
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
end_tags(void)
{
  printf("\\textsl{\\footnotesize{}~:] }");
}


/*++++++++++++++++++++++++++++++++++++++
  
  ++++++++++++++++++++++++++++++++++++++*/

static void
start_tag(void)
{
  if (!first_tag) {
    printf("\\textsl{\\footnotesize{}, }\n");
  }
  first_tag = 0;
}

static void write_tag_text(char *brivla, char *place, char *trans, int brac)/*{{{*/
{
  if (brac) {
    printf("\\textsl{\\footnotesize{}%s%s (%s)}\n", brivla, place, make_texsafe(trans));
  } else {
    printf("\\textsl{\\footnotesize{}%s%s %s}\n", brivla, place, make_texsafe(trans));
  }
}
/*}}}*/
static void write_partial_tag_text(char *t)/*{{{*/
{
  printf("\\textsl{\\footnotesize{}%s}\n", t);
}
/*}}}*/

DriverVector latex_driver =/*{{{*/
{
  initialise,
  write_prologue,
  latex_write_epilog,
  latex_write_open_bracket,
  latex_write_close_bracket,
  set_eols,
  write_lojban_text,
  write_translation,
  start_tags,
  end_tags,
  start_tag,
  write_tag_text,
  write_partial_tag_text
};/*}}}*/
