# $Header$
#
# Generate mapping from 3 letter rafsi to gismu
#
# COPYRIGHT

while (<>) {
    $raf = substr($_,0, 5);
    $raf =~ s/([^ ])[ ]*$/$1/;
    $gis = substr($_,5, 5);
    print "@".$raf.":".$gis."\n";
}
