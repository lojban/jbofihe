#!/usr/bin/env perl
# $Header$
#
# Create insert for canonluj.c with the rafsi in the gismu table.
#
# COPYRIGHT

%rafsi = ();
while (<STDIN>) {
    chomp;
    $gismu = substr($_,0,5);
    $gismu =~ s/ +$//;
    $rafsi = substr($_,6,12);
    $rafsi =~ s/ +/ /g;
    $rafsi =~ s/^ //;
    $rafsi =~ s/ $//;
    foreach (split / /,$rafsi) {
	$rafsi{$_} = $gismu;
    }
    if (length $gismu == 5) {
	$rafsi{substr($gismu,0,4)} = $gismu;
    }
}

for my $r (sort keys %rafsi) {
    print "{ \"".$r."\", \"".$rafsi{$r}."\"},\n";
}
