#!/usr/bin/env perl
#
#

$output = "";

# Set probabilities for various optional things.
#($plo, $pmed, $phi, $piter) = (0.0005, 0.001, 0.004, 0.003);
($plo, $pmed, $phi, $piter) = (0.01, 0.05, 0.25, 0.1);

# Number of times the expansion can recurse back to a higher level construct
# (prevent infinite recursion)
$recur = 10;

# {{{ %p : Associative array defining probabilities for taking various optional constructs
%p = (
  relative_clauses => $pmed
);
#}}}

$seeding = shift @ARGV;
if (!defined $seeding) { $seeding = time(); }
$usseeding = $seeding;

print "Seed [$seeding]\n";

open (RND, "../mymt $seeding|");
$to_unit = 65536.0 * 65536.0;

sub rnd {#{{{
  read RND, $x, 4;
  $y = unpack("N", $x);
  return $y / $to_unit;
}
#}}}
sub getnum1 {#{{{
  my $n = 1;
  while (&rnd < $piter) { ++$n; }
  return $n;
}
#}}}
sub pick {#{{{
  my ($ref) = @_;
  my $n = $#$ref;
  my $i = int($n * &rnd);
  return $ref->[$i];
}
#}}}
sub pickx {#{{{
  my $n = $#_;
  my $i = int($n * &rnd);
  return $_[$i];
}
#}}}
$llen = 0;
#
######
# Implement stack of where we are in the grammar

sub clear_stack {#{{{
  @names = ();
  @number = ();
  $next_number = 0;
  @trace = ();
}
#}}}
sub in {#{{{
  my ($name) = @_;
  push(@names, $name);
  push(@number, $next_number);
  my $result = $next_number;
  ++$next_number;
  my $trace = sprintf("%s { (%3d:%05d)", $name, $#number, $result);
  push(@trace, $trace);
  return $result;
}
#}}}
sub out {#{{{
  my ($n) = @_; # Allow multiple popping
    if (defined $n) {
      while ($#number >= 0) {
        my $x = pop(@number);
        my $name = pop(@names);
        if ($x != $n) {
          my $trace = sprintf("} (%3d:%05d) %s", $#number+1, $x, $name);
          push(@trace, $trace);
          print STDERR "Auto clear of $name from stack ($x, closing $n)\n";
        } else {
          my $trace = sprintf("} %3d:%05d %s (->%s)", $#number+1, $x, $name, $names[$#$names]);
          push(@trace, $trace);
          return;
        }
      }
    }
}
#}}}

sub emit {#{{{
  my ($x, $selmao) = @_;
  $output .= $x;
  $llen += length($x);
  if ($llen > 72) {
    $output .= "\n";
    $llen = 0;
  } else {
    $output .= " ";
    $llen++;
  }
  push(@trace, "   ".$x." [".$selmao."]");
}
#}}}
sub clear_output {#{{{
  $output = "";
  $llen = 0;
}
#}}}

sub A {#{{{
  &emit(&pickx("a", "e", "o", "u", "ji"), "A");
}
#}}}
@BAI = (#{{{
  "ba'i",
  "bai",
  "bau",
  "be'i",
  "ca'i",
  "cau",
  "ci'e",
  "ci'o",
  "ci'u",
  "cu'u",
  "de'i",
  "di'o",
  "do'e",
  "du'i",
  "du'o",
  "fa'e",
  "fau",
  "fi'e",
  "ga'a",
  "gau",
  "ja'e",
  "ja'i",
  "ji'e",
  "ji'o",
  "ji'u",
  "ka'a",
  "ka'i",
  "kai",
  "ki'i",
  "ki'u",
  "koi",
  "ku'u",
  "la'u",
  "le'a",
  "li'e",
  "ma'e",
  "ma'i",
  "mau",
  "me'a",
  "me'e",
  "mu'i",
  "mu'u",
  "ni'i",
  "pa'a",
  "pa'u",
  "pi'o",
  "po'i",
  "pu'a",
  "pu'e",
  "ra'a",
  "ra'i",
  "rai",
  "ri'a",
  "ri'i",
  "sau",
  "si'u",
  "ta'i",
  "tai",
  "ti'i",
  "ti'u",
  "tu'i",
  "va'o",
  "va'u",
  "zau",
  "zu'e"
);
#}}}
sub BAI {#{{{
  $n = 1+$#BAI;
  $r = int($n * &rnd);
  &emit($BAI[$r], "BAI");
}
#}}}
sub BE {#{{{
  &emit("be", "BE");
}
#}}}
sub BEhO {#{{{
  &emit("be'o", "BEhO");
}
#}}}
sub BEI {#{{{
  &emit("bei", "BEI");
}
#}}}
sub BIhI {#{{{
  &emit(&pickx("bi'i", "bi'o", "mi'i"), "BIhI");
}
#}}}
sub BO {#{{{
  emit("bo", "BO");
}
#}}}
sub BOI {#{{{
  &emit("boi", "BOI");
}
#}}}
@BRIVLA = (#{{{
  # Obviously this is a bit limited.  Eventually I want to do a random word
  # generator.
  "zdani", "zarci", "klama", "nanmu", "bevri"
);
#}}}
sub BRIVLA {#{{{
# FIXME
  &emit(&pick(\@BRIVLA), "BRIVLA");
}
#}}}
@CAhA = (#{{{
"ca'a",
"ka'e",
"nu'o",
"pu'i"
);
#}}}
sub CAhA {#{{{
  $n = 1+$#CAhA;
  $r = int($n * &rnd);
  &emit($CAhA[$r], "CAhA");
}
#}}}
sub CEhE {#{{{
  &emit("ce'e", "CEhE");
}
#}}}
sub CEI {#{{{
  &emit("cei", "CEI");
}
#}}}
sub CMENE {#{{{
  &emit("london.", "CMENE");
}
#}}}
sub CO {#{{{
  &emit("co", "CO");
}
#}}}
sub CU {#{{{
  &emit("cu", "CU");
}
#}}}
sub CUhE {#{{{
  &emit("cu'e", "CUhE");
}
#}}}
@FA = (#{{{
  "fa", "fe", "fi", "fo", "fu", "fai", "fi'a"
);
#}}}
sub FA {#{{{
  &emit(&pick(\@FA), "FA");
}
#}}}
@FAhA = (#{{{
"be'a",
"bu'u",
"ca'u",
"du'a",
"fa'a",
"ga'u",
"ne'a",
"ne'i",
"ne'u",
"ni'a",
"pa'o",
"re'o",
"ri'u",
"ru'u",
"te'e",
"ti'a",
"to'o",
"vu'a",
"ze'o",
"zo'a",
"zo'i",
"zu'a"
);
#}}}
sub FAhA {#{{{
  &emit (&pick(\@FAhA), "FAhA");
};
#}}}
sub FEhE {#{{{
  &emit("fe'e", "FEhE");
}
#}}}
sub FEhU {#{{{
  &emit("fe'u", "FEhU");
}
#}}}
sub FIhO {#{{{
  &emit("fi'o", "FIhO");
}
#}}}
sub GA {#{{{
  &emit(&pickx("ga", "ge", "ge'i", "go", "gu"), "GA");
}
#}}}
sub GAhO {#{{{
  &emit(&pickx("ga'o", "ke'i"), "GAhO");
}
#}}}
sub GEhU {#{{{
  &emit("ge'u", "GEhU");
}
#}}}
sub GI {#{{{
  &emit("gi", "GI");
}
#}}}
sub GIhA {#{{{
  &emit(&pickx("gi'a", "gi'e", "gi'i", "gi'o", "gi'u"), "GIhA");
}
#}}}
@GOhA = (#{{{
  "bu'a", "bu'e", "bu'i", "co'e", "du",
  "go'a", "go'e", "go'i", "go'o", "go'u",
  "mo", "nei", "no'a"
);
#}}}
sub GOhA {#{{{
  &emit(&pick(\@GOhA), "GOhA");
}
#}}}
@GOI = (#{{{
"goi", "ne", "no'u", "pe", "po",
"po'e", "po'u"
);
#}}}
sub GOI {#{{{
  &emit(&pick(\@GOI), "GOI");
}
#}}}
sub GUhA {#{{{
  &emit(&pickx("gu'a", "gu'e", "gu'i", "gu'o", "gu'u"), "GUhA");
}
#}}}
sub I {#{{{
  &emit("i", "I");
}
#}}}
sub JA {#{{{
  &emit(&pickx("ja", "je", "je'i", "jo", "ju"), "JA");
}
#}}}
sub JAI {#{{{
  &emit("jai", "JAI");
}
#}}}
@JOI = (#{{{
"ce", "ce'o", "fa'u", "jo'e", "jo'u",
"joi", "ku'a", "pi'u"
);
#}}}
sub JOI {#{{{
  &emit(&pick(\@JOI), "JOI");
}
#}}}
sub KE {#{{{
  &emit("ke", "KE");
}
#}}}
sub KEhE {#{{{
  &emit("ke'e", "KEhE");
}
#}}}
sub KEI {#{{{
  &emit("kei", "KEI");
}
#}}}
sub KI {#{{{
  emit("ki", "KI");
}
#}}}
@KOhA = (#{{{
"da", "da'e", "da'u", "de", "de'e",
"de'u", "dei", "di", "di'e", "di'u",
"do", "do'i", "do'o", "fo'a", "fo'e",
"fo'i", "fo'o", "fo'u", "ke'a", "ko", "ko'a",
"ko'e", "ko'i", "ko'o", "ko'u", "ma",
"ma'a", "mi", "mi'a", "mi'o", "ra",
"ri", "ru", "ta", "ti", "tu",
"vo'a", "vo'e", "vo'i", "vo'o", "vo'u",
"zi'o", "zo'e", "zu'i"
);
#}}}
sub KOhA {#{{{
  &emit(&pick(\@KOhA), "KOhA");
}
#}}}
sub KU {#{{{
  &emit("ku", "KU");
}
#}}}
sub KUhO {#{{{
  &emit("ku'o", "KUhO");
}
#}}}
@LAhE = (#{{{
"la'e", "lu'a", "lu'e", "lu'i",
"lu'o", "tu'a", "vu'i"
);
#}}}
sub LAhE {#{{{
  &emit(&pick(\@LAhE), "LAhE");
}
#}}}
sub LA {#{{{
  &emit(&pickx("la", "lai", "la'i"), "LA");
}
#}}}
@LE = (#{{{
"le", "le'e", "le'i", "lei",
"lo", "lo'e", "lo'i", "loi"
);
#}}}
sub LE {#{{{
  &emit(&pick(\@LE), "LE");
}
#}}}
sub LIhU {#{{{
  &emit("li'u", "LIhU");
}
#}}}
sub LU {#{{{
  emit("lu", "LU");
}
#}}}
sub LUhU {#{{{
  &emit("lu'u", "LUhU");
}
#}}}
sub ME {#{{{
  &emit("me", "ME");
}
#}}}
sub MEhU {#{{{
  &emit("me'u", "MEhU");
}
#}}}
sub MOhI {#{{{
  &emit ("mo'i", "MOhI");
}
#}}}
@MOI = (#{{{
"cu'o", "mei", "moi", "si'e"
);
#}}}
sub MOI {#{{{
  &emit(&pick(\@MOI), "MOI");
}
#}}}
sub NA {#{{{
  &emit("na", "NA");
}
#}}}
sub NAhE {#{{{
  &emit("na'e", "NAhE");
}
#}}}
sub NAI {#{{{
  &emit("nai", "NAI");
}
#}}}
sub NIhO {#{{{
  &emit(&pickx("ni'o", "no'i"), "NIhO");
}
  #}}}
sub NOI {#{{{
  &emit(&pickx("noi", "poi", "voi"), "NOI");
}
#}}}
@NU = (#{{{
"du'u", "jei", "ka", "li'i", "ni",
"nu", "si'o", "su'u"
);
#}}}
sub NU {#{{{
  &emit(&pick(\@NU), "NU");
}
#}}}
@PA = (#{{{
  "bi", "ce'i", "ci", "ci'i", "da'a",
  "dau", "du'e", "fei", "fi'u", "gai",
  "jau", "ji'i", "ka'o", "ki'o", "ma'u",
  "me'i", "mo'a", "mu", "ni'u", "no",
  "no'o", "pa", "pai", "pi", "pi'e",
  "ra'e", "rau", "re", "rei", "ro",
  "so", "so'a", "so'e", "so'i", "so'o",
  "so'u", "su'e", "su'o", "te'o", "tu'o",
  "vai", "vo", "xa", "xo", "za'u",
  "ze"
);
#}}}
sub PA {#{{{
  &emit(&pick(\@PA), "PA");
}
#}}}
sub PEhE {#{{{
  &emit("pe'e", "PEhE");
}
#}}}
sub PU {#{{{
  $r = &rnd;
  &emit(&pickx("pu", "ca", "ba"), "PU");
}
#}}}
sub RAhO {#{{{
  &emit("ra'o", "RAhO");
}
#}}}
sub ROI {#{{{
  &emit("roi", "ROI");
}
#}}}
@SE = (#{{{
  "se", "te", "ve", "xe"
);
#}}}
sub SE {#{{{
  &emit(&pick(\@SE), "SE");  
}
#}}}
@TAhE = (#{{{
  "di'i", "na'o", "ru'i", "ta'e"
);
  #}}}
sub TAhE {#{{{
  &emit (&pick(\@TAhE), "TAhE");
}
#}}}
sub TUhE {#{{{
  &emit("tu'e", "TUhE");
}
  #}}}
sub TUhU {#{{{
  &emit("tu'u", "TUhU");
}
  #}}}
sub VA {#{{{
  &emit(&pickx("va", "vi", "vu"), "VA");
}
#}}}
sub VAU {#{{{
  &emit("vau", "VAU");
}
#}}}
@VEhA = (#{{{
  "ve'a", "ve'e", "ve'i", "ve'u"
);
#}}}
sub VEhA {#{{{
  &emit (&pick(\@VEhA), "VEhA");
}
#}}}
@VIhA = (#{{{
  "vi'a", "vi'e", "vi'i", "vi'u"
);
#}}}
sub VIhA {#{{{
  &emit (&pick(\@VIhA), "VIhA");
}
#}}}
sub VUhO {#{{{
  &emit ("vu'o", "VUhO");
}
#}}}
@ZAhO = (#{{{
  "ba'o",
  "ca'o",
  "co'a",
  "co'i",
  "co'u",
  "de'a",
  "di'a",
  "mo'u",
  "pu'o",
  "za'o"
);
#}}}
sub ZAhO {#{{{
  &emit (&pick(\@ZAhO), "ZAhO");
}
#}}}
@ZEhA = (#{{{
  "ze'a", "ze'e", "ze'i", "ze'u"
);
#}}}
sub ZEhA {#{{{
  &emit(&pick(\@ZEhA), "ZEhA");
}
#}}}
sub ZI {#{{{
  &emit(&pickx("zi", "za", "zu"), "ZI");
}
#}}}
sub ZIhE {#{{{
  &emit("zi'e", "ZIhE");
}
#}}}
sub ZOhU {#{{{
  &emit("zo'u", "ZOhU");
}
#}}}
#
sub number {#{{{
## FIXME
  $n = &getnum1;
  for (1..$n) {
    &PA;
  }
};
#}}}
sub interval_property {#{{{
  my $r = &rnd;
  if ($r < 0.5) {
    &number;
    &ROI;
    if (&rnd < $phi) { &NAI; }
  } elsif ($r < 0.75) {
    &ZAhO;
    if (&rnd < $phi) { &NAI; }
  } else {
    &TAhE;
    if (&rnd < $phi) { &NAI; }
  }
}
#}}}
sub interval_property_seq {#{{{
  my $n = &getnum1;
  for (1 .. $n) {
    &interval_property;
  }
}
#}}}
sub time_offset_seq {#{{{
  &PU;
  &NAI if (&rnd < $phi);
  &ZI if (&rnd < $phi);
}
#}}}
sub zeha_pu_nai {#{{{
  &ZEhA;
  if (&rnd < 0.5) {
    &PU;
    &NAI if (&rnd < $phi);
  }
}
#}}}
sub time {#{{{
  $r = 15.0 * &rnd;
     if ($r <  1) {                                      &interval_property_seq  }
  elsif ($r <  2) {                        &zeha_pu_nai;                         }
  elsif ($r <  3) {                        &zeha_pu_nai; &interval_property_seq; }
  elsif ($r <  4) {      &time_offset_seq;                                       }
  elsif ($r <  5) {      &time_offset_seq;               &interval_property_seq; }
  elsif ($r <  6) {      &time_offset_seq; &zeha_pu_nai;                         }
  elsif ($r <  7) {      &time_offset_seq; &zeha_pu_nai; &interval_property_seq; }
  elsif ($r <  8) { &ZI;                                                         }
  elsif ($r <  9) { &ZI;                                 &interval_property_seq; }
  elsif ($r < 10) { &ZI;                   &zeha_pu_nai;                         }
  elsif ($r < 11) { &ZI;                   &zeha_pu_nai; &interval_property_seq; }
  elsif ($r < 12) { &ZI; &time_offset_seq;                                       }
  elsif ($r < 13) { &ZI; &time_offset_seq;               &interval_property_seq; }
  elsif ($r < 14) { &ZI; &time_offset_seq; &zeha_pu_nai;                         }
  elsif ($r < 15) { &ZI; &time_offset_seq; &zeha_pu_nai; &interval_property_seq; }
}
#}}}
sub mohi_space_offset {#{{{
  &MOhI;
  &space_offset;
}
#}}}
sub space_offset {#{{{
  &FAhA;
  if (&rnd < $phi) { &NAI; }
  if (&rnd < $phi) { &VA; }
}
#}}}
sub space_int_props {#{{{
  my $n = &getnum1;
  for (1 .. $n) {
    &FEhE;
    &interval_property;
  }
}
#}}}
sub space_interval {#{{{
  my $r = &rnd;
  my $x1, $x2;
  if ($r < 0.333) { $x1=1; $x2=1; }
  elsif ($r < 0.667) { $x1=0; $x2=1; }
  else               { $x1=1; $x2=0; }
  if ($x1) {
    $r = &rnd;
       if ($r < 0.333) { &VEhA ; &VIhA; }
    elsif ($r < 0.667) {         &VIhA; }
    else               { &VEhA;         }
    if (&rnd < 0.5) {
      &FAhA;
      if (&rnd < $phi) { &NAI; }
    }
  }
  if ($x2) {
    &space_int_props;
  }
}
#}}}
sub space_offset_seq {#{{{
  $n = &getnum1;
  for (1 .. $n) {
    &space_offset;
  }
}
#}}}
sub space {#{{{
  $r = 15.0 * &rnd;
     if ($r <  1) {                                          &mohi_space_offset; }
  elsif ($r <  2) {                         &space_interval;                     }
  elsif ($r <  3) {                         &space_interval; &mohi_space_offset; }
  elsif ($r <  4) {      &space_offset_seq;                                      }
  elsif ($r <  5) {      &space_offset_seq;                  &mohi_space_offset; }
  elsif ($r <  6) {      &space_offset_seq; &space_interval;                     }
  elsif ($r <  7) {      &space_offset_seq; &space_interval; &mohi_space_offset; }
  elsif ($r <  8) { &VA;                                                         }
  elsif ($r <  9) { &VA;                                     &mohi_space_offset; }
  elsif ($r < 10) { &VA;                    &space_interval;                     }
  elsif ($r < 11) { &VA;                    &space_interval; &mohi_space_offset; }
  elsif ($r < 12) { &VA; &space_offset_seq;                                      }
  elsif ($r < 13) { &VA; &space_offset_seq;                  &mohi_space_offset; }
  elsif ($r < 14) { &VA; &space_offset_seq; &space_interval;                     }
  elsif ($r < 15) { &VA; &space_offset_seq; &space_interval; &mohi_space_offset; }
}
#}}}
sub tsst {#{{{
  if (&rnd < 0.5) {
    &time;
    &space if (&rnd < $phi);
  } else {
    &space;
    &time if (&rnd < $phi);
  }
}
#}}}
sub simple_tense_modal {#{{{
  $r = &rnd;
  if ($r < 0.25) {
    &NAhE if (&rnd < $phi);
    &SE   if (&rnd < $phi);
    &BAI;
    &NAI  if (&rnd < $phi);
    &KI   if (&rnd < $phi);
  } elsif ($r < 0.3) {
    &KI;
  } elsif ($r < 0.35) {
    &CUhE;
  } else {
    &NAhE if (&rnd < $phi);
    $r = &rnd;
    if ($r < 0.5) {
      &tsst;
    } elsif ($r < 0.75) {
      &tsst;
      &CAhA;
    } else {
      &CAhA;
    }
    &KI if (&rnd < $phi);
  }
}


########################
#

#}}}

sub text {#{{{
  while (&rnd < $piter) { &NAI; }
  my $r = &rnd;
  if ($r < $pmed) {
    &CMENE;
    while (&rnd < $piter) { &CMENE; }
    &opt_free_seq;
  } elsif ($r < $pmed+$pmed) {
    ## FIXME
  }
  if (&rnd < $pmed) { &joik_jek; }
  &text_1;
}
#}}}
sub text_1 {#{{{
  if (&rnd < $phi) {
    if (&rnd < 0.8) {
      my $n = &getnum1;
      for (1 .. $n) {
        &I;
        my $r = &rnd;
        if ($r < 0.333) { &jek; }
        elsif ($r < 0.667) { &joik; }
        if (&rnd < $phi) {
          if (&rnd < $phi) { &stag; }
          &BO;
        }
        &opt_free_seq;
      }
    } else {
      &NIhO;
      while (&rnd < $piter) { &NIhO; }
    }
  }
  if (&rnd < 0.95) { &paragraphs; }
}
#}}}
sub paragraphs {#{{{
  my $n = &getnum1 - 1;
  &paragraph;
  for (1..$n) {
    &NIhO;
    &opt_free_seq;
    &paragraph;
  }
}
#}}}
sub paragraph {#{{{
  if (&rnd < 0.95) {
    &statement;
  } else {
    &fragment;
  }
  my $n = &getnum1 - 1;
  for (1..$n) {
    &I;
    &opt_free_seq;
    my $r = &rnd;
    if ($r < 0.9) {
      &statement;
    } elsif ($r < 0.95) {
      &fragment;
    } else {
      # blank
    }
  }
}
#}}}
sub statement {#{{{
  my $z;
  if (&rnd > $piter) {
    $z = &in("statement_s1");
    &statement_1;
  } else {
    $z = &in("statement_pnx_stmt");
    &prenex;
    &statement;
  }
  &out($z);
}
#}}}
sub statement_1 {#{{{
  &statement_2;
  my $n = &getnum1 - 1;
  for (1..$n) {
    &I;
    &joik_jek;
    if (&rnd < 0.9) {
      &statement_2;
    }
  }
}
#}}}
sub statement_2 {#{{{
  &statement_3;
  if (&rnd < 0.3) {
    &I;
    my $r = &rnd;
    if    ($r < 0.333) { &jek; }
    elsif ($r < 0.667) { &joik; }
    
    if (&rnd < $phi) { &stag; }
    &BO;
    &opt_free_seq;
    if ($r < 0.5) { &statement_2; }
  }
}
#}}}
sub statement_3 {#{{{
  if ($recur>0 && &rnd < 0.2) {
    --$recur;
    if (&rnd < $phi) { &tag; }
    &TUhE;
    &opt_free_seq;
    &text_1;
    &TUhU;
    &opt_free_seq;
  } else {
    &sentence;
  }
}
#}}}
sub fragment {#{{{
  my $r = &rnd;
  my $z;
  if    ($r < 0.1) { $z = &in("fragment_ek"); &ek; &opt_free_seq; }
  elsif ($r < 0.2) { $z = &in("fragment_gihek"); &gihek; &opt_free_seq; }
  elsif ($r < 0.3) { $z = &in("fragment_quantifier"); &quantifier; }
  elsif ($r < 0.4) { $z = &in("fragment_na"); &NA; &opt_free_seq; }
  elsif ($r < 0.55) { $z = &in("fragment_terms"); &terms; &VAU; &opt_free_seq; }
  elsif ($r < 0.65) { $z = &in("fragment_prenex"); &prenex; }
  elsif ($r < 0.8) { $z = &in("fragment_relative_clauses"); &relative_clauses; }
  elsif ($r < 0.9) { $z = &in("fragment_links"); &links; }
  else             { $z = &in("fragment_linkargs"); &linkargs; }
  &out($z);
}
#}}}
sub prenex {#{{{
  &terms;
  &ZOhU;
  &opt_free_seq;
}
#}}}
sub sentence {#{{{
  my $z = &in("sentence");
  if (&rnd < 0.5) {
    &terms;
    &CU;
    &opt_free_seq;
  }
  &bridi_tail;
  &out($z);
}
#}}}
sub subsentence {#{{{
  if (&rnd < $piter) {
    &prenex;
    &subsentence;
  } else {
    &sentence;
  }
}
#}}}
sub bridi_tail {#{{{
  my $zz = &in("bridi_tail");
  &bridi_tail_1;
  if (&rnd < $piter) {
    my $z = &in("bridi_tail_iter");
    &gihek;
    if (&rnd < $phi) { &stag; }
    &KE;
    &opt_free_seq;
    &bridi_tail;
    &KEhE;
    &opt_free_seq;
    &tail_terms;
    &out($z);
  }
  &out($zz);
}
#}}}
sub bridi_tail_1 {#{{{
  &bridi_tail_2;
  while (&rnd < $piter) {
    &gihek;
    &opt_free_seq;
    &bridi_tail_2;
    &tail_terms;
  }
}
#}}}
sub bridi_tail_2 {#{{{
  &bridi_tail_3;
  while (&rnd < $piter) {
    my $z = &in("bridi_tail_2_iter");
    &gihek;
    if (&rnd < $phi) { &stag; }
    &BO;
    &opt_free_seq;
    &bridi_tail_2;
    &tail_terms;
    &out($z);
  }
}
#}}}
sub bridi_tail_3 {#{{{
  my $z;
  if ($recur >0 && &rnd < 0.33) {
    --$recur;
    $z = &in("bridi_tail_3_gek");
    &gek_sentence;
  } else {
    $z = &in("bridi_tail_3_selbri");
    &selbri;
    &tail_terms;
  }
  &out($z);
}
#}}}
sub gek_sentence {#{{{
  my $r = &rnd;
  if ($r < 0.8) {
    &gek;
    &subsentence;
    &gik;
    &subsentence;
    &tail_terms;
  } elsif ($r < 0.9) {
    if (&rnd < $phi) { &tag; }
    &KE;
    &opt_free_seq;
    &gek_sentence;
    &KEhE;
    &opt_free_seq;
  } else {
    &NA;
    &opt_free_seq;
    &gek_sentence;
  }
}
#}}}
sub tail_terms {#{{{
  my $z = &in("tail_terms");
  if (&rnd < 0.667) {
    &terms;
  }
  &VAU;
  &opt_free_seq;
  &out($z);
}
#}}}
sub terms {#{{{
  my $n = &getnum1;
  my $z = &in("terms");
  for (1..$n) { &terms_1; }
  &out($z);
}
#}}}
sub terms_1 {#{{{
  &terms_2;
  while (&rnd < $piter) {
    &PEhE;
    &opt_free_seq;
    &joik_jek;
    &terms_2;
  }
}
#}}}
sub terms_2 {#{{{
  &term;
  while (&rnd < $piter) {
    &CEhE;
    &opt_free_seq;
    &term;
  }
}
#}}}
sub term {#{{{
# FIXME - reenable termset
  if (0 && $recur>0 && &rnd < 0.05) { --$recur; &termset; }
  else {
    my $r = &rnd;
    if ($r < 0.45) {
      &sumti;
    } elsif ($r < 0.9) {
      if (&rnd < 0.5) { &tag; }
      else            { &FA; &opt_free_seq; }

      if (&rnd < 0.75) { &sumti; }
      else             { &KU; &opt_free_seq; }
    } else {
      &NA;
      &KU;
      &opt_free_seq;
    }
  }
}
#}}}
sub termset {#{{{
  my $z;
  if (&rnd < 0.5) {
    $z = &in("termset_gek");
    &NUhI;
    &opt_free_seq;
    &gek;
    &terms;
    &NUhU;
    &opt_free_seq;
    &gik;
    &terms;
    &NUhU;
    &opt_free_seq;
  } else {
    $z = &in("termset_terms");
    &NUhI;
    &opt_free_seq;
    &terms;
    &NUhU;
    &opt_free_seq;
  }
  &out($z);
}
#}}}
sub sumti {#{{{
  &sumti_1;
  if ($recur>0 && &rnd < $pmed) {
    --$recur;
    &VUhO;
    &relative_clauses;
  }
}
#}}}
sub sumti_1 {#{{{
  &sumti_2;
  if ($recur>0 && &rnd<$pmed) {
    my $z = &in("sumti_1_iter");
    --$recur;
    if (&rnd < 0.5) { &ek; }
    else            { &joik; }
    if (&rnd < $phi) { &stag; }
    &KE;
    &opt_free_seq;
    &sumti;
    &KEhE;
    &opt_free_seq;
    &out($z);
  }
}
#}}}
sub sumti_2 {#{{{
  &sumti_3;
  while (&rnd < $piter) {
    &joik_ek;
    &sumti_3;
  }
}
#}}}
sub sumti_3 {#{{{
  my $z = &in("sumti_3");
  &sumti_4;
  if (&rnd < 0.05) {
    if (&rnd < 0.5) { &ek; }
    else            { &joik; }
    if (&rnd < $phi) { &stag; }
    &BO;
    &opt_free_seq;
    &sumti_3;
  }
  &out($z);
}
#}}}
sub sumti_4 {#{{{
  if ($recur>0 && &rnd < 0.1) {
    --$recur;
    &gek;
    &sumti;
    &gik;
    &sumti_4;
  } else {
    &sumti_5;
  }
}
#}}}
sub sumti_5 {#{{{
  my $z;
  if (&rnd < 0.5) {
    $z = &in("sumti_5_qs6");
    if (&rnd < $pmed) { &quantifier; }
    &sumti_6;
  } else {
    $z = &in("sumti_5_selbri");
    &quantifier;
    &selbri;
    &KU;
    &opt_free_seq;
  }
  if (&rnd < $p{relative_clauses}) { &relative_clauses; }
  &out($z);
}
#}}}
sub sumti_6 {#{{{
  my $r = &rnd;
  my $z = &in("sumti_6");
  ## FIXME -not all cases done
  if ($recur>0 && $r<0.05) {
    --$recur;
    if (&rnd < 0.25) { &LAhE; &opt_free_seq; }
    else             { &NAhE; &BO; &opt_free_seq; }

    if (&rnd < $p{relative_clauses}) { &relative_clauses; }
    &sumti;
    &LUhU;
    &opt_free_seq;
  } elsif ($recur>0 && $r<0.06) {
    --$recur;
    &LU;
    &text;
    &LIhU;
    &opt_free_seq;
  } else {
    # Non recursive cases
    $r = &rnd;
    if ($r < 0.15) {
      &KOhA;
    } elsif ($r < 0.6) {
      if (&rnd < 0.5) { &LA; }
      else            { &LE; }
      &opt_free_seq;
      &sumti_tail;
      &KU;
      &opt_free_seq;
    } else {
      &LA;
      &opt_free_seq;
      if ($recur>0 && &rnd < $p{relative_clauses}) { --$recur; &relative_clauses; }
      my $n = &getnum1;
      for (1..$n) { &CMENE; }
      &opt_free_seq;
    }
  }
  &out($z);
}
#}}}
sub sumti_tail {#{{{
  if ($recur>0 && &rnd<0.25) {
    --$recur;
    &relative_clauses;
    &sumti_tail_1;
  } else {
    if ($recur>0 && &rnd<$pmed) {
      --$recur;
      &sumti_6;
      if ($recur>0 && &rnd<$p{relative_clauses}) {
        --$recur;
        &relative_clauses;
      }
    }
    &sumti_tail_1;
  }
}
#}}}
sub sumti_tail_1 {#{{{
  if ($recur>0 && &rnd<0.05) {
    --$recur;
    &quantifier;
    &sumti;
  } else {
    if (&rnd < $pmed) { &quantifier; }
    &selbri;
    if ($recur>0 && &rnd<$p{relative_clauses}) {
      --$recur;
      &relative_clauses;
    }
  }
}
#}}}
sub relative_clauses {#{{{
  &relative_clause;
  while (&rnd < $piter) {
    &ZIhE;
    &opt_free_seq;
    &relative_clause;
  }
}
#}}}
sub relative_clause {#{{{
  if (&rnd < 0.5) {
    &GOI;
    &opt_free_seq;
    &term;
    &GEhU;
    &opt_free_seq;
  } else {
    &NOI;
    &opt_free_seq;
    &subsentence;
    &KUhO;
    &opt_free_seq;
  }
}
#}}}
sub selbri {#{{{
  my $z = &in("selbri");
  while (&rnd < $piter) {
    if (&rnd < $phi) { &tag; }
    &NA;
    &opt_free_seq;
  }
  if (&rnd < $phi) { &tag; }
  &selbri_2;
  &out($z);
}
#}}}
sub selbri_2 {#{{{
  my $z = &in("selbri_2");
  &selbri_3;
  if (&rnd < $pmed) {
    &CO;
    &opt_free_seq;
    &selbri_2;
  }
  &out($z);
}
#}}}
sub selbri_3 {#{{{
  &selbri_4;
  while (&rnd < $piter) {
    &selbri_4;
  }
}
#}}}
sub selbri_4 {#{{{
  &selbri_5;
  while (&rnd < $piter) {
    if ($recur>0 && &rnd<$pmed) {
      --$recur;
      &joik;
      if ($rndrnd < 0.5) { $stag; }
      &KE;
      &opt_free_seq;
      &selbri_3;
      &KEhE;
      &opt_free_seq;
    } else {
      &joik_jek;
      &selbri_5;
    }
  }
}
#}}}
sub selbri_5 {#{{{
  &selbri_6;
  if (&rnd < $pmed) {
    if (&rnd < 0.5) { &jek; }
    else            { &joik; }
    if (&rnd < $phi) { &stag; }
    &BO;
    &opt_free_seq;
    &selbri_5;
  }
}
#}}}
sub selbri_6 {#{{{
  my $z;
  if ($recur>0 && &rnd<0.02) {
    --$recur;
    $z = &in("selbri_6_guhek");
    if (&rnd < $phi) { &NAhE; &opt_free_seq; }
    &guhek;
    &selbri;
    &gik;
    &selbri_6;
  } else {
    $z = &in("selbri_6_tu_bo_s6");
    &tanru_unit;
    if (&rnd < $piter) {
      &BO;
      &opt_free_seq;
      &selbri_6;
    }
  }
  &out($z);
}
#}}}
sub tanru_unit {#{{{
  my $n = &getnum1 - 1;
  &tanru_unit_1;
  while (&rnd < $piter) {
    &CEI;
    &opt_free_seq;
    &tanru_unit_1;
  }
}
#}}}
sub tanru_unit_1 {#{{{
  &tanru_unit_2;
  if ($recur>0 && &rnd < $pmed) {
    --$recur;
    &linkargs;
  }
}
#}}}
sub tanru_unit_2 {#{{{
  my $r = &rnd;
  my $z = undef;
  if ($recur>0 && $r<0.01) {
    --$recur;
    $z = &in("tanru_unit_2_ke_s3");
    &KE;
    &opt_free_seq;
    &selbri_3;
    &KEhE;
    &opt_free_seq;
  } elsif ($recur>0 && $r<0.02) {
    --$recur;
    $z = &in("tanru_unit_2_me");
    &ME;
    &opt_free_seq;
    &sumti;
    &MEhU;
    &opt_free_seq;
    if (&rnd < $pmed) { &MOI; &opt_free_seq; }
  } elsif ($recur>0 && $r<0.05) {
    $z = &in("tanru_unit_2_nu");
    &NU;
    if (&rnd < $pmed) { &NAI; }
    &opt_free_seq;
    my $n = (&rnd<0.95) ? 0 : (&getnum1 - 1);
    for (1..$n) {
      &joik_jek;
      &NU;
      if (&rnd < $pmed) { &NAI; }
      &opt_free_seq;
    }
    &subsentence;
    &KEI;
    &opt_free_seq;
  } else {
    if ($r < 0.1) {
      &SE;
      &opt_free_seq;
      &tanru_unit_2;
    } elsif ($r < 0.15) {
      &NAhE;
      &opt_free_seq;
      &tanru_unit_2;
    } elsif ($r < 0.2) {
      &JAI;
      &opt_free_seq;
      if (&rnd < $phi) { &tag; }
      &tanru_unit_2;
    } elsif ($r < 0.5) {
      &BRIVLA;
      &opt_free_seq;
    } elsif ($r < 0.8) {
## FIXME
      $z = &in("tanru_unit_2_moi");
      &number;
      &MOI;
      &opt_free_seq;
    } else {
      &GOhA;
      if (&rnd < $pmed) { &RAhO; }
      &opt_free_seq;
    }
  }
  &out($z) if (defined $z);
}
#}}}
sub linkargs {#{{{
  &BE;
  &opt_free_seq;
  &term;
  if (&rnd < $pmed) {
    &links;
  }
  &BEhO;
  &opt_free_seq;
}
#}}}
sub links {#{{{
  &BEI;
  &opt_free_seq;
  &term;
  if (&rnd < $piter) {
    &links;
  }
}
#}}}
sub quantifier {#{{{
  #FIXME
  &number;
  &BOI;
  &opt_free_seq;
}
#}}}
sub ek {#{{{
  if (&rnd < $phi) { &NA; }
  if (&rnd < $phi) { &SE; }
  &A;
  if (&rnd < $phi) { &NAI; }
}
#}}}
sub gihek {#{{{
  if (&rnd < $phi) { &NA; }
  if (&rnd < $phi) { &SE; }
  &GIhA;
  if (&rnd < $phi) { &NAI; }
}
#}}}
sub jek {#{{{
  if (&rnd < $phi) { &NA; }
  if (&rnd < $phi) { &SE; }
  &JA;
  if (&rnd < $phi) { &NAI; }
}
#}}}
sub joik {#{{{
  my $r = &rnd;
  if ($r < 0.333) {
    if (&rnd < $phi) { &SE; }
    &JOI;
    if (&rnd < $phi) { &NAI; }
  } elsif ($r < 0.667) {
    &interval;
  } else {
    &GAhO; &interval; &GAhO;
  }
}
#}}}
sub interval {#{{{
  if (&rnd < $phi) { &SE; }
  &BIhI;
  if (&rnd < $phi) { &NAI; }
}
#}}}
sub joik_ek {#{{{
  if (&rnd < 0.5) { &ek; }
  else            { &joik; }
  &opt_free_seq;
}
#}}}
sub joik_jek {#{{{
  if (&rnd < 0.5) { &jek; }
  else            { &joik; }
  &opt_free_seq;
}
#}}}
sub gek {#{{{
  my $r = &rnd;
  if ($r < 0.2) {
    if (&rnd < $phi) { &SE; }
    &GA;
    if (&rnd < $phi) { &NAI; }
    &opt_free_seq;
  } elsif ($r < 0.5) {
    &joik;
    &GI;
    &opt_free_seq;
  } else {
    &stag;
    &GI;
  }
}
#}}}
sub guhek {#{{{
  if (&rnd < $phi) { &NA; }
  if (&rnd < $phi) { &SE; }
  &GUhA;
  if (&rnd < $phi) { &NAI; }
}
#}}}
sub gik {#{{{
  &GI;
  if (&rnd < $phi) { &NAI; }
  &opt_free_seq;
}
#}}}
sub tag {#{{{
  &tense_modal;
  while (&rnd < $piter) {
    &joik_jek;
    &tense_modal;
  }
}
#}}}
sub tense_modal {#{{{
  if ($recur>0 && &rnd < 0.08) {
    --$recur;
    &FIhO;
    &opt_free_seq;
    &selbri;
    &FEhU;
    &opt_free_seq;
  } else {
    &simple_tense_modal;
    &opt_free_seq;
  }
}
#}}}
sub stag {#{{{
  my $z = &in("stag");
  &simple_tense_modal;
  while (&rnd < $piter) {
    if (&rnd < 0.5) { &jek; }
    else            { &joik; }
    &simple_tense_modal;
  }
  &out($z);
}
#}}}

sub opt_free_seq {#{{{

}
#}}}

$failures = 0;
$case = 0;
$| = 1;
while ($failures < 5) {
  &clear_output;
  &clear_stack;
  &text;
  $output .= "\n";

  open(OUT, "|../../jbofihe > jbofihe.out 2>jbofihe.err");
  print OUT $output;
  close (OUT);

  $case++;
  if ($case%250 == 0) { print STDERR "$case\n"; }
  $result = $?;
  if ($result != 0) {
    $failures++;
    print STDERR "Failure $failures at case_$case\n";
    open (TEXT, ">case_$usseeding_$case.txt");
    print TEXT $output;
    close (TEXT);
    open (TRACE, ">case_$usseeding_$case.gen");
    for my $t (@trace) {
      print TRACE $t."\n";
    }
    close(TRACE);
    if (-r "jbofihe.out") { rename "jbofihe.out", "jbofihe_$usseeding_$case.out"; }
    if (-r "jbofihe.err") { rename "jbofihe.err", "jbofihe_$usseeding_$case.err"; }
  }
}

# vimsw=2:ts=2:et:syntax=OFF
