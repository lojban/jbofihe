# $Header$
#
# Remove rules from the grammar that have elidable terminators elided.
# The resulting grammar can be run through bison to see how many
# shift/reduce conflicts remain.
#
# COPYRIGHT

while (<>) {
    print;
    last if (/^\%\%$/);
}

while (<>) {
    s,/\* SG \*/,START_GEK,;
    print unless (m~/\* ET [A-Za-z]+~);
    if (m~/\* ET ~ && !m~/\* ET [A-Za-z]+~) {
        chomp;
        print STDERR "Line with odd ET construction [$_]\n";
    }
}
