#!/usr/bin/env perl

# $Header$

# Convert a nondeterminstic finite automaton (NFA) into a deterministic finite
# automaton (DFA).

# COPYRIGHT

#######################################################################
#
# The NFA is defined in terms of a set of states, with transitions between the
# states.  The transitions may occur on any one of a set of symbols (specified
# with | characters between the options), or may be 'epsilon' transitions, i.e.
# occurring without consumption of any input.  A state may have multiple
# transitions for the same input symbol (hence 'nondeterministic').  The final
# state encountered within the input file is the start state.  A state may be
# entered more than once in the file; the transitions in the multiple
# definitions are combined to give the complete transition set.  A state may
# have an exit value assigned (with =); this is the return value of the
# automaton if the end of string is encountered when in that state.  (If the
# resulting DFA can be in multiple exiting NFA states when the end of string is
# reached, the result is all the associated NFA exit values or'd together, so
# it is best to use distinct bits for NFA exit values unless it is known that
# is safe not to in a particular case.) The input grammar allows a BLOCK <name>
# ... ENDBLOCK construction + block instantiation.  This allows common parts of
# the NFA state machine to be reused in multiple places as well as aiding
# structuring and readability.  See morf_nfa.in for an example of the input
# grammar, and morf.c for an example of how to build the automaton around the
# tables that this script generates.
#
#######################################################################

# Each NFA state record consists of three arrays : transition symbol, name of
# target state, ref to target state (filled in later)

# Name -> (state_name -> state) mapping
%block = ();

# Ref to current block
$cblock = undef;

# Ref to last block
$lastblock = undef;

# Ref to last state in use in input file (this will be the initial state for
# the matching)
$laststate = undef;

# Name of current block being defined
$cbname = undef;

# Ref to current state for which transitions are being built
$cstate = undef;

# Map of all token symbols that are encountered.
%tokens = ();

# Tokens in user's order (as declared by TOKENS directive in input file),
# values 0 upwards (used for addressing into transition array for each state,
# so order + values MUST agree with lexer.)
@tokset=(); # map value to token
%tokmap=(); # map token to value

# Prefix to put before array names (override by PREFIX directive in input file)
$prefix="dfa";

# Loop to parse input file
while (<>) {
    s/\#.*$//o; # Strip comments
    chomp;
    if (/^\s*$/o) {
        next; # discard blank lines
    } elsif (/^\s*TOKENS\s+(.*)\s*$/io) {
        $toks = $1;
        $toks =~ s/[ \t]+/ /g;
        $toks =~ s/^ //g;
        $toks =~ s/ $//g;
        @tokset = split(/ /, $toks);
        for $i (0 .. $#tokset) {
            $tokmap{$tokset[$i]} = $i;
        }
    } elsif (/^\s*PREFIX\s+(\w+)/io) {
        $prefix = $1;
    } elsif (/^\s*BLOCK\s+(\w+)/io) {
        $name = $1;
        if (defined $cblock) {
            die "Attempt to start a new block '$name' whilst still inside block '$cbname' (missing ENDBLOCK?)";
        }
        if (defined $block{$name}) {
            die "Already have a block called '$name'";
        }
        $block{$name} = {}; # New state_name->state hash
        $cblock = $block{$name};
        $cbname = $name;
    } elsif (/^\s*(\w+)\s*\:\s*(\w+)/o) {
        $instance_name = $1;
        $block_type = $2;
        if ($block_type eq $cbname) {
            die "Attempt to instantiate block '$cbname' within itself";
        }
        # Instantiate an already defined block inside the new one
        $iblock = $block{$block_type};
        for $s (keys %$iblock) {
            $st = $iblock->{$s};
            $newname = $instance_name.".".$st->{name};
            $cblock->{$newname} = { name => $newname,
                                    tok => [ @{$st->{tok}} ],
                                    sname => [ map { $instance_name.".".$_; } @{$st->{sname}} ] };
            if (defined $st->{exival}) {
                $cblock->{$newname}->{exival} = $st->{exival};
            }
        }

        # No need to fix up sref - this gets done later along with the new
        # states added in this block

    } elsif (/^\s*STATE\s+([A-Za-z0-9_.]+)/io) {
        $sname = $1;
        if (defined $cblock->{$sname}) {
            $cstate = $cblock->{$sname};
            warn "Already have a state '$sname' in block '$cbname', extending it";
        } else {
            $cstate = $cblock->{$sname} = { name => $sname,
                                            tok => [], sname => [], sref => [] };
        }
    } elsif (/^\s*(.*)\s*\-\>\s*(.+)/o) {
        # Transition
        $toks = $1; # Maybe empty
        $snames = $2;
        # Launder toks
        $toks =~ s/\s+//og; # Kill all whitespace
        @toks = split(/\|/, $toks); # Break on pipe chars
        # Launder state names
        $snames =~ s/\s+//og;
        if ($snames =~ /^$/) {
            die "No destination states for transition on '$toks'";
        }
        @snames = split(/\,/, $snames);
        if ($#toks < 0) {
            foreach $s (@snames) {
                # Insert epsilon transitions
                push (@{$cstate->{tok}}, undef);
                push (@{$cstate->{sname}}, $s);
            }
        } else {
            foreach $t (@toks) {
                $tokens{$t} = 1;
                if (!defined $tokmap{$t}) {
                    die "Token $t not defined\n";
                }
                foreach $s (@snames) {
                    push (@{$cstate->{tok}}, $t);
                    push (@{$cstate->{sname}}, $s);
                }
            }
        }
    } elsif (/^\s*=\s*([^ \t]+)/) {
    	# Scanner exits when this state is reached, define
        # the exit value. (If multiple NFA exit states are hit at
        # once, OR the exit values together.  So they ought to be
        # bits in a vector if this situation can arise).
        
    	$cstate->{exival} = $1;

    } elsif (/^\s*ENDBLOCK/io) {
        # Bind target states
        for $s (keys %$cblock) {
            $rsname = $cblock->{$s}->{sname};
            $rsref = $cblock->{$s}->{sref} = [];
            for $i (0 .. $#$rsname) {
                $sn = $rsname->[$i];
                $rsref->[$i] = $cblock->{$sn};
                if (!defined $rsref->[$i]) {
                    die "No match for target state $sn";
                }
            }
        }
        
        $lastblock = $cblock;
        $laststate = $cstate;
        
        $cblock = $cbname = $cstate = undef;
    }
}

# $lastblock points to the outermost block go through and number all NFA states
# inside it.  (Thus some states in the input file may be 'dead' if they are in
# blocks that don't get instantiated.)

$n = 0;
@nfa_states = ();
for $s (sort keys %$lastblock) {
    $st = $lastblock->{$s};
    $st->{number} = $n;
    $nfa_states[$n] = $st;
    $n++;
}

# Print out the NFA states to log file
for $i (0 .. $#nfa_states) {
    $st = $nfa_states[$i];
    print STDERR "NFA state $i, name=".$st->{name}."\n";
    $tok = $st->{tok};
    $sname = $st->{sname};
    $sref = $st->{sref};

    $exi = $st->{exival};
    if (defined $exi) {
        print STDERR "Exit with value $exi\n";
    } else {
        for $j (0 .. $#$tok) {
            print STDERR "  ";
            if (defined $$tok[$j]) {
                print STDERR "[".$$tok[$j]."] -> ".$$sname[$j]." (".$$sref[$j]->{name}.")\n";
            } else {
                print STDERR "[<eps>] -> ".$$sname[$j]." (".$$sref[$j]->{name}.")\n";
            }
        }
    }
    print STDERR "\n";
}

# determine epsilon closures of states
# first, get direct epsilon motions

$nnfa = $#nfa_states;
$nnfa1 = 1 + $nnfa;

@queue = (); # Work queue
for $i (0 .. $nnfa) {
    $st = $nfa_states[$i];
    # Clear vector (not sure this does what I want it to, but doesn't affect
    # operation of script since the vector is initially undef anyway)
    vec($st->{eps}, 0, $nnfa1) = 0;
    vec($st->{eps}, $i, 1) = 1; # Reflexive
    my $tok = $st->{tok};
    my $sref = $st->{sref};
    for $j (0 .. $#$tok) {
        if (!defined $$tok[$j]) { # epsilon transition
            $ss = $$sref[$j];
            $sn = $ss->{number};
            vec($st->{eps}, $sn, 1) = 1;
            push (@queue, [$i, $sn]);
        }
    }
}

# second, close the mapping
while ($job = shift(@queue)) {
    my ($x, $y) = @$job;
    my $st1 = $nfa_states[$x];
    my $st2 = $nfa_states[$y];
    for $i (0 .. $nnfa) {
        if ((vec($st2->{eps}, $i, 1) == 1) &&
            (vec($st1->{eps}, $i, 1) == 0)) {
            vec($st1->{eps}, $i, 1) = 1;
            push (@queue, [$x, $i]);
        }
    }
}
 
# third, print mapping out for test
for my $i (0 .. $nnfa) {
    $st = $nfa_states[$i];
    print STDERR "State $i has eps. closure :";
    for my $j (0 .. $nnfa) {
        if (vec($st->{eps}, $j, 1) == 1) {
            print STDERR " $j";
        }
    }
    print STDERR "\n";
}

# Now, for each state, build bit vectors defining the states that can be
# transitioned to by each token.

for my $i (0 .. $nnfa) {
    $st = $nfa_states[$i];
    # Map from token to bit vector
    $dmap = $st->{dmap} = { };
    $tok = $st->{tok};
    $sref = $st->{sref};
    for $i (0 .. $#$tok) {
        $t = $tok->[$i];
        $targ_num = $sref->[$i]->{number};
        vec($dmap->{$t}, $targ_num, 1) = 1;
    }
}

# Now start to build the DFA.
$dfa_states = ();

# Build initial DFA state
$dfa_states[0] = { };
$dfa_states[0]->{nfa} = $laststate->{eps};

# Loop to construct DFA
$dd = 0;
while ($dd <= $#dfa_states) {
    print STDERR "Doing DFA state $dd\n";
    # $dd is the one whose outbound transitions we are working on
    $dx = $dfa_states[$dd];
    $dx->{map} = { };

    # work out whether this is an exit state, and the exit value if it
    # is.  We currently build a scanner that exits when the first NFA exit
    # state is hit, rather than trying for the longest match.  This is OK
    # as for the target app. there will be an END_OF_STRING token to cause
    # a transition into a defined set of exit states.

    @exit_val = ();
    for $i (0 .. $nnfa) {
    	if (vec($dx->{nfa}, $i, 1) == 1) {
            $nex = $nfa_states[$i]->{exival};
            if (defined $nex) {
                push (@exit_val, $nex);
            }
        }
    }

    if ($#exit_val >= 0) {
        $dx->{exival} = join("|",@exit_val);
    } else {
        $dx->{exival} = undef;
    }

    for $t (keys %tokens) {
        $hit = 0;
        for $xx (0 .. $nnfa) {
            vec($ovec, $xx, 1) = 0;
        }
        for $n (0 .. $nnfa) {
            if (vec($dx->{nfa}, $n, 1) == 1) {
                # This NFA state is present in the src DFA state
                $st = $nfa_states[$n];
                $tok = $st->{tok};
                $sref = $st->{sref};
                for $i (0 .. $#$tok) {
                    if ($tok->[$i] eq $t) {
                        $targ = $sref->[$i]; # Target state
                        $e = $targ->{eps};
                        $ovec |= $e; # Merge destn states onto existing
                        $hit = 1;
                    }
                }
            }
        }
        # Now have $ovec being a bit vector of which NFA states token $t can take
        # us to from this DFA state.  See if we know of this one

        if ($hit) {
            $done = undef;
            for $i (0 .. $#dfa_states) {
                if ($ovec eq $dfa_states[$i]->{nfa}) {
                    $dx->{map}->{$t} = $i;
                    $done = 1;
                    last;
                }
            }
            if (!$done) {
                # Add a new DFA state
                push (@dfa_states, { nfa=>$ovec, org=>$dd } );
                $dx->{map}->{$t} = $#dfa_states;
            }
        } else {
            $dx->{map}->{$t} = undef;
        }
    }
    $dd++;
}

# Dump out DFA for debug
for $i (0 .. $#dfa_states) {
    $dx = $dfa_states[$i];
    print STDERR "------------------------------------\n";
    print STDERR "DFA state $i, corr. NFA states :\n  ";
    for $j (0 .. $#nfa_states) {
        if (vec($dx->{nfa}, $j, 1) == 1) {
            print STDERR " $j";
        }
    }
    print STDERR "\n";
    print STDERR "Route :\n";
    @route=();
    $org = $dx->{org};
    $here = $i;
    while (defined $org) {
        @ttok=();
        for $t (sort keys %tokens) {
            if ($dfa_states[$org]->{map}->{$t} == $here) {
                push(@ttok, $t);
            }
        }
        @route = ("[".$org."] ".join('|',@ttok), @route);
        $here = $org;
        $org = $dfa_states[$org]->{org};
    }
    print STDERR "  ".join(" -> ", @route);
    print STDERR "\n";
    if (defined $dx->{exival}) {
    	print STDERR "Exit with value ".$dx->{exival}."\n";
    }
    print STDERR "Transitions :\n";
    for $t (sort keys %tokens) {
        if (defined $dx->{map}->{$t}) {
            print STDERR "  [$t] -> ".$dx->{map}->{$t}."\n";
        }
    }
}

# Finally dump out state/transition and accept tables
print "static unsigned long ".$prefix."_exitval[] = {\n";
for $i (0 .. $#dfa_states) {
    $dx = $dfa_states[$i];
    if (defined $dx->{exival}) {
        print $dx->{exival};
    } else {
        print "0";
    }
    if ($i < $#dfa_states) {
        print ", /* ".$i." */\n";
    } else {
        print "  /* ".$i." */\n";
        print "};\n\n";
    }
}

print "static short ".$prefix."_trans[] = {\n";
$n = 0;
for $i (0 .. $#dfa_states) {
    $dx = $dfa_states[$i];
    for $j (0 .. $#tokset) {
        $t = $tokset[$j];
        $ns = $dx->{map}->{$t};
        if ($n > 0) {
            print ",";
        } else {
            print "  ";
        }
        if ($n%8 == 7) {
            print "\n  ";
        } else {
            print " ";
        }
        $n++;
        if (defined $ns) {
            printf "%5d", $ns;
        } else {
            printf "%5d", -1;
        }
    }
}
print "\n};\n";
$n = 1+$#tokset;
print "#define NEXT_STATE(s,t) ".$prefix."_trans[".$n."*(s)+(t)]\n";

