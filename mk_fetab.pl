#!/usr/bin/env perl

# $Header$

# Build the tables for the front-end stage of the morphology alg.
# Map input char. string to approx 12 symbol alphabet, and
# eventually store the posn. of each vowel to consonant transition.

# This builds the tables for doing this.

# Encoded this:
# L (2 MS bits) is 2 symbols ago
# S (5 bits) is 1 symbol ago
# G (5 LS bits) is latest symbol

# LSG is 12 bit index into LUT
# bits 3:0 are return token
# bits 5:4 are action on LSG
# bit  6 is whether to store position
# bit  7 is whether to inhibit further position stores

#######################################################################
# Notes on the lexing to feed into the DFA
# 
# Have a LUT with 3 inputs catenated together to provide the returned tok and the action to update the state.
# 
# Returned token alphabet is
# V : vowel
# C : cons following non-cons
# APOS : apostrophe
# CI : init pair (=> permissible too)
# CP : permissible non-init pair
# CN : non-permissible pair (maybe with 'y' between)
# Y : 'y'
# H : 3 letter hyphens that occur in F-III's (may be returned when DFA expects a
#     CP, watch out!)
# BT : Bad triple (ntc,nts,ndj,ndz)
# 
# (The next 3 are needed to properly check CVV[rn](>=2syl or CVV) forms
# R : 'r' after non-cons
# N : 'n' after non-cons
# NR : 'nr' after non-cons
# 
# i.e. 12 returned token types
# state action has 3 states : clear, shift or hold
# Encode L as 2 bits, S & G as 5 bits each to give a LUT mapping 4096 cases to single bytes
# (4 bits return tok, 2 bits action, 2 bits spare)
# 
# State as follows
#  L (limited) : encode 'n', 'r', cons other than 'n' or 'r', or other/blank
#  S (saved) : encode any consonant.  All vowels, apostrophes, 'y' encode to 'other'
#              (note, apostrophe = 39ASCII = 7 when reduced to 5 bits, so this needs recoding to another dead
# 	      value.  Vowels and y can be left as-they-are, or collapsed if there is a 256->32 table to
# 	      collapse the values)
#  G (got) : the last token
#  L', S' : the next values of L & S.
#           (propagation from S -> L' has to encode 32->4)
# 
#  Use these abbrevs
#  C : any cons.
#  V : any vowel
#  Cn : cons other than n
#  Cr : cons other than r
#  Cnr : cons other than n or r
#  * : any
#  - : blank setting
# 
# SGI : SG is CC
# SGP : SG is C/C
# 
# ----------------------------------------------
#   L    S    G    SGI   SGP  ->tok     L'    S'
# ----------------------------------------------
#   *    *    V                 V       -     -   (clear state)
#   *    *    '                 APOS    -     -   (clear state)
#   *    *    y                 Y       L     S   (retain state for checking cons pairs)
#   
#   -    -    r                 R       S     G
#   -    -    n                 N       S     G
#   -    -    Cnr               C       S     G
# 
# # F-III hyphen cases (none of these can be CI
# #   in the S/G pair)
# 
#   Cnr  r    Cnr               H       S     G   
#   n    r    Cnr               H       S     G   
#   Cnr  r    n                 H       S     G   
#   n    r    n                 H       S     G   
# 
#   r    n    Cnr               H       S     G   
#   Cnr  n    r                 H       S     G   
#   r    n    Cnr               H       S     G   
#   r    n    r                 H       S     G   
# 
#   r    l    n                 H       S     G   
#   n    l    r                 H       S     G   
#
#   n    t    c                 BT      S     G
#   n    t    s                 BT      S     G
#   n    d    j                 BT      S     G
#   n    d    z                 BT      S     G
#  
#   -    n    r                 NR      S     G
# 
# # Cons pairs - returned token depends on what type
# # of pair has appeared (note : cases in DFA where
# # CP is a significant token may have to be checked for
# # spurious applicability of H token too)
# 
#   *    C    C    YES   YES    CI      S     G
#   *    C    C    NO    YES    CP      S     G
#   *    C    C    NO    NO     CN      S     G
# 
# # Fall-thru case for anything else (e.g. bogus chars)
# 
#   *    *    *                 JUNK    don't care
  
#######################################################################

$TOK_UNK = 0;
$TOK_V = 1;
$TOK_APOS = 2;
$TOK_Y = 3;
$TOK_R = 4;
$TOK_N = 5;
$TOK_C = 6;
$TOK_NR = 7;
$TOK_CI = 8;
$TOK_CP = 9;
$TOK_CN = 10;
$TOK_H  = 11;
$TOK_BT = 12; # bad triples

$AC_CLR = 0;
$AC_SFT = 1;
$AC_FRZ = 2;

@table = ();

$n = 0;
print "static unsigned char inact[] = {\n";
for ($i=0; $i<4; $i++) {
    # encoding 0=clear/non-cons, 1=n, 2=r, 3=C\[nr]
    for ($s=0; $s<32; $s++) {
        for ($g=0; $g<32; $g++) {
            $a = sprintf("%c%c", $s+96, $g+96);
            $a =~ s/~/'/go;
            $a = "V".$a if ($i == 0);
            $a = "n".$a if ($i == 1);
            $a = "r".$a if ($i == 2);
            $a = "C".$a if ($i == 3);

            if ($a =~ /^(rln|nlr)$/) {
                $act = $AC_SFT;
                $tok = $TOK_H;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^([bcdfgjklmnpstvxz]r[bcdfgjklmnpstvxz])$/) {
                $act = $AC_SFT;
                $tok = $TOK_H;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^(rn[bcdfgjklmprstvxz])$/) {
                $act = $AC_SFT;
                $tok = $TOK_H;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^([bcdfgjklmprstvxz]nr)$/) {
                $act = $AC_SFT;
                $tok = $TOK_H;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /[aeiou]$/) {
                $act = $AC_CLR;
                $tok = $TOK_V;
                $set = 0;
                $inh = 0;
            } elsif ($a =~ /'$/) {
                $act = $AC_CLR;
                $tok = $TOK_APOS;
                $set = 0;
                $inh = 0;
            } elsif ($a =~ /y$/) {
                $act = $AC_FRZ;
                $tok = $TOK_Y;
                $set = 0;
                $inh = 0;
            } elsif ($a =~ /^V[aeiou`]r$/) {
                $act = $AC_SFT;
                $tok = $TOK_R;
                $set = 1;
                $inh = 0;
            } elsif ($a =~ /^V[aeiou`]n$/) {
                $act = $AC_SFT;
                $tok = $TOK_N;
                $set = 1;
                $inh = 0;
            } elsif ($a =~ /^V[aeiou`][bcdfgjklmpstvxz]$/) { # First cons after run of vowel etc
                $act = $AC_SFT;
                $tok = $TOK_C;
                $set = 1;
                $inh = 0;
            } elsif ($a =~ /^Vnr$/) { # nr specifically (for CVV/lujvo hyph checking)
                $act = $AC_SFT;
                $tok = $TOK_NR;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.(bb|cc|dd|ff|gg|jj|kk|ll|mm|nn|pp|rr|ss|tt|vv|xx|zz)$/) { # both the same
                $act = $AC_SFT;
                $tok = $TOK_CN;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.([bdgvjz][ptkfcsx])$/) { # voiced/unvoiced
                $act = $AC_SFT;
                $tok = $TOK_CN;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.([ptkfcsx][bdgvjz])$/) { # unvoiced/voiced
                $act = $AC_SFT;
                $tok = $TOK_CN;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.([cjsz]{2})$/) { # not both from this set
                $act = $AC_SFT;
                $tok = $TOK_CN;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^n(dj|dz|tc|ts)$/) { # specific forbidden triples
                $act = $AC_SFT;
                $tok = $TOK_BT;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.(cx|kx|xc|xk|mz)$/) { # specific forbidden pairs
                $act = $AC_SFT;
                $tok = $TOK_CN;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /^.(pl|pr|fl|fr|bl|br|vl|vr|cp|cf|ct|ck|cm|cn|cl|cr|jb|jv|jd|jg|jm|sp|sf|st|sk|sm|sn|sl|sr|zb|zv|zd|zg|zm|tc|tr|ts|kl|kr|dj|dr|dz|gl|gr|ml|mr|xl|xr)$/) { # specific initial pairs
                $act = $AC_SFT;
                $tok = $TOK_CI;
                $set = 0;
                $inh = 1;
            } elsif ($a =~ /[bcdfgjklmnprstvxz]{2}$/) {
                $act = $AC_SFT;
                $tok = $TOK_CP;
                $set = 0;
                $inh = 1;
            } else {
                $act = $AC_CLR;
                $tok = $TOK_UNK;
                $set = 0;
                $inh = 0;
            }
            
            $te = $tok + ($act << 4) + ($set << 6) + ($inh << 7);
            $ent = ($i << 10) + ($s << 5) + $g;
            $table[$ent] = $te;
            print "," if ($n > 0);
            print "\n  " if ($n%8 == 0);
            print " " if ($n%8 != 0);
            $n++;
            printf "0x%02x", $te;

            printf STDERR "i=%d s=%02x (%c) g=%02x (%c) act=%d tok=%d set=%d inh=%d\n",
                $i, $s, $s+96, $g, $g+96, $act, $tok, $set, $inh;
            
        }
    }
}

print "};\n";



