#!/usr/bin/env perl
# $Header$
# Process the NORALUJV.txt file to generate dictionary input
#
# COPYRIGHT

if ($#ARGV < 0) {
    $dir="./";
} else {
    $dir = $ARGV[0];
}

open (IN, "<".$dir."NORALUJV.txt") || die "Could not open NORALUJV.txt";
@lines = <IN>;
close (IN);

foreach $x (@lines) {
    chomp $x;
    $lujvo = substr($x, 0, 30);
    $lujvo =~ s/ +$//;
    $rest = substr($x, 31);
    $rest =~ m/^([^:]*):([^:]*):(.*)$/ || next;
    $makeup = $1;
    $trans = $2;
    $plac = $3;
    $plac =~ s/\([^\)]+\)//g;
    @plac = split(/,/, $plac);

    $trans =~ s/^ +//;
    $trans =~ s/ +$//;

    print $lujvo."1:".$trans."\n";
    
    @places = ();
    foreach $p (@plac) {
	if ($p =~ m/x([1-9]) = ([a-z\']+[12345])/) {
	    $place = $1;
	    $gismu = $2;
	    $gplace = $3;
	    
	    if ($place > 1) {
		$places[$place] = $gismu.$gplace;
		print $lujvo.$place.":\@".$gismu.$gplace."\n";
	    }
	} else {
	}
    }
}
