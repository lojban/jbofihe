#!/usr/bin/env perl

# $Header$

# Accept a list of tests as command arguments, run them and check the results.

# COPYRIGHT

#######################################################################
#
# Usage notes :
#
# Each command line arg should be the path to a file whose name ends in .in
# The contents of each such file should be something like :
#
# require "testpak.pl";
# &test(prog=>"jbofihe -x -b", stdin => "mi klama le zdani";
#
# Multiple &test lines can appear.
#
# The specified program will be run with the specified arguments and text on
# its standard input.  The results (exit status + data written to stdout and
# stder) will be gathered in a file with the .out suffix replacing .in.  If a
# file with the .ref suffix exists, .out and .ref are compared to determine
# whether the test has passed.  The first time a new test is run, the .out file
# is manually checked and moved to the .ref file if acceptable.  This way,
# regression failures can be detected.
#######################################################################

$| = 1; # Autoflush stdout

$here = $0;
$here =~ s=/[^/]+$==;

$ENV{JBOFIHE_DICTIONARY} = "$here/../smujmaji.dat";
$ENV{PATH} = "$here/..:".$ENV{PATH};

while ($test = shift @ARGV) {
    unless (-r $test) {
        print "No such test [$test]\n";
        next;
    }

    $base = $test;
    $base =~ s/\.in$//o;
    $out = $base.".out";
    $ref = $base.".ref";

    print "Running $base .. ";
    $cmd = "perl -I$here $test $out";
    system ($cmd);

    if (-r "$ref") {
        system ("diff $out $ref > /dev/null");
        $status = $? >> 8;
        if ($status == 0) {
            print "passed\n";
            unlink $out; # No need to keep this if all is OK
        } elsif ($status == 1) {
            print "FAILED\n";
        } elsif ($status == 2) {
            print "COULD NOT DIFF RESULTS\n";
        }
    } else {
        system ("egrep '###FAILURE###' $out > /dev/null");
        $status = $? >> 8;
        if ($status == 0) {
            print "NO REFERENCE, SOME CASES FAILED\n";
        } else {
            print "NO REFERENCE RESULT\n";
        }
    }

}

