/***************************************
  $Header$

  Output formatting functions
  ***************************************/

/* COPYRIGHT */

#include "cm.h"

/*+ Which format has been requested +*/
OutputFormat ofmt;

/*+ Line width +*/
int width;

/* ================================================== */

#define BUFFER_SIZE 512

static char lines[3][BUFFER_SIZE];
static int width_used;

/* ================================================== */

static void
block_newline(void)
{
  if (width_used > 0) {
    printf("%s\n", lines[0]);
    printf("%s\n", lines[1]);
    printf("%s\n", lines[2]);
    printf("\n");
  }
  lines[0][0] = lines[1][0] = lines[2][0] = 0;
  width_used = 0;
}

/* ================================================== */

static void
append_content(char *buf, const char *x, int len, int mlen)
{
  int i;
  int d;
  char *p;
  strcat(buf, x);
  d = mlen - len;
  p = buf;
  while (*p) p++;
  for (i=0; i<d; i++) {
    *p++ = ' ';
  }
  *p = 0;
}

/* ================================================== */

static void
do_block(const char *a1, const char *a2, const char *a3)
{
  int l1, l2, l3, ml;

  l1 = strlen(a1);
  l2 = strlen(a2);
  l3 = strlen(a3);
  ml = (l1 > l2) ? l1 : l2;
  ml = (ml > l3) ? ml : l3;

  if (ml + width_used + 1 > width) {
    block_newline();
  }

  append_content(&lines[0][0], a1, l1, ml+1);
  append_content(&lines[1][0], a2, l2, ml+1);
  append_content(&lines[2][0], a3, l3, ml+1);
  
  width_used += (ml + 1);
  return;
}

/* ================================================== */
/* ================================================== */

void
start_output(void)
{
  switch (ofmt) {
    case OF_LATEX:
      printf("\\documentclass[10pt]{article}\n"
             "\\usepackage{geometry}\n"
             "\\geometry{left=0.5in,right=0.5in,top=0.5in,bottom=0.5in,a4paper}\n"
             "\\def\\rmdefault{bch}\n"
             "\\setlength{\\parindent}{0pt}\n"
             "\\setlength{\\parskip}{1ex plus0.5ex minus0.5ex}\n"
             "\\setlength{\\baselineskip}{10pt}\n"
             "\\def\\arraystretch{0.7}\n"
             "\\begin{document}\n"
             );
      break;
    case OF_TEXT:
      break;
    case OF_TEXTBLK:
      lines[0][0] = lines[1][0] = lines[2][0] = 0;
      width_used = 0;
      break;
  }

}

/* ================================================== */

void
end_output(void)
{
  switch (ofmt) {
    case OF_LATEX:
      printf("\\end{document}\n");
      break;
    case OF_TEXT:
      break;
    case OF_TEXTBLK:
      block_newline();
      break;
  }
}

/* ================================================== */

void
output(const char *lojban, const char *trans, const char *selmao)
{
  switch (ofmt) {
    case OF_LATEX:
    printf ("\\begin{tabular}[t]{l}"
            "\\textbf{\\footnotesize %s}\\\\\n"
            "\\textrm{\\footnotesize %s}\\\\\n"
            "\\textit{\\footnotesize %s}\n"
            "\\end{tabular}\n"
            "\\rule{0in}{1.0\\baselineskip}",
            lojban, selmao, trans);
      break;
    case OF_TEXT:
    printf ("%s <%s> [%s] ", lojban, selmao, trans);
      break;
    case OF_TEXTBLK:
      do_block(lojban, selmao, trans);
      break;
  }
}
/* ================================================== */

void
output_newline(void)
{
  switch (ofmt) {
    case OF_LATEX:
    printf("\n\n");
      break;
    case OF_TEXT:
    printf("\n");
      break;
    case OF_TEXTBLK:
      block_newline();
      printf("\n");
      break;
  }
}

/* ================================================== */

void
output_paren(const char *text)
{
  switch (ofmt) {
    case OF_LATEX:
    printf ("\\textrm{\\footnotesize %s}", text);
      break;
    case OF_TEXT:
    printf ("(%s) ", text);
      break;
    case OF_TEXTBLK:
      do_block("(", "(", "(");
      do_block(text, "", "");
      do_block(")", ")", ")");
      break;
  }
}

/* ================================================== */
