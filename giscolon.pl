# $Header$
#
# Create dictionary source for the basic gismu list.
#
# COPYRIGHT

$_=<>;
while(<>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    if (length($gis) == 5) {
	$eng = substr($_,19,20);
	$eng =~ s/([^ ])[ ]*$/$1/;
	print $gis.":".$eng."\n";
    }
}

