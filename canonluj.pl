#!/usr/bin/env perl
# $Header$
#
# Create insert for canonluj.c with the rafsi->gismu table.
#
# COPYRIGHT

%rafsi = ();
while (<STDIN>) {
    chomp;
    m/^([^ ]+) +([^ ]+) / || die "Unmatched [$_]";
    my $r = $1;
    my $g = $2;
    $rafsi{$r} = $g;
    if (length $g == 5) {
	$rafsi{substr($g,0,4)} = $g;
    }
}

for my $r (sort keys %rafsi) {
    print "{ \"".$r."\", \"".$rafsi{$r}."\"},\n";
}
