#!/usr/bin/env perl

# $Header$

# Script to build dictionary compiler inputs from gismu and cmavo files.

# COPYRIGHT

open (IN, "<reduced_gismu") || die "Cannot open reduced_gismu";
while (<IN>) {
    chomp;
    $gis = substr($_,0,6);
    $gis =~ s/[ \t]+$//o;
    $eng = substr($_,19,20);
    $eng =~ s/([^ ])[ ]*$/$1/;
    print $gis.":".$eng."\n";
    $raf = substr($_, 6, 12);
    $raf =~ s/[ \t]+/ /og;
    $raf =~ s/^ //o;
    $raf =~ s/ $//o;
    @raf = split / /, $raf;
    for $r (@raf) {
        print "%".$r.":".$eng."\n";
        print "@".$r.":".$gis."\n";
    }
    if (length($gis) == 5) {
        $raf4 = substr($gis,0,4);
        print "%".$raf4.":".$eng."\n";
        print "@".$raf4.":".$gis."\n";

    }
}

close (IN);

open (IN, "<reduced_cmavo") || die "Cannot open reduced_cmavo";
while(<IN>) {
    chomp;
    $cma = substr($_,0,10);
    $cma =~ s/[ \t]+$//o;
    $cma =~ s/^[ \t]+//o;
    $cma =~ s/^\.//;
    $sma = substr($_,10,6);
    $sma =~ s/[ \t]+$//o;
    $sma =~ s/^[ \t]+//o;
    $sma =~ s/^\.//;
    $eng = substr($_,20,40);
    $eng =~ s/[ \t]+$//o;

    print $cma.":".$eng."\n";
    print "+".$cma.":".$sma."\n";

}
close(IN);


