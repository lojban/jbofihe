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
    print unless (m~/\* ET \*/~);
}
