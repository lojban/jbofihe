#
# Script to make the gismu places table
#
# COPYRIGHT

$_=<>;
while(<>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    $gloss = substr($_, 61, 157-61);
    if (length($gis) == 5) {
	if ($gloss =~ / x5 /) {
	    $high = 5;
	} elsif ($gloss =~ / x4 /) {
	    $high = 4;
	} elsif ($gloss =~ / x3 /) {
	    $high = 3;
	} elsif ($gloss =~ / x2 /) {
	    $high = 2;
	} elsif ($gloss =~ / x1 /) {
	    $high = 1;
	}
	print "#\n# ".$gloss."\n";
	for ($i=1; $i<=$high; $i++) {
	    print "#".$gis.$i.":DSAP;\n";
	}
    }
}
