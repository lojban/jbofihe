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
		print "Found gdbm.h in ".$incl.", using that.\n";
		last try_dirs;
	    }
	}
    }

die "Can't find gdbm.h.  You need to install the GNU gdbm library\n" unless ($incl_dir);

sub check_wordlists {
    my ($dir) = @_;
    if ((-r $dir."/gismu") && (-r $dir."/lujvo-list") && (-r $dir."/cmavo") && (-r $dir."/oblique.key") && (-r $dir."/rafsi")) {
	print "Found word lists in directory $dir, using that.\n";
	return 1;
    } else {
	return 0;
    }
}


# Try to find the directory containing the files 'gismu', 'cmavo', 'oblique.key' etc
try_wordlists:
    if (&check_wordlists(".")) {
	$word_list_dir = ".";
    } else {
	# Try all directories below the parent directory
	@dirs = qx=find .. -type d -print=;
	for $dir (@dirs) {
	    if (&check_wordlists($dir)) {
		$word_list_dir = $dir;
		last try_wordlists;
	    }
	}

	# Otherwise, try everything below the user's home directory
	@dirs = qx=find ~ -type d -print=;
	for $dir (@dirs) {
	    if (&check_wordlists($dir)) {
		$word_list_dir = $dir;
		last try_wordlists;
	    }
	}

	dir "Can't find word lists gismu, cmavo etc in your directory structure.\n";
    }



open(IN, "<Makefile.in");
open(OUT, ">Makefile");
while (<IN>) {
    s/\@\@GDBM-LIBRARY\@\@/$lib_dir/e;
    s/\@\@GDBM-INCLUDE\@\@/$incl_dir/e;
    s/\@\@WORD-LISTS\@\@/$word_list_dir/e;
    print OUT;
}
close(IN);
close(OUT);

