# $Header$
#
# Perl script to read through the bison grammar for lojban and insert
# simple actions, typically to build a nonterminal node.
#
# COPYRIGHT
#

while (<>) {
    print;
    if (/^\%\%/) { last; }
}

%nonterms=();

$max_nchildren = 0;

$first = 1;
@flags=();

while (<>) {

    if (/^[ \t]*([a-z0-9A-Z_]+)[ \t]*:(.*)$/) {
	if ($first) {
	    # Special processing assumes the outermost rule has 1 child and appears
	    # first in the file.  This rule is to match the complete input.
	    print;
	    print sprintf("{ top = \$1; }\n");
	    $first = 0;

	} else {
	    $nonterm=$1;
	    $nonterm =~ y/a-z/A-Z/;
	    $nonterms{$nonterm} = 1;
	    $childs = $2;
	    @children = split(/[ \t]+/,$childs);
	    $nchildren = $#children;
	    if ($nchildren < 0) { $nchildren = 0; }
	    $n_live_children = 0;
	    for ($i=1; $i<=$nchildren; $i++) {
		if ($children[$i] =~ /START_EK|START_GIHEK|START_GUHEK|START_JEK|START_JOIK|START_GEK|START_BAI|EK_KE|EK_BO|JEK_KE|JEK_BO|JOIK_KE|JOIK_BO|I_JEKJOIK|I_BO|GIHEK_KE|GIHEK_BO|NAhE_BO|NAhE_time|NAhE_space|NUMBER_MAI|NUMBER_MOI|NUMBER_ROI|EOF_MARK/) {
		    $flags[$i] = 0;
		} else {
		    $flags[$i] = 1;
		    $n_live_children++;
		}
	    }

	    if ($n_live_children > $max_nchildren) { $max_nchildren = $n_live_children; }

	    print;
	    print sprintf("{\$\$ = new_node_%d(%s", $n_live_children, $nonterm);
	    for ($i = 1; $i<=$nchildren; $i++) {
		if ($flags[$i]) {
		    print sprintf(",\$%d", $i);
		}
	    }
	    print ");}\n";
	}

	# IF there are rules with 'error' in them, 2 things are assumed :
 	# 1. they are not the first rule for a particular non-terminal
	# 2. they have their own action pre-supplied in the source file.
	# so we ignore them.

    } elsif  (/^[ \t]*\|(.*)$/ && !/ error / && !/ error$/) {
	$childs = $1;
	@children = split(/[ \t]+/,$childs);
	$nchildren = $#children;
	if ($nchildren < 0) { $nchildren = 0; }
	$n_live_children = 0;
	for ($i=1; $i<=$nchildren; $i++) {
	    if ($children[$i] =~ /START_EK|START_GIHEK|START_GUHEK|START_JEK|START_JOIK|START_GEK|START_BAI|EK_KE|EK_BO|JEK_KE|JEK_BO|JOIK_KE|JOIK_BO|I_JEKJOIK|I_BO|GIHEK_KE|GIHEK_BO|NAhE_BO|NAhE_time|NAhE_space|NUMBER_MAI|NUMBER_MOI|NUMBER_ROI|EOF_MARK/) {
		$flags[$i] = 0;
	    } else {
		$flags[$i] = 1;
		$n_live_children++;
	    }
	}
	
	if ($n_live_children > $max_nchildren) { $max_nchildren = $n_live_children; }
	print;
	print sprintf("{\$\$ = new_node_%d(%s", $n_live_children, $nonterm);
	for ($i = 1; $i<=$nchildren; $i++) {
	    if ($flags[$i]) {
		print sprintf(",\$%d", $i);
	    }
	}
	print ");}\n";
    } else {
	print;
    }
}

$nonterms{"AUGMENTED"} = 1;

@nt = sort keys(%nonterms);

open(NT, ">nonterm.h");
print NT "#ifndef NONTERM_H\n#define NONTERM_H\n\n/* max childen = $max_nchildren */\n\nextern char *nonterm_names[];\n\ntypedef enum {\n";
$first=1;
for $nt (@nt) {
    if (!$first) {
	print NT ",\n  ";
    } else {
	print NT "  ";
    }
    $first=0;
    print NT $nt;
}
print NT "\n} NonTerm;\n\n#endif\n";
close (NT);

open(NTC, ">nonterm.c");
print NTC "char *nonterm_names[] = {\n";
$first = 1;
for $nt (@nt) {
    if (!$first) {
	print NTC ",\n  ";
    } else {
	print NTC "  ";
    }
    $first=0;
    print NTC "\"".$nt."\"";
}
print NTC "};\n";
close (NTC);


