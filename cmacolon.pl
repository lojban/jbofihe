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
    $sma = substr($_,10,6);
    $eng = substr($_,20,40);
    $eng =~ s/([^ ])[ ]*$/$1/;
    if ($cma =~ /^[aeiou]\'?[aeiou]/) {
	print $cma.":{..".$eng."}\n";
    } elsif ($sma =~ /^[ ]*UI/) {
	print $cma.":{..".$eng."}\n";
    } else {
	print $cma.":".$eng."\n";
    }
}

