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

@dirs=split(/:/, $ENV{"PATH"});
try_dirs:
    for $dir (@dirs) {
	if ($dir =~ m,^(.*)\/bin$,) {
	    $base = $1;
	    $incl = $base."/include";
	    if (-r $incl."/gdbm.h") {
		$incl_dir = $incl;
		$lib_dir = $base."/lib";
		last try_dirs;
	    }
	}
    }

die "Can't find gdbm.h.  You need to install the GNU gdbm library\n" unless ($incl_dir);

open(IN, "<Makefile.in");
open(OUT, ">Makefile");
while (<IN>) {
    s/\@\@GDBM-LIBRARY\@\@/$lib_dir/e;
    s/\@\@GDBM-INCLUDE\@\@/$incl_dir/e;
    print OUT;
}
close(IN);
close(OUT);

