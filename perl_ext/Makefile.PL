# $Id$

use ExtUtils::MakeMaker;
use Cwd qw(abs_path cwd);
use strict;

my $SRCDIR=$ENV{'srcdir'};
$SRCDIR="." unless $SRCDIR;
my $BUILDDIR=$ENV{'builddir'};
$BUILDDIR="." unless $BUILDDIR;


my $MING_TOP_SRCDIR=abs_path($SRCDIR."/../");
my $MING_TOP_BUILDDIR=abs_path($BUILDDIR."/../");

print "SRCDIR: ".$SRCDIR."\n";
print "BUILDDIR: ".$BUILDDIR."\n";
print "MING_TOP_SRCDIR: ".$MING_TOP_SRCDIR."\n";
print "MING_TOP_BUILDDIR: ".$MING_TOP_BUILDDIR."\n";

my $CONFIG_PL = ${BUILDDIR}.'/t/config.pl';

my @DEFAULT_DIR = qw(..);

my @do_clean = qw {
    SWF.h
    Boot.xsh
    ${CONFIG_PL}
    ${BUILDDIR}/t/gradient.swf
    SWF/MyConfig.pm
};

for(@do_clean) { unlink $_ }

my @SWF_MODULES_NAMES = (
        'Action',
	'Bitmap',
	'Button',
	'Constants',
	'DisplayItem',
	'Fill',
	'Font',
	'Gradient',
	'Morph',
	'Movie',
	'MovieClip',
	'Shape',
	'Sound',
	'SoundInstance',
	'SoundStream',
	'Text',
	'TextField',
	'VideoStream',
	'PrebuiltClip',
	'FontCharacter',
	'ButtonRecord',
	'BinaryData',
	'InitAction',
	'Matrix',
	'Shadow',
	'Blur',
	'FilterMatrix',
	'Filter',
	'CXform',
	'BrowserFont',
	'FontCollection',
	'Character'
);

my @XS_OBJECTS = map "${BUILDDIR}/$_", @SWF_MODULES_NAMES;
push @XS_OBJECTS, "${SRCDIR}/swf_util";
push @XS_OBJECTS, "${BUILDDIR}/SWF";
my $XS_OBJECTS = join ('$(OBJ_EXT) ' , (@XS_OBJECTS)) . '$(OBJ_EXT)';

my $XS_DEP_FILES = "";
foreach my $val ( @SWF_MODULES_NAMES ) {
	$XS_DEP_FILES .= "$BUILDDIR/$val.xs: \n\tln -s $SRCDIR/$val.xs .\n";
}
$XS_DEP_FILES .= "$BUILDDIR/SWF.xs: \n\tln -s $SRCDIR/SWF.xs .\n";

my @LD_OBJECTS = map "${BUILDDIR}/$_", @SWF_MODULES_NAMES;
push @LD_OBJECTS, "${BUILDDIR}/swf_util", "${BUILDDIR}/SWF";
my $LD_OBJECTS = join ('$(OBJ_EXT) ' , (@LD_OBJECTS)) . '$(OBJ_EXT)';

my $PM_HASH;
foreach my $val ( @SWF_MODULES_NAMES ) {
	$PM_HASH->{"${SRCDIR}/SWF/$val.pm"} = "blib/lib/SWF/$val.pm";
}
$PM_HASH->{"${BUILDDIR}/SWF.pm"} = "blib/lib/SWF.pm";
#also install Sprite.pm (deprecated) for compat reasons
$PM_HASH->{"${SRCDIR}/SWF/Sprite.pm"} = "blib/lib/SWF/Sprite.pm";

my $MAN3_HASH;
foreach my $val ( @SWF_MODULES_NAMES ) {
	$MAN3_HASH->{"${SRCDIR}/SWF/$val.pm"} = "blib/man3/SWF::$val.3pm";
}
$MAN3_HASH->{"${BUILDDIR}/SWF.pm"} = "blib/man3/SWF.3pm";
#also install Sprite.3pm (deprecated) for compat reasons
$MAN3_HASH->{"${SRCDIR}/SWF/Sprite.pm"} = "blib/man3/SWF::Sprite.3pm";

#$MING_DIR is not the src-dir
my ($MING_DIR, $ming_version);
#foreach my $dir (@DEFAULT_DIR) {
#    next unless -d $dir;
#    my $abs_dir = abs_path($dir);
#    $ming_version = get_ming_version($dir);
#
#    if( $ming_version ){
#      $MING_DIR = $abs_dir;
#			print STDERR $MING_DIR."\n";
#      last;
#    }
#}
#
#$MING_DIR="../";
#ask_ming_dir() unless ($MING_DIR);

$MING_DIR=$MING_TOP_BUILDDIR;
my $ming_version = get_ming_version($MING_TOP_SRCDIR);


print "\nUsing ming version $ming_version with source dir ". $MING_TOP_SRCDIR ." and build dir ". $MING_TOP_BUILDDIR .".\n\n";
#compile() unless ming_is_compiled();
print "ming library isn't compiled, it is needed for the perl module\n" unless ming_is_compiled();
# 2006/01/24 (peterdd) now we can look into ming_config.h to get some info.
my $libs=get_mingconfig($MING_TOP_BUILDDIR."/src/ming_config.h");
write_swf_h($MING_DIR."/src/");
write_config_test ($MING_DIR."/src/");
write_bootxs();

sub MY::postamble {
	"Makefile.PL: $SRCDIR/Makefile.PL\n\tln -s $SRCDIR/Makefile.PL .\n" .
	"$XS_DEP_FILES";
}


WriteMakefile(
    'NAME'	   => 'SWF',
    'VERSION_FROM' => $BUILDDIR.'/SWF.pm',
    ( $] <  5.005 ? () :
      (
       ABSTRACT   => 'Perl interface to Ming -  a library for generating SWF ("Flash") format movies.',
       AUTHOR     => 'Soheil Seyfaie (soheil at users.sourceforge.net)',
      )
    ),
    clean   => {
        FILES   => "@do_clean",
    },
    'OBJECT'       => "$XS_OBJECTS",
    'LDFROM'       => "$LD_OBJECTS",
    'INC'          => "-I$MING_TOP_SRCDIR/src -I$BUILDDIR -I$MING_TOP_BUILDDIR/src -I$SRCDIR",
    'LIBS'         => [ '-L'.$MING_TOP_BUILDDIR . '/src/.libs -lming '.$libs ],
    'PM'	   => $PM_HASH,
    'MAN3PODS'	   => $MAN3_HASH,
    'TYPEMAPS'     => ["$SRCDIR/typemap"],
);

sub MY::install {
	my $self = shift;
	write_myconfig_file($self->{INSTALLSITEARCH});
	return $self->MM::install;
}

sub ming_is_compiled
{
	my $libming = $MING_TOP_BUILDDIR."/src/.libs/libming.a";

	if ( -e $libming )
	{
		return 1;
	}
	else
	{
    		print "$libming doesn't exist.\n";
		return undef;
	}
}

# unused. At least it should take into account the configure flags by
# running ./config.status
sub compile{
    print "Compiling ming ...\n";
    my $cur_dir = cwd;
    chdir $MING_TOP_BUILDDIR;
    system $MING_TOP_SRCDIR."./configure";
    system "make";
    chdir $cur_dir;
}

sub ask_ming_dir{
    while(1) {

	print <<EOF;

You need ming source code to install this software. If you haven't installed
ming before, please download it from http://www.opaque.net (Please read the
README file).

EOF
		print "Please tell me where I can find your Ming src (type q to quit): ";
		my $src_dir = prompt("", "");
		exit(0) if $src_dir eq "q";
		if(-d $src_dir) {
			$ming_version = get_ming_version($src_dir);
			if($ming_version){
				$MING_TOP_SRCDIR = $src_dir;
				last;
			}
		}
		else {
			print "Can't stat '$src_dir'\n";
		}
	}
}

## Find Ming version inside the configure.in found in the directory
## given as first argument
sub get_ming_version{
	my $d = shift;
	my $filename = shift || "configure.in";

	my $file = "$d/$filename";
	print "Checking $file\n";

	return undef unless (-e $file);
	#print "File exists\n";

	local *FH;
	open FH, "$file" or die "can't open $file $!";

	my ($majorversion,$minorversion,$microversion);
	foreach my $i (<FH>) {
		if($i =~ /^MAJOR_VERSION=/) { $i =~ s/^MAJOR_VERSION=(.*)/$1/; chomp($majorversion=$i);}
		elsif($i =~ /^MINOR_VERSION=/) { $i =~ s/^MINOR_VERSION=(.*)/$1/; chomp($minorversion=$i);}
		elsif($i =~ /^MICRO_VERSION=/) { $i =~ s/^MICRO_VERSION=(.*)/$1/; chomp($microversion=$i);}
	}
	close(FH);
	my $version=$majorversion.'.'.$minorversion.'.'.$microversion;
	print "SWF::VERSION = $version\n";
	if( $version  =~ /^\./ ) {return undef;}
	else {return $version;}
}

# peterdd: Someone should provide a better approach to detect the needed libs
sub get_mingconfig{
	 my $file=shift;
   my $lib="";
	 print "Checking ming config $file :\n";
	 open(FH, "$file") or die "can't open $file $!";
	 while(<FH>){
      next unless /^#define/;
			if (/USE_ZLIB/ and /1$/) { $lib.=" -lz"};
			if (/USE_GIF/ and /1$/) {$lib.=" -lungif"};
			if (/USE_PNG/ and /1$/) {$lib.=" -lpng"};
			if (/USE_FREETYPE/ and /1$/) {$lib.=" -lfreetype"};
	 }
	 close(FH);
	 return $lib;
	 
}

sub write_myconfig_file{
    my $dir = shift;
    local *FH;
    mkdir "SWF";
    open FH, ">SWF/MyConfig.pm" or die "can't open MyConfig $!";
    print FH<<EOConf;
#
# SWF Configuration
#

package SWF::MyConfig;

%Conf = (
	 INSTALLSITEARCH => '$dir',
	 );
1;
EOConf

}

sub write_bootxs (){
    local *FH;
    open FH, ">${BUILDDIR}/Boot.xsh" or die "can't open Boot.xsh $!";
    print FH "BOOT:\n{\n";
    print FH join "\n", map "extern void boot_SWF__$_(CV *cv);", @SWF_MODULES_NAMES;
    print FH "\n", join "\n", map "callXS(boot_SWF__$_, cv, mark);", @SWF_MODULES_NAMES;
     print FH "\n}\n";
    close FH;
}

# We don't really need this function anymore. 
# We may add $dir to INC of WriteMakefile()
sub write_swf_h {
    local *FH;
    open FH, ">SWF.h" or die "can't open SWF.h $!";
    print FH <<EOF;
#include "ming.h"
EOF
    close FH;
}


sub write_config_test {
    my $dir = shift;
    $dir = abs_path($dir);
    unlink ${CONFIG_PL};
    mkdir $BUILDDIR."/t";
    local *FH;
    open FH, ">${CONFIG_PL}" or die "can't open ${CONFIG_PL} $!";
    print FH <<EOF;
# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
#  ====================================================================


use strict;

{
    my \$MING_DIR = '$dir';
    sub ming_dir(){return \$MING_DIR};
}


EOF

print FH <<'EOF';
{
    my $test_no = 1;

    # syntax: ok(\$n == 1);
    sub ok {
	print "not " if $_[0];
	print "ok $test_no\n";
	$test_no++;
    }

    sub test_ok ($$) {
	my($got, $want) = @_;    
	return ok() if $got eq $want;
	warn "Test $test_no: wanted '$want', got '$got'\n";
	ok(1);
    }
    sub skip_test {
	print "1..0\n";
	exit;
    }
}

EOF
    close FH;
}
    




