#!/usr/bin/env perl

# $Header$

# Script to read in a load of dictionary data, sort it, and write it out in the
# form of a C structure initialisation.  This can be used for embedding the
# dictionary within the software binary itself.

# COPYRIGHT

%def = ();

while (<>) {
chomp;
    if (/^[ \t]*#/) {
        next;
    }

    if (/^([^:]+):([^:]+)/) {
        $def{$1} = $2;
    }
}

$n = 0;
print "static Keyval dict[] = {";
for $x (sort keys %def) {
    print "," if ($n++);
    print "\n  ";
    print "{ \"".$x."\", \"".$def{$x}."\"}";
}
print "};\n\n";
print "static int n_entries = $n;\n\n";
    
