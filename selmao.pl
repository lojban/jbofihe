#!/usr/bin/env perl
# $Header$
#
# Create dictionary source mapping cmavo to selmao
#
# COPYRIGHT

$junk = <STDIN>;
while (<STDIN>) {
    chomp;
    $cmavo = substr($_, 0, 10);
    $selmao = substr($_, 11, 9);
    $cmavo =~ s/^\.//;
    $cmavo =~ s/ +//g; # Squash all runs of spaces.  Not sure if this is best?
    $selmao =~ s/^ +//;
    $selmao =~ s/ +$//;
    print "+".$cmavo.":".$selmao."\n";
}
