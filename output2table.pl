#!/usr/bin/env perl

# Read the .output file from bison, and construct a table encoding the
# rule/state information for use by the traceback feature.

# $Header$

# COPYRIGHT

$prefix = shift || die "No prefix specified";

while (<>) {
    last if (/^Grammar/);
}

@lhs = ();
@rhs = ();
@rhsperrule = ();
%codes = ();
@decodes = ();
$hicode = 0;

while (<>) {
    chomp;
    last if (/^Terminals, with rules/);
    next if (/^\s*$/);
    next if (/^\s*Number, Line, Rule/); # extra junk in recent bisons (1.31 onwards?)

    my $number = undef;
    my $lhs = undef;
    my $rhs = undef;

    if (m{^rule ([0-9]+) +([a-zA-Z0-9_]+) \-\>[ \t](.*)$}) {
        $number = $1;
        $lhs = $2;
        $rhs = $3;
    } elsif (m{^\s+([0-9]+)\s+[0-9]+\s+([a-zA-Z0-9_]+) \-\>[ \t](.*)$}) {
        $number = $1;
        $lhs = $2;
        $rhs = $3;
    }

    die "Unmatched rule [$_]" unless (defined $number);

    if ($rhs =~ m{/\* empty \*/}) {
        @r = ();
    } else {
        @r = split(/ /, $rhs);
    }
    $lhs[$number] = $lhs;
    $rhs[$number] = [ @r ];
    $rhsperrule[$number] = 1+$#r;
    if (!defined $codes{$lhs}) {
        $codes{$lhs} = $hicode++;
    }
}

# Discard one line after "Terminals" line
$_ = <STDIN>;

while (<>) {
    chomp;
    last if (/^\s*$/);
    next if (/^\s/);

    m{^([^ \t]+)} || die "Unmatched line [$_];";
    $codes{$1} = $hicode++;
}

for $c (keys %codes) {
    $decodes[$codes{$c}] = $c;
}

$state = 0;
@rules = (); # state -> array of rule numbers allowing shift
@focus = (); # state -> array of posn of '.' in rules allowing shift
@shift_toks = (); # state -> array of token names which can be shifted in that state
@state_toks = (); # state -> number of tokens which can shift in that state
@rulesperstate = (); # state -> number of rules with context in that state

while (<>) {
    chomp;
    if (/^state ([0-9]+)/) {
        $state = $1;
        $rules[$state] = [ ];
        $focus[$state] = [ ];
        $shift_toks[$state] = [ ];
        $rulesperstate[$state] = 0;
        $state_toks[$state] = 0;
        next;
    }

    if (/^[ \t]+([^ \t]+)[ \t]+\-\>[ \t]+(.+)[ \t]+\(rule[ \t]+([0-9]+)\)/) {
        $lhs = $1;
        $rhs = $2;
        $ruleno = $3;
        @r = split(/ /, $rhs);
        for ($i = 0; ; $i++) {
            last if ($r[$i] eq '.');
        }
        push (@{$focus[$state]}, $i);
        push (@{$rules[$state]}, $ruleno);
        ++$rulesperstate[$state];
    }

    if (/^\s+([^ \t]+)\s+shift, and go to state ([0-9]+)/) {
        push (@{$shift_toks[$state]}, $1);
        ++$state_toks[$state];
    }
        
}

# Write token/nonterm table out

print "static char *".$prefix."_toknames[] = {\n";
for $c (0 .. $#decodes) {
    print "\"".$decodes[$c]."\",\n";
}
print "};\n";

# Write rule index table out
$n=0;
$t = 0;
print "static unsigned short ".$prefix."_ruleindex[] = {\n";
for ($s=0;$s<=$#lhs;$s++) {
    print ", " unless ($n==0);
    printf "%d", $t;
    $n++;
    print "\n" if ($n%10 == 0);
    $t += $rhsperrule[$s];
}
print "};\n";

# Write rule lhs table out
$n=0;
print "static unsigned short ".$prefix."_rulelhs[] = {\n";
for ($s=0;$s<=$#lhs;$s++) {
    print ", " unless ($n==0);
    printf "%d", $codes{$lhs[$s]};
    $n++;
    print "\n" if ($n%10 == 0);
}
print "};\n";

# Write rule rhs table out
$n=0;
print "static unsigned short ".$prefix."_rulerhs[] = {\n";
for ($s=0;$s<=$#lhs;$s++) {
    for ($i=0;$i<=$#{$rhs[$s]};$i++) {
        print ", " unless ($n==0);
        printf "%d", $codes{$rhs[$s][$i]};
        $n++;
        print "\n" if ($n%10 == 0);
    }
}
print "};\n";
# Write state index table out
$n=0;
$t = 0;
print "static unsigned short ".$prefix."_stateindex[] = {\n";
for ($s=0;$s<=$state+1;$s++) {
    print ", " unless ($n==0);
    printf "%d", $t;
    $n++;
    print "\n" if ($n%10 == 0);
    $t += $rulesperstate[$s];
}
print "};\n";



# Write focus table out
$n=0;
print "static unsigned char ".$prefix."_focus[] = {\n";
for ($s=0;$s<=$state;$s++) {
    for ($i=0;$i<=$#{$focus[$s]};$i++) {
        print ", " unless ($n==0);
        printf "%d", $focus[$s][$i];
        $n++;
        print "\n" if ($n%10 == 0);
    }
}
print "};\n";

# Write rule_of_state table out
$n=0;
print "static unsigned short ".$prefix."_shiftrule[] = {\n";
for ($s=0;$s<=$state;$s++) {
    for ($i=0;$i<=$#{$rules[$s]};$i++) {
        print ", " unless ($n==0);
        printf "%d", $rules[$s][$i];
        $n++;
        print "\n" if ($n%10 == 0);
    }
}
print "};\n";

# Write shift_toks table out
$t = 0;
$n = 0;
print "static unsigned short ".$prefix."_shift_in_state_index[] = {\n";
for ($s=0;$s<=$state+1;$s++) {
    print ", " unless ($n==0);
    printf "%d", $t;
    $n++;
    print "\n" if ($n%10 == 0);
    $t += $state_toks[$s];
}
print "};\n";

# Write table of tokens which can shift in each state
$n=0;
print "static unsigned short ".$prefix."_shift_in_state[] = {\n";
for ($s=0;$s<=$state;$s++) {
    for ($i=0;$i<=$#{$shift_toks[$s]};$i++) {
        print ", " unless ($n==0);
        printf "%d", $codes{$shift_toks[$s][$i]};
        $n++;
        print "\n" if ($n%10 == 0);
    }
}
print "};\n";


    
