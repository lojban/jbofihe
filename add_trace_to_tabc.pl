#!/usr/bin/env perl

# $Header$

# Script to take the x.tab.c output from Bison, and insert extra function calls
# into it to support the following :

# - detection of possible elidable terminators
# - backtracing on syntax errors

# Hopefully, this changes things that are not *too* specific to particular
# versions of bison.  It was originally developed against output from bison
# version 1.27.

# COPYRIGHT

print <<EOF;
extern elide_trace_reduce(int, int);
extern elide_trace_shift(int);
extern elide_trace_register(const short*, const short*, const short*, const short*);
extern report_trace_shift(int);
extern report_trace_reduce(int, int);
extern report_trace_error(short *yyss, short *yyssp);
EOF

while (<>) {

    # Force type of yytranslate, otherwise we won't have a fixed type to
    # declare it as in the elide code.
    
    s/static const char yytranslate/static const short yytranslate/o;

    if (m{/\* Shift the lookahead}o) {
        print;
        print "elide_trace_shift(yychar);\n";
        print "report_trace_shift(yychar);\n";
    } elsif (m{^\s*yyreduce\:}o) {
        print;
        print "elide_trace_reduce(yystate, yyn);\n";
        print "report_trace_reduce(yystate, yyn);\n";
        
    } elsif (m{^\s*yyerrlab\:}o) {
        print;
        print "report_trace_error(yyss, yyssp);";

    } elsif (m{/\* Initialize stack pointers}o) {
        print "elide_trace_register(yypact, yytranslate, yycheck, yytable);\n";
        print;
    } else {
        print;
    }
}

        
