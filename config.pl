# $Header$
#
# To be run with perl.  This builds Makefile from Makefile.in,
# substituting things whose location varies on different systems, and
# has to be looked for.
#
# There is no #! at the top because perl isn't installed in the same
# place on all systems.
#
# COPYRIGHT

$prefix="/usr/local";
$install="ginstall";
$debug=0;
$profile=0;
$mmap=1;
$embed=0;

while ($_ = shift @ARGV) {
    if (/^--prefix=(.*)$/) {
        $prefix = $1;
    } elsif (/^-p/) {
        $prefix = shift @ARGV;
    } elsif (/^--installprog=(.*)$/) {
        $install = $1;
    } elsif (/^--profile$/) {
		$profile = 1;
    } elsif (/^--debug$/) {
		$debug = 1;
	} elsif (/^--nommap$/) {
        $mmap = 0;
    } elsif (/^--embed$/) {
        $embed = 1;
    }
}

$optdebug = $debug ? "-g -Wall" : "-O2";
if ($profile) {
    $optdebug .= " -pg";
}
$mmap_flag = $mmap ? "-DHAVE_MMAP=1" : "";
$defines = $mmap_flag;
if ($debug) {
    $defines .= " -DEXPOSE_SIGNALS";
}

if ($embed) {
    $dictdata_c = "dictdata.c";
    $defines .= " -DEMBEDDED_DICTIONARY";
} else {
    $dictdata_c = "";
}
    

open(IN, "<Makefile.in");
open(OUT, ">Makefile");
while (<IN>) {
    s/\@\@PREFIX\@\@/$prefix/eg;
    s/\@\@INSTALLPROG\@\@/$install/eg;
	s/\@\@OPTDEBUG\@\@/$optdebug/eg;
    s/\@\@DEFINES\@\@/$defines/eg;
    s/\@\@DICTDATA_C\@\@/$dictdata_c/eg;
    print OUT;
}
close(IN);
close(OUT);

