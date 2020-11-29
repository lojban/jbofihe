#!/usr/bin/perl -w
#
# Locate lines of Lojban text from a file containing some lines
# which are lojban and some which are not.
#
# Requires Getopt::Long
#
use strict;
use warnings;
use utf8;
use Data::Dumper;
use Getopt::Long;
use Pod::Usage;
use File::Temp qw/ tempfile /;

binmode STDOUT, ':encoding(UTF-8)';
binmode STDIN, ':encoding(UTF-8)';
binmode STDERR, ':encoding(UTF-8)';

##############################################################################

Getopt::Long::Configure("bundling");

# percent of words on a line which must be lojban
my $needed = 80;
my $help=0;
# how much IRC stuff to ignore at detection time
my $detect='strip_irc';
# how much IRC stuff to ignore at output time
my $output='clean_irc';
GetOptions ('help|h|?' => \$help,
            'needed|n=n' => \$needed,
            'detect|d=s'  => \$detect,
            'output|o=s'  => \$output,
          )
or pod2usage(2);

pod2usage(1) if $help;

# If too many remaining args
if (scalar(@ARGV) > 2) {
  pod2usage(2);
}
my $infile = '-';
if( $ARGV[0] ){
  $infile = $ARGV[0];
}
my $outfile = '-';
if( $ARGV[1] ){
  $outfile = $ARGV[1];
}

open INFILE, "<$infile"
  or pod2usage("\n\ncan't open file $infile $!\n\n");
binmode INFILE, ':encoding(UTF-8)';
open OUTFILE, ">$outfile"
  or pod2usage("\n\ncan't open file $infile $!\n\n");
binmode OUTFILE, ':encoding(UTF-8)';

while (<INFILE>) {
  my ($line, $orig_line);

  $orig_line = $_;

  # trim the irc-style formatting out of this line before evaluating
  if( $detect eq 'strip_irc' ) {
    if (/-----.*/ || /^--- /) {
      # These are log lines that are produced in some IRC file
      # formats; they indicate things like a change of day in the
      # log file.  Unless we're stripping IRC stuff, we just keep
      # these unaltered
      if( $output ne 'strip_irc' ) {
        print OUTFILE;
      }
      next;
    }

    # 2019-12-12
    s/^\d\d\d\d-\d\d-\d\d\s*//;
    # 06:00:52 PST/-0800
    s;^\d\d:\d\d:\d\d [A-Z][A-Z][A-Z]/[+-]?\d\d\d\d\s*;;;
    # 06:00:52 PST
    s/^\d\d:\d\d:\d\d [A-Z][A-Z][A-Z]\s*//;
    # No idea what this was for, but seems harmless
    s/^\s*\**\s*//;
    # Long-form dates
    s/^\d\d [A-Z][a-z][a-z] \d\d\d\d \d\d:\d\d:\d\d\s*//;

    if ( /^\s*-!-\s+/ ) {
      # Lines like: -!- skapata [~skapata@2804:14c:87b1:c970:4c06:5e00:85ab:52d3] has quit [Remote host closed the connection]
      if( $output eq 'raw' ) {
        print OUTFILE;
      }
      next;
    }

    # No idea what this was for, but seems harmless; some timestamp I guess
    s/^[[]\d\d:\d\d[]]\s*//;

    # Handle the double-nick thing caused by the bridges
    #
    # By this time we've cleaned out the dates and stuff, so this
    # takes text that starts with stuff like:
    #
    #   <       xxxx_> <snuti>:
    #
    # and drops the first nick, which comes from the IRC bridges, in
    # both the current string and the original string; \Q quotes
    # regex characters in the substitution
    #
    if( /^\s*(\<[^>]*\>\s*)\<[^>]*\>:\s*/ ) {
      my $bridge_nick = $1;
      if( $output ne 'raw' ) {
        $orig_line =~ s/\Q$bridge_nick//;
      }
      s/\Q$bridge_nick//;
    }

    # Nicknames
    s/^\s*\<[^>]*\>\s*:?\s*//;
    # ... some files have comments I guess?
    s/^#[^>#]*\> //;
    # The topic is interesting, the rest is not; this line hasn't
    # really been tested
    s/^.{3,16} has changed the topic to "([^"]*)"/$1/;
  }

  chomp;

  ## print "line after stripping: $_\n";

  use IPC::Open2;

  my @words = split(/\s+/);

  ## print "words: ".join("\n", @words)."\n";

  foreach my $word (@words) {
    # vlatai doesn't like leading dots; might as well kill trailing dots too
    $word =~ s/^\s*\.*//;
    $word =~ s/\.*\s*$//;
  }

  ## print "words: ".join("\n", @words)."\n";

  next if( ! @words );

  my ($from_vlatai, $to_vlatai);
  my $pid = open2($from_vlatai, $to_vlatai, 'vlatai' ) or die "vlatai won't run\n";

  binmode $to_vlatai, ':encoding(UTF-8)';

  # Have to manually decode so that we get character replacement;
  # see https://github.com/lojban/jbofihe/issues/13
  binmode $from_vlatai;

  print $to_vlatai join("\n", @words)."\n";
  close $to_vlatai;
  my @vlatai_says = <$from_vlatai>;
  close $from_vlatai;

  # Have to manually decode so that we get character replacement;
  # see https://github.com/lojban/jbofihe/issues/13
  @vlatai_says = map { Encode::decode('UTF-8', $_) } @vlatai_says;

  ## print "says: ".Dumper(\@vlatai_says)."\n";

  my $good_count=grep(m{: (cmavo\(s\)|lujvo|gismu) :}, @vlatai_says);
  my $cmene_count=grep(m{: cmene :}, @vlatai_says);
  my $total_count=scalar @vlatai_says;

  next if($good_count <= 0);

  # Special cases for non-lojban quotes
  if( grep(m{^\s*la'oi\s*$}, @words) ) {
    $total_count -= 1;
  }
  if( grep(m{^\s*zo'oi\s*$}, @words) ) {
    $total_count -= 1;
  }
  if( grep(m{^\s*zoi\s*$}, @words) ) {
    $total_count -= 3;
  }
  if( grep(m{^\s*la'o\s*$}, @words) ) {
    $total_count -= 3;
  }

  next if($total_count <= 0);

  ## print "good_count: $good_count; cmene_count: $cmene_count; total_count: $total_count\n";

  my $score=((($good_count + ($cmene_count * 0.4)) / $total_count) * 100);

  # Special cases for very short sentences.
  if ($total_count == 2 )
  {
    # In a two-word sentence, this matches 1 normal lojban
    # word and 1 cmene.
    $needed=60;
  }
  if ($total_count == 3)
  {
    # In a 3-word sentence, this is 2
    # normal lojban words and one unmatched or cmene.
    $needed=66;
  }

  ## print "score: $score, needed: $needed\n";

  if ( $score >= $needed)
  {
    ## print "PASSED! -- $_\n";
    if( $output eq 'strip_irc' ) {
      print OUTFILE "$_\n";
    } else {
      print OUTFILE "$orig_line";
    }
  }

  kill 1, $pid;
  waitpid( $pid, 0 );
}
close INFILE;
close OUTFILE;

__END__

=head1 NAME

find_lojban

=head1 SYNOPSIS

find_lojban [options] [inputfile [outputfile]]

Perl program that takes a file and returns only those lines with significant amounts of Lojban in them.

If inputfile or outputfile are empty or "-", uses stdin/stdout.

 Options:
   --help/-h            This help message
   --needed/-n          The percentage of each line that needs to seem to be Lojban for it to be approved; defaults to 80%.  Note that this is ignored for very short (1-3 word) lines
   --detect/-d          Whether or not to try to strip out IRC-isms (dates, nicknames, etc) before deciding how much Lojban a line has.  Valid values are "strip_irc" (the default) and "raw"
   --output/-o          Similar IRC handling to detect, but for output; valid values are "clean_irc" (the default, which trims things like join/part lines and IRC bridge nicks), "strip_irc" (which removes nicks, dates, and other IRC related stuff completely), and "raw"
   --ignore-irc/-i      Ignore things at the beginning of the line that look like IRC-specific stuff; things like dates and <names> and so on
   --strip-irc/-s       Implies --ignore-irc, but also strips out the data in question so that it does not appear in the resulting output
=cut
