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
$debug=0;
$profile=0;
$mmap=1;
$plist=0;
$embed=0;
$plist=0;

while ($_ = shift @ARGV) {
    if (/^--help/ || /^-h/) {
        &print_help();
        exit 0;
    } elsif (/^--prefix=(.*)$/) {
        $prefix = $1;
    } elsif (/^-p/) {
        $prefix = shift @ARGV;
    } elsif (/^--profile$/) {
		$profile = 1;
    } elsif (/^--plist$/) {
        $plist = 1;
    } elsif (/^--debug$/) {
		$debug = 1;
	} elsif (/^--nommap$/) {
        $mmap = 0;
    } elsif (/^--embed$/) {
        $embed = 1;
    }
}

if ($plist) {
    $defines .= " -DPLIST";
    $cmafihe_ldopts="-lPropList";
} else {
    $cmafihe_ldopts="";
}

$optdebug = $debug ? "-g -Wall" : "-O2";
if ($profile) {
    $optdebug .= " -pg";
}
$mmap_flag = $mmap ? "-DHAVE_MMAP=1" : "";
$defines = $mmap_flag;
if ($debug) {
if ($plist) {
    $defines .= " -DPLIST";
}

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
while (<IN>) 
{
    s/\@\@PREFIX\@\@/$prefix/eg;
    s/\@\@OPTDEBUG\@\@/$optdebug/eg;
    s/\@\@CMAFIHE_LDOPTS\@\@/$cmafihe_ldopts/eg;
    s/\@\@DEFINES\@\@/$defines/eg;
    s/\@\@DICTDATA_C\@\@/$dictdata_c/eg;
    print OUT;
}
close(IN);
close(OUT);


sub print_help {

    print <<EOF;
Configuration script for jbofihe & friends

--prefix=<prefix>    Set installation directory parent (default=/usr/local)
-p <prefix>          Ditto
--profile            Build for profiling
--debug              Build a debuggable version
--nommap             Don't use mmap for reading the dictionary file
--embed              Embed minimal dictionary directly into jbofihe program
EOF
}

