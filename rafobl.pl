# $Header$
#
# Generate places for the rafsi
#
# COPYRIGHT

%obl=();
open(OBL, "<oblique.key");
while (<OBL>) {
    chop;
    if (/^([^;]+);([^;]+)$/) {
	$gisnum = $1;
	$meaning = $2;
    } elsif (/^([^;]+);([^;]+);.*$/) {
	$gisnum = $1;
	$meaning = $2;
    } else {
	print STDERR "Unmatched [$_]\n";
    }

    $meaning =~ s/([^ ])[ ]*$/$1/;
    $obl{$gisnum} = $meaning;
}
close(OBL);

open(GIS, "<gismu");
$_=<GIS>;
while (<GIS>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    if (length($gis) == 5) {
	$eng = substr($_,19,20);
	$eng =~ s/([^ ])[ ]*$/$1/;
	
	$raf1 = substr($_,6,3);
	if ($raf1 =~ /[a-z]+/) {
	    for ($i=1;$i<=5;$i++) {
		$gg = $gis.$i;
		if ($mng = $obl{$gg}) {
		    print "%".$raf1.$i.":".$mng."\n";
		}
	    }
	}

	$raf2 = substr($_,10,3);
	if ($raf2 =~ /[a-z]+/) {
	    for ($i=1;$i<=5;$i++) {
		$gg = $gis.$i;
		if ($mng = $obl{$gg}) {
		    print "%".$raf2.$i.":".$mng."\n";
		}
	    }
	}

	$raf3 = substr($_,14,4);
	$raf3 =~ s/([^ ]) *$/$1/;
	if ($raf3 =~ /[a-z\']+/) {
	    for ($i=1;$i<=5;$i++) {
		$gg = $gis.$i;
		if ($mng = $obl{$gg}) {
		    print "%".$raf3.$i.":".$mng."\n";
		}
	    }
	}


    }
}
