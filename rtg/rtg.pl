#!/usr/bin/env perl
#
#

$output = "";

# Set probabilities for various optional things.
($plo, $pmed, $phi, $piter) = (0.05, 0.1, 0.4, 0.3);

# Number of times the expansion can recurse back to a higher level construct
# (prevent infinite recursion)
$recur = 0;

# {{{ %p : Associative array defining probabilities for taking various optional constructs
%p = (
  relative_clauses => $pmed
);
#}}}

sub rnd {#{{{
    return rand;
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
sub emit {#{{{
  my ($x) = @_;
  $output .= $x;
  $llen += length($x);
  if ($llen > 72) {
    $output .= "\n";
    $llen = 0;
  } else {
    $output .= " ";
    $llen++;
  }
}
#}}}

sub A {#{{{
  &emit(&pickx("a", "e", "o", "u", "ji"));
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
  emit($BAI[$r]);
}
#}}}
sub BE {#{{{
  &emit("be");
}
#}}}
sub BEhO {#{{{
  &emit("be'o");
}
#}}}
sub BEI {#{{{
  &emit("bei");
}
#}}}
sub BIhI {#{{{
  &emit(&pickx("bi'i", "bi'o", "mi'i"));
}
#}}}
sub BO {#{{{
  emit("bo");
}
#}}}
sub BOI {#{{{
  &emit("boi");
}
#}}}
sub BRIVLA {#{{{
# FIXME
  &emit("zdani");
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
  emit($CAhA[$r]);
}
#}}}
sub CEI {#{{{
  emit("cei");
}
#}}}
sub CMENE {#{{{
  &emit("london.");
}
#}}}
sub CO {#{{{
  &emit("co");
}
#}}}
sub CU {#{{{
  &emit("cu");
}
#}}}
sub CUhE {#{{{
  emit("cu'e");
}
#}}}
@FA = (#{{{
  "fa", "fe", "fi", "fo", "fu", "fai", "fi'a"
);
#}}}
sub FA {#{{{
  &emit(&pick(\@FA));
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
  &emit (&pick(\@FAhA));
};
#}}}
sub FEhE {#{{{
  &emit("fe'e");
}
#}}}
sub FEhU {#{{{
  &emit("fe'u");
}
#}}}
sub FIhO {#{{{
  &emit("fi'o");
}
#}}}
sub GA {#{{{
  &emit(&pickx("ga", "ge", "ge'i", "go", "gu"));
}
#}}}
sub GAhO {#{{{
  &emit(&pickx("ga'o", "ke'i"));
}
#}}}
sub GEhU {#{{{
  &emit("ge'u");
}
#}}}
sub GI {#{{{
  &emit("gi");
}
#}}}
sub GIhA {#{{{
  &emit(&pickx("gi'a", "gi'e", "gi'i", "gi'o", "gi'u"));
}
#}}}
@GOhA = (#{{{
  "bu'a", "bu'e", "bu'i", "co'e", "du",
  "go'a", "go'e", "go'i", "go'o", "go'u",
  "mo", "nei", "no'a"
);
#}}}
sub GOhA {#{{{
  &emit(&pick(\@GOhA));
}
#}}}
@GOI = (#{{{
"goi", "ne", "no'u", "pe", "po",
"po'e", "po'u"
);
#}}}
sub GOI {#{{{
  &emit(&pick(\@GOI));
}
#}}}
sub GUhA {#{{{
  &emit(&pickx("gu'a", "gu'e", "gu'i", "gu'o", "gu'u"));
}
#}}}
sub I {#{{{
  emit("i");
}
#}}}
sub JA {#{{{
  &emit(&pickx("ja", "je", "je'i", "jo", "ju"));
}
#}}}
sub JAI {#{{{
  &emit("jai");
}
#}}}
@JOI = (#{{{
"ce", "ce'o", "fa'u", "jo'e", "jo'u",
"joi", "ku'a", "pi'u"
);
#}}}
sub JOI {#{{{
  &emit(&pick(\@JOI));
}
#}}}
sub KE {#{{{
  &emit("ke");
}
#}}}
sub KEhE {#{{{
  &emit("ke'e");
}
#}}}
sub KEI {#{{{
  &emit("kei");
}
#}}}
sub KI {#{{{
  emit("ki");
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
  &emit(&pick(\@KOhA));
}
#}}}
sub KU {#{{{
  emit("ku");
}
#}}}
sub KUhO {#{{{
  &emit("ku'o");
}
#}}}
@LAhE = (#{{{
"la'e", "lu'a", "lu'e", "lu'i",
"lu'o", "tu'a", "vu'i"
);
#}}}
sub LAhE {#{{{
  &emit(&pick(\@LAhE));
}
#}}}
sub LA {#{{{
  &emit(&pickx("la", "lai", "la'i"));
}
#}}}
@LE = (#{{{
"le", "le'e", "le'i", "lei",
"lo", "lo'e", "lo'i", "loi"
);
#}}}
sub LE {#{{{
  &emit(&pick(\@LE));
}
#}}}
sub LIhU {#{{{
  emit("li'u");
}
#}}}
sub LU {#{{{
  emit("lu");
}
#}}}
sub LUhU {#{{{
  &emit("lu'u");
}
#}}}
sub ME {#{{{
  &emit("me");
}
#}}}
sub MEhU {#{{{
  &emit("me'u");
}
#}}}
sub MOhI {#{{{
  emit ("mo'i");
}
#}}}
@MOI = (#{{{
"cu'o", "mei", "moi", "si'e"
);
#}}}
sub MOI {#{{{
  &emit(&pick(\@MOI));
}
#}}}
sub NA {#{{{
  emit("na");
}
#}}}
sub NAhE {#{{{
  emit("na'e");
}
#}}}
sub NAI {#{{{
  emit("nai");
}
#}}}
sub NIhO {#{{{
  &emit(&pickx("ni'o", "no'i"));
}
  #}}}
sub NOI {#{{{
  &emit(&pickx("noi", "poi", "voi"));
}
#}}}
@NU = (#{{{
"du'u", "jei", "ka", "li'i", "ni",
"nu", "si'o", "su'u"
);
#}}}
sub NU {#{{{
  &emit(&pick(\@NU));
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
  &emit(&pick(\@PA));
}
#}}}
sub PU {#{{{
  $r = &rnd;
  if ($r < 0.333) { emit("pu"); }
  elsif ($r < 0.667) { emit("ca"); }
  else { emit("ba"); }
}
#}}}
sub RAhO {#{{{
  &emit("ra'o");
}
#}}}
sub ROI {#{{{
  &emit("roi");
}
#}}}
sub SE {#{{{
  my $r = &rnd;
  if ($r < 0.25) { emit("se"); }
  elsif ($r < 0.5) { emit("te"); }
  elsif ($r < 0.75) { emit("ve"); }
  else { emit("xe"); }
}
#}}}
@TAhE = (#{{{
  "di'i", "na'o", "ru'i", "ta'e"
);
  #}}}
sub TAhE {#{{{
  emit (&pick(\@TAhE));
}
#}}}
sub TUhE {#{{{
  emit("tu'e");
}
  #}}}
sub TUhU {#{{{
  emit("tu'u");
}
  #}}}
sub VA {#{{{
  my $r = &rnd;
  if ($r < 0.333) { emit ("va"); }
  elsif ($r < 0.667) { emit ("vi"); }
  else { emit ("vu"); }
}
#}}}
sub VAU {#{{{
  emit("vau");
}
#}}}
@VEhA = (#{{{
  "ve'a", "ve'e", "ve'i", "ve'u"
);
#}}}
sub VEhA {#{{{
  emit (&pick(\@VEhA));
}
#}}}
@VIhA = (#{{{
  "vi'a", "vi'e", "vi'i", "vi'u"
);
#}}}
sub VIhA {#{{{
  emit (&pick(\@VIhA));
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
  emit (&pick(\@ZAhO));
}
#}}}
sub ZEhA {#{{{
  my $r = &rnd;
  if ($r < 0.25) { emit("ze'a"); }
  elsif ($r < 0.5) { emit("ze'e"); }
  elsif ($r < 0.75) { emit("ze'i"); }
  else { emit("ze'u"); }
}
#}}}
sub ZI {#{{{
  $r = &rnd;
  if ($r < 0.333) { emit("zi"); }
  elsif ($r < 0.667) { emit("za"); }
  else { emit("zu"); }
}
#}}}
sub ZIhE {#{{{
  emit("zi'e");
}
#}}}
sub ZOhU {#{{{
  emit("zo'u");
}
#}}}
#
sub number {#{{{
## FIXME
  $n = &getnum1;
  &emit("pa");
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
  ##FIXME
  &text_1;
}
#}}}
sub text_1 {#{{{
  ##FIXME
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
  if (1 || &rnd < 0.95) { # FIXME
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
  if (&rnd > $piter) {
    &statement_1;
  } else {
    &prenex;
    &statement;
  }
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
##FIXME
  die;
}
#}}}
sub prenex {#{{{
  &terms;
  &ZOhU;
  &opt_free_seq;
}
#}}}
sub sentence {#{{{
  if (&rnd < 0.5) {
    &terms;
    &CU;
    &opt_free_seq;
  }
  &bridi_tail;
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
  ## FIXME
  &bridi_tail_1;
}
#}}}
sub bridi_tail_1 {#{{{
  ## FIXME
  &bridi_tail_2;
}
#}}}
sub bridi_tail_2 {#{{{
  ## FIXME
  &bridi_tail_3;
}
#}}}
sub bridi_tail_3 {#{{{
  if (0 && &rnd < 0.33) { # FIXME
    &gek_sentence;
  } else {
    &selbri;
    &tail_terms;
  }
}
#}}}
sub gek_sentence {#{{{
  ##FIXME
  die;
}
#}}}
sub tail_terms {#{{{
  if (&rnd < 0.667) {
    &terms;
  }
  &VAU;
  &opt_free_seq;
}
#}}}
sub terms {#{{{
  my $n = &getnum1;
  for (1..$n) { &terms_1; }
}
#}}}
sub terms_1 {#{{{
  ##FIXME
  &terms_2;
}
#}}}
sub terms_2 {#{{{
  ## FIXME
  &term;
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
  ##FIXME
  die;
}
#}}}
sub sumti {#{{{
  #FIXME
  &sumti_1;
}
#}}}
sub sumti_1 {#{{{
  #FIXME
  &sumti_2;
}
#}}}
sub sumti_2 {#{{{
##FIXME
  &sumti_3;
}
#}}}
sub sumti_3 {#{{{
  &sumti_4;
  if (&rnd < 0.05) {
    if (&rnd < 0.5) { &ek; }
    else            { &joik; }
    if (&rnd < $phi) { &stag; }
    &BO;
    &opt_free_seq;
    &sumti_3;
  }
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
  if (&rnd < 0.5) {
    if (&rnd < $pmed) { &quantifier; }
    &sumti_6;
  } else {
    &quantifier;
    &selbri;
    &KU;
    &opt_free_seq;
  }
  if (&rnd < $p{relative_clauses}) { &relative_clauses; }
}
#}}}
sub sumti_6 {#{{{
  my $r = &rnd;
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
}
#}}}
sub sumti_tail {#{{{
  if ($recur>0 && &rnd<0.25) {
    --$recur;
    $relative_clauses;
    $sumti_tail_1;
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
  while (&rnd < $piter) {
    if (&rnd < $phi) { &tag; }
    &NA;
    &opt_free_seq;
  }
  if (&rnd < $phi) { &tag; }
  &selbri_2;
}
#}}}
sub selbri_2 {#{{{
  &selbri_3;
  if (&rnd < $pmed) {
    &CO;
    &opt_free_seq;
    &selbri_2;
  }
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
  if ($recur>0 && &rnd<0.02) {
    --$recur;
    if (&rnd < $phi) { &NAhE; &opt_free_seq; }
    &guhek;
    &selbri;
    &gik;
    &selbri_6;
  } else {
    &tanru_unit;
    if (&rnd < $piter) {
      &BO;
      &opt_free_seq;
      &selbri_6;
    }
  }
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
  if ($recur>0 && $r<0.01) {
    --$recur;
    &KE;
    &opt_free_seq;
    &selbri_3;
    &KEhE;
    &opt_free_seq;
  } elsif ($recur>0 && $r<0.02) {
    --$recur;
    &ME;
    &opt_free_seq;
    &sumti;
    &MEhU;
    &opt_free_seq;
    if (&rnd < $pmed) { &MOI; &opt_free_seq; }
  } elsif ($recur>0 && $r<0.05) {
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
      &number;
      &MOI;
      &opt_free_seq;
    } else {
      &GOhA;
      if (&rnd < $pmed) { &RAhO; }
      &opt_free_seq;
    }
  }
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
    &JA;
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
  &simple_tense_modal;
  while (&rnd < $piter) {
    if (&rnd < 0.5) { &jek; }
    else            { &joik; }
    &simple_tense_modal;
  }
}
#}}}

sub opt_free_seq {#{{{

}
#}}}

$| = 1;
for (1..50) {
  $output = "";
  $llen = 0;
  &text;
  $output .= "\n";

  print $output;
  open(OUT, "|../jbofihe");
  print OUT $output;
  close (OUT);
  print "------------------------------\n";
}

# vimsw=2:ts=2:et:syntax=OFF
