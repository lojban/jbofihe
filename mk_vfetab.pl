#!/usr/bin/env perl

# $Header$

# Create tables for the second front-end stage of the morphology algorithm.
# This stage is concerned with tracking the validity of vowel clusters,
# including whether a comma appears between the vowels. 

$VTOK_V = 1; # must agree with coding for V in consonant FSM
$VTOK_Y = 3; # must agree with coding for y in consonant FSM
$VTOK_VV = 16; # ai/au/ei/oi valid anywhere
$VTOK_VX = 17; # [iu][aeiou] - vowel extended
$VTOK_VO = 18; # [aeiou][aeiou] less the 14 patterns above 
$VTOK_VY = 19; # combinations involving y where valid [only in cmene]
$VTOK_YY = 20; # two copies of y adjacent with no separation (only in hesitation string)
$VTOK_UNK = 0;

# Note, C encodes all consonants + apostrophe

@syms = qw(, C y a e i o u);

$n = 0;
print "static unsigned char vcheck[] = {";
for $i (0 .. 7) {
for $j (0 .. 7) {
for $k (0 .. 7) {
$x = $syms[$i].$syms[$j].$syms[$k];
if ($x =~ /..,/) {
    $r = $VTOK_UNK;
} elsif ($x =~ /.(ai|au|ei|oi)/) {
    $r = $VTOK_VV;
} elsif ($x =~ /.[iu][aeiou]/) {
    $r = $VTOK_VX;
} elsif ($x =~ /.[aeiou][aeiou]/) {
    $r = $VTOK_VO;
} elsif ($x =~ /.[iu]y/) {
    $r = $VTOK_VY;
} elsif ($x =~ /.Cy/) {
    $r = $VTOK_Y;
} elsif ($x =~ /[Cy]yy/) {
    $r = $VTOK_YY;
} elsif ($x =~ /.C[aeiou]/) {
    $r = $VTOK_V;
} else {
    $r = $VTOK_UNK;
}

if ($n > 0) { print ","; }
if ($n%8==0) { print "\n  "; }
$n++;
printf "%2d", $r;
}
}
}
print "};\n\n";


