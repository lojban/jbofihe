# $Header$
#
# Generate basic translations for the rafsi
#
# COPYRIGHT

while (<>) {
    $raf = substr($_,0, 5);
    $raf =~ s/([^ ])[ ]*$/$1/;
    $eng = substr($_,12);
    $eng =~ s/([^ ])[ ]*$/$1/;
    print "%".$raf.":".$eng;
}
