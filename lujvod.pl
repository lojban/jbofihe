# $Header$
#
# Create dictionary source for the lujvo-list.
#
# COPYRIGHT

while (<>) {
    if (/([^#:]+)[#:]([^:]+):([^:]+)/) {
        print $1.":".$3."\n";
    } else {
        print STDERR "Unmatched ".$_;
    }
}
