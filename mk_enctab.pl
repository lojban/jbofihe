#!/usr/bin/env perl

# Create encoding table to turn bit vector output by DFA into the recognized
# word type (given that multiple bits may be set in the vector and it's down to
# a priority scheme to decide which one dominates.)

# $Header$

# COPYRIGHT

# Build first state table, 256 entries for dealing with cmavo, gismu, lujvo.
# Each class should be mutually disjoint.

$W_UNKNOWN = 0;
$W_CMAVOS = 1;
$W_CMAVOS_END_CY = 2;
$W_GISMU = 3;
$W_LUJVO = 4;
$W_FUIVLA = 5;
$W_CMENE = 6;
$W_BAD_TOSMABRU = 8;
$W_BAD_SLINKUI = 9;
$W_BIZARRE = 10;

@table = ();
for $c (0..1) {
for $cy (0..1) {
for $g0 (0..1) {
for $g1 (0..1) {
for $l0 (0..1) {
for $l1 (0..1) {
for $l1t (0..1) {
for $tos (0..1) {
    $addr = $c | ($cy<<1) | ($g0<<2) | ($g1<<3) | ($l0<<4) | ($l1<<5) | ($l1t<<6) | ($tos<<7);
    $result = $W_UNKNOWN;
    $decrement = 0;
    if (($c && $cy) ||
        (($c || $cy) && ($g0 || $g1)) ||
        (($c || $cy) && ($l0 || $l1 || $l1t)) ||
        (($g0 || $g1) && ($l0 || $l1 || $l1t)) ||
        (($l1t || $tos) && ($l0 || $l1))) {
        $result = $W_BIZARRE;
    } elsif ($g0) {
        $result = $W_GISMU;
    } elsif ($g1) {
        $result = $W_GISMU;
        $decrement = 1;
    } elsif ($l0) {
        $result = $W_LUJVO;
    } elsif ($l1) {
        $result = $W_LUJVO;
        $decrement = 1;
    } elsif ($l1t) {
        $decrement = 1;
        $result = ($tos) ? $W_LUJVO : $W_BAD_TOSMABRU;
    } elsif ($c) {
        $result = $W_CMAVOS;
    } elsif ($cy) {
        $result = $W_CMAVOS_END_CY;
    } else {
        # unknown
    }
    $table[$addr] = $result + ($decrement ? 0x80 : 0x0);

}
}
}
}
}
}
}
}

print "static unsigned char morf_enctab1[256] = {\n  ";
for $i (0 .. 255) {
    printf "0x%02x", $table[$i];
    if ($i < 255) {
        print ", ";
    }
    if ($i%8 == 7) {
        print "\n  ";
    }
}
print "};\n\n";

# 2nd table to decode fuivla/slinkui and cmene

@table = ();
for $fv0 (0..1) {
for $fv1 (0..1) {
for $sl0 (0..1) {
for $cmn (0..1) {
    $addr = ($fv0) | ($fv1<<1) | ($sl0<<2) | ($cmn<<3);
    $result = $W_UNKNOWN;
    $decrement = 0;
    if (($cmn && ($fv0 || $fv1 || $sl0))) {
        $result = $W_BIZARRE;
    } elsif ($cmn) {
        $result = $W_CMENE;
    } elsif ($fv0 && $sl0) {
        # If the shorter tail word is invalid due to slinku'i, yet the longer
        # word formed by leaving the previous syllable attached is OK, then
        # that is the correct result. (e.g baislinku'i is a valid fu'ivla as is)
        if ($fv1) {
            $decrement = 1;
            $result = $W_FUIVLA;
        } else {
            $result = $W_BAD_SLINKUI;
        }
    } elsif ($fv0) {
        $result = $W_FUIVLA;
    } elsif ($fv1) {
        $result = $W_FUIVLA;
        $decrement = 1;
    } else {
        # Keep default
    }
    $table[$addr] = $result + ($decrement ? 0x80 : 0x0);

}
}
}
}

print "static unsigned char morf_enctab2[32] = {\n  ";
for $i (0 .. 31) {
    printf "0x%02x", $table[$i];
    if ($i < 31) {
        print ", ";
    }
    if ($i%8 == 7) {
        print "\n  ";
    }
}
print "};\n\n";


