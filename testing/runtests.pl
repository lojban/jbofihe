#!/usr/bin/env perl

# $Header$

# Accept a list of tests as command arguments, run them and check the results.

$| = 1; # Autoflush stdout

$here = $0;
$here =~ s=/[^/]+$==;

$ENV{JBOFIHE_DICTIONARY} = "/home/richard/cvswork/jbofihe/smujmaji.dat";
$ENV{PATH} = "/home/richard/cvswork/jbofihe:".$ENV{PATH};

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
        print "NO REFERENCE RESULT\n";
    }

}

