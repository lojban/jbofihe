#!/usr/bin/env perl
#
# $Header$

# Perl script for chaining jvocu'adju and vlatai together to look for failures on random input.

# Copyright 2001  Richard P. Curnow
# LICENCE

@gc = ();
$| = 1; # autoflush
open (IN, "<../reduced_gismu");
while (<IN>) {
    chomp;
    m{^([^ \t]+)[ \t]}o;
    $word = $1;
    $word =~ s/\'/\\'/o;
    push (@gc, $word);
}
close (IN);

srand;

$ngc = 1 + $#gc;
$kk = 0;

main_loop:
while (1) {
    ++$kk;
    if (($kk%500) == 0) {
        print "$kk\n";
    }
    
    @args = ();
    $shown_args = 0;
    $n = 2 + int (rand(6));
    for $i (1 .. $n) {
        while (1) {
            # For final component, can't have a cmavo, unless that cmavo has a
            # rafsi ending in a vowel.  Don't know that in here, so be
            # pessimistic.
            $e = int(rand $ngc);
            $w = $gc[$e];
            
            # Lujvo canonicaliser in vlatai reverses brody to brodu always, so
            # lots of bogus failures will occur unless the other four are avoided.
            next if ($w =~ /brod[aeio]/o);
            
            if (($i < $n) || (length ($w) == 5)) {
                push (@args, $gc[$e]);
                last;
            }
        }
    }

    @lujvo = (); # alleged lujvo for this tanru
    $args = join(" ", @args);

    open (IN, "../jvocuhadju -l -a $args 2>&1 |");
    while (<IN>) {
        chomp;
        if (/unmatched/io) {
            # Due to picking a cmavo as the last tanru component where that cmavo
            # has no rafsi ending in a vowel.
            next main_loop;
        }
        
        if (/^[ \t]+[0-9]+[ \t]+([^ \t]+)/) {
            push (@lujvo, $1);
        }
    }
    close (IN);

    open (OUT, ">temp_vt.in");
    for $i (@lujvo) {
        print OUT "$i\n";
    }
    close (OUT);
        
    open (IN, "../vlatai -el < temp_vt.in |");
    while (<IN>) {
        chomp;
        m{^([^:]+):([^:]+):(.+[^\s])\s+\[([^]]+)\]\s*$}o or die "Unmatched $_";
        $a = $1;
        $b = $2;
        $c = $3;
        $d = $4;
        $b =~ s/^ +//o;
        $b =~ s/ +$//o;
        $c =~ s/^ +//o;
        $c =~ s/ +$//o;
        $d =~ s/\+/ /go;
        $d =~ s/\'/\\'/go;
        if (($b ne "lujvo") ||
            ($c =~ / /) ||
            ($d ne $args)) {
            print "$args\n" unless ($shown_args);
            $shown_args = 1;
            print "###$b:$c\n";
        }
    }
    close(IN);

}

    

