# $Header$
#
# Create a rough and ready lujvo places dictionary, with scope for
# editing the contents.
#
# COPYRIGHT

if ($#ARGV < 0) {
    $dir="./";
} else {
    $dir = $ARGV[0];
}

%places = ();

open(OBL, "<".$dir."oblique.key");
while (<OBL>) {
    /([a-z]{5}[12345])\;(.*)$/;
#    print $1." ---> ".$2."\n";
    $places{$1} = $2;
}
close(OBL);

%gismu = ();
%meaning = ();
open(GIS, "<".$dir."gismu");
while (<GIS>) {
    $gis = substr($_,0,6);
    $gis =~ s/([^ ])[ ]*$/$1/;
    $meaning = substr($_,19,20);
    $meaning =~ s/([^ ])[ ]*$/$1/;
    $meaning{$gis} = $meaning;
    $gloss = substr($_, 61, 157-61);
    $gloss =~ s/([^ ])[ ]*$/$1/;
    $gismu{$gis} = $gloss;
}
close(GIS);

while (<>) {
    if (/([^#:]+)[#:]([^:]+):([^:]+):(.*)$/) {
	   $lv = $1;
	   $dc = $2;
	   @dc=split(/\+/, $dc);
	   $tr = $3;
	   $p = $4;
	   $p =~ s/[\(\[].*$//;
	   @p = split(/ +/, $p);
	   print "\n\n";
	   print "####################\n";
	   print "# Lujvo  : $lv\n";
	   print "# Basis  : $dc\n";
	   print "# Expn   : ";
	   $first=1;
	   for $w (@dc) {
	       print "+" unless ($first);
	       $first = 0;
	       print $meaning{$w};
	   }
	   print "\n";
	   print "# Defn   : $tr\n";
	   print "# Places : $p\n";
	   print "#\n";
	   %found_gis = ();
	   for $d (@dc) {
	       if ($gismu{$d}) {
		   $found_gis{$d} = 1;
	       }
	   }
	   for $p (@p) {
	       $tp = $p;
	       $tp =~ s/=//;
	       $tp =~ s/^\$//;
	       if ($tp =~ /^([a-z]{5})([12345])/) {
		   $gg = $1;
#		   print "#   ".$tp." : ".$places{$tp}."\n";
		   $found_gis{$gg} = 1;
	       }
	   }
	   print "#\n";
	   for $gis (sort keys %found_gis) {
	       print "#  ".$gis." : ".$gismu{$gis}."\n";
	   }
	   print "#\n";

	   $tr =~ s/;.*$//;
	   $pl=1;
	   for $p (@p) {
	       if ($p =~ /^\$=/) {
		   if ($p =~ /^\$\=([a-z\']+)([12345])/) {
		       $g = $1;
		       $gp = $2;
		       print ", ".$g.$gp." (".$places{$g.$gp}.")";
		   }
	       } else {
		   print "\n".$lv.$pl.":";
		   if ($p =~ /^\$([a-z\']+)([12345])/) {
		       $g = $1;
		       $gp = $2;
		       if ($pl == 1) {
			   print $tr.": ".$g.$gp." (".$places{$g.$gp}.")";
		       } else {
			   print "@".$g.$gp.": ".$g.$gp." (".$places{$g.$gp}.")";
		       }
		       $pl++;
		   }
	       }
	   }
	   print "\n";
	   
       } else {
	   print STDERR "Unmatched ".$_;
       }
      }

