# $Header$
#
# Create mapping from 4 letter rafsi to gismu
#
# COPYRIGHT

$_=<>;
while(<>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    if (length($gis) == 5) {
	print "@".substr($gis,0,4).":".$gis."\n";
    }
}
