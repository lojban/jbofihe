# $Header$
#
# Create dictionary source for the 4 letter rafsi
#
# COPYRIGHT

$_=<>;
while(<>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    if (length($gis) == 5) {
	$eng = substr($_,19,20);
	$eng =~ s/([^ ])[ ]*$/$1/;
	print "%".substr($gis,0,4).":".$eng."\n";
    }
}

