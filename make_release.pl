#!/usr/bin/env perl
#
# $Header$
#
# Copyright 2001 Richard P. Curnow
# LICENCE
#

# This script is executed in the directory where the jbofihe-x.xx.tar.gz file
# is required to end up.

# Arguments are :
# (required) version number of new release
# (optional) version number(s) of release(s) to create patches against

# It is assumed that any subdirectory of the current directory is fair game for
# overwriting (so previous versions can be expanded for diff'ing.)

$newver = shift @ARGV or die "No new release version specified";
@oldver = @ARGV;
$program = "jbofihe";

# Check all old versions exist.
print STDERR "Check old versions exist for patching from\n";
for $o (@oldver) {
    $tarball = $program."-".$o.".tar.gz";
    die "Can't find tarball for old version $o" unless (-r $tarball);
}

die "Bad format for new version number" unless ($newver =~ /[0-9]\.[0-9]+((-pre[0-9]+)|([a-z]+))/);

$tag = $newver;
$tag =~ s/\-/_/go;
$tag =~ s/\./_/go;
$tag = "V".$tag;

# Do tagging
print STDERR "Tagging repository for new release.\n";
system ("cvs rtag -F $tag $program");
die "Tagging failed" if ($?);

# Export
print STDERR "Export new release\n";
$subdir = $program."-".$newver;
system ("cvs export -d $subdir -r $tag $program");
die "Export failed" if ($?);

# Configure
print STDERR "Configure new source kit\n";
system ("cd $subdir ; perl build_kit");
die "build_kit failed" if ($?);

# Tar up
print STDERR "Create tar.gz file\n";
system ("tar cvf - $subdir | gzip -9 > $subdir.tar.gz");
die "tar/gzip failed" if ($?);

# Create patch files
print STDERR "Create patches\n";
for $o (@oldver) {
    system ("rm -rf $program-$o");
    $tarball = $program."-".$o.".tar.gz";
    system ("tar xzvf $tarball");
    system ("diff -urN $program-$o $program-$newver | gzip -9 > $program-$o-$newver.patch.gz");
}

# Generate LSM file for the new release

print STDERR "Create new LSM file\n";
$date = qx|date +%d%b%y|;
$date =~ y/a-z/A-Z/;
$size = (stat("$program-$newver.tar.gz")) [7];
$size = int($size / 1024);
$size .= "k";

open (OUT, ">jbofihe.lsm");
print OUT <<EOF;
Begin3
Title:          jbofihe
Version:        $newver
Entered-date:   $date
Description:    A parser for Lojban, a constructed human language
                with a machine-parseable grammar (see www.lojban.org).
                Also outputs rough English translations.  Several
                related utilities bundled.
Keywords:       lojban, constructed language
Author:         rpc\@myself.com (Richard Curnow)
Maintained-by:  rpc\@myself.com (Richard Curnow)
Primary-site:   sunsite.unc.edu /pub/Linux/apps/misc
                $size jbofihe-$newver.tar.gz
Platforms:      Unix, MSDOS
Copying-policy: GPL
End
EOF

close (OUT);

