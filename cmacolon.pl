# $Header$
#
# Create dictionary source for the cmavo.
#
# COPYRIGHT

$_=<>;
while(<>) {
    $cma = substr($_,0,10);
    $cma =~ s/([^ ])[ ]*$/$1/;
    $cma =~ s/^[ ]*([^ ])/$1/;
    $cma =~ s/^\.//;
    $eng = substr($_,20,40);
    $eng =~ s/([^ ])[ ]*$/$1/;
    print $cma.":".$eng."\n";
}

