#!/usr/bin/env perl

# $Header$

# Perl package for supporting regression tests.

local *LOG;
my $inited = 0;

#######################################################################

sub putfile {
    my ($text, $name) = @_;
    open (OUT, ">$name") || die "Can't open $name for output";
    print OUT $text;
    close(OUT);
}

#######################################################################

sub copy {
    my ($from, $log) = @_;
    open (IN, "<$from") || die "Can't open $from for input";
    while (<IN>) {
        print $log $_;
    }
    close (IN);
}

#######################################################################

sub preen {
    my ($x) = @_;
    $x =~ s/\t/ /go;
    $x =~ s/^ +//o;
    $x =~ s/ +$//o;
    return $x;
}

#######################################################################

sub openlog {
    my $name = $ARGV[0];
    if (defined $name) {
        open (LOG, ">$name");
    } else {
        open (LOG, ">&STDOUT");
    }
}


#######################################################################

sub test {
    local %_ = @_;

    if (!$inited) {
        $inited = 1;
        &openlog();
    }

    my ($program, $stdin, $inputs, $outputs, $notes, $expect, $pid);

    $program = $_{prog};
    $program = $_{program} unless (defined $program);
    $program = $prog       unless (defined $program);
    die "No program defined" unless (defined $program);

    $stdin = $_{stdin};
    $stdin = $_{text} unless (defined $stdin);
    $stdin = $_{in}   unless (defined $stdin);
    
    $inputs = $_{inputs};
    $outputs = $_{outputs};

    $expect = $_{expect};
    $notes = $_{notes};
    $notes = $_{note} unless (defined $notes);

    if (defined $stdin) {
        &putfile($stdin, "__STDIN");
    }

    my $testsep = ('#' x 60)."\n";
    my $bigsep = ('=' x 40)."\n";
    my $medsep = ('-' x 25)."\n";
    my $sep = ('-' x 10)."\n";

    print LOG $testsep;
    print LOG "PROGRAM : $program\n";
    print LOG $bigsep;
    if (defined $stdin) {
        print LOG "STDIN :\n";
        print LOG $sep;
        print LOG $stdin."\n";
        print LOG $medsep;
    }

    if (defined $inputs) {
        $inputs = &preen($inputs);
        my @inputs = split / +/, $inputs;
        foreach my $x (@inputs) {
            print LOG "$x\n";
            print LOG $sep;
            &copy($x, *LOG);
            print $medsep;
        }
    }

    print LOG $bigsep;

    if (defined $notes) {
        print LOG "NOTE : ";
        print LOG $notes."\n";
        print LOG $bigsep;
    }

    # Now actually do the run
    if (defined $stdin) {
        open (STDIN, "<__STDIN");
    }
    open (STDOUT, ">__STDOUT");
    open (STDERR, ">__STDERR");

    $program =~ s/\'/\\'/go;
    my $cmd = "/bin/sh -c \"$program\"";
    system ($cmd);

    my $status = $? >> 8;
    my $dumped_core = $? & 128;
    my $signal = $? & 127;

    print LOG "Exit status : $status\n";
    if (defined $expect && $status != $expect) {
        print LOG "  ###FAILURE### ---> EXPECTED STATUS WAS $expect\n";
    }
    print LOG "Core dumped : ", ($dumped_core ? "YES" : "NO"), "\n";
    print LOG "Signal      : $signal\n";

    print LOG $bigsep;

    foreach my $x ("__STDOUT", "__STDERR", @outputs) {
        print LOG "$x\n";
        print LOG $sep;
        &copy($x, *LOG);
        print LOG $medsep;
    }

    unlink("__STDIN");
    unlink("__STDOUT");
    unlink("__STDERR");
    
}
    

#######################################################################

1;

