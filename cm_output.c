/***************************************
  $Header$

  Output formatting functions
  ***************************************/

/* COPYRIGHT */

#include "cm.h"

int do_latex = 0;

/* ================================================== */

void
start_output(void)
{
  if (do_latex) {
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
  }

}

/* ================================================== */

void
end_output(void)
{
  if (do_latex) {
    printf("\\end{document}\n");
  }
}

/* ================================================== */

void
output(const char *lojban, const char *trans, const char *selmao)
{
  if (do_latex) {
    printf ("\\begin{tabular}[t]{l}"
            "\\textbf{\\footnotesize %s}\\\\\n"
            "\\textrm{\\footnotesize %s}\\\\\n"
            "\\textit{\\footnotesize %s}\n"
            "\\end{tabular}\n"
            "\\rule{0in}{1.0\\baselineskip}",
            lojban, selmao, trans);
  } else {
    printf ("%s <%s> [%s] ", lojban, selmao, trans);
  }
}
/* ================================================== */

void
output_newline(void)
{
  if (do_latex) {
    printf("\n\n");
  } else {
    printf("\n");
  }
}

/* ================================================== */

void
output_paren(const char *text)
{
  if (do_latex) {
    printf ("\\textrm{\\footnotesize %s}", text);
  } else {
    printf ("(%s) ", text);
  }
}


