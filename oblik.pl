# $Header$
#
# Create dictionary source for the gismu places.
#
# COPYRIGHT

while (<>) {
    s/^([^;]+);([^;]+)$/$1:$2/;
    s/^([^;]+);([^;]+);.*$/$1:$2/;
    print;
}
