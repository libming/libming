# $Id$

use ExtUtils::MakeMaker;
use Cwd qw(abs_path cwd);
use strict;

my @DEFAULT_DIR = qw(.. ../src);

my $is_soheil;
#$is_soheil = (defined($ENV{USER}) && ($ENV{USER} eq "soheil"));
#$Verbose=1 if $is_soheil;



my @do_clean = qw {
    SWF.h
    Boot.xsh
    t/config.pl
    t/gradient.swf
    SWF/MyConfig.pm
};

for(@do_clean) { unlink $_ }

my @SWF_MODULES = qw {
    Action
    Bitmap
    Button
    Constants
    DisplayItem
    Fill
    Font
    Gradient
    Morph
    Movie
    MovieClip
    Shape
    Sound
	SoundInstance
	SoundStream
    Text
    TextField
    VideoStream
};

my $XS_OBJECTS = join ('$(OBJ_EXT) ' , (@SWF_MODULES, qw(swf_util SWF))) . '$(OBJ_EXT)';

my ($MING_DIR, $ming_version);

foreach my $src_dir (@DEFAULT_DIR) {
    next unless -d $src_dir;
    my $abs_src_dir = abs_path($src_dir);
    print "    Trying $abs_src_dir ... " if $is_soheil;
    $ming_version = get_ming_version($src_dir);

    if( $ming_version ){
	$MING_DIR = $src_dir;
        print "Yes\n" if $is_soheil;
	last;
    }
    print "No\n" if $is_soheil;
}

ask_ming_dir() unless ($MING_DIR);

print "\nUsing ming version $ming_version in the ". abs_path($MING_DIR) ." directory.\n\n";
compile() unless ming_is_compiled();
write_swf_h($MING_DIR);
write_config_test ($MING_DIR);
write_bootxs();

WriteMakefile(
    'NAME'	   => 'SWF',
    'VERSION_FROM' => 'SWF.pm',
    ( $] <  5.005 ? () :
      (
       ABSTRACT   => 'Perl interface to Ming -  a library for generating SWF ("Flash") format movies.',
       AUTHOR     => 'Soheil Seyfaie (soheil at users.sourceforge.net)',
      )
    ),
    clean   => {
        FILES   => "@do_clean",
    },
    'MYEXTLIB'     => $MING_DIR . '/libming.a',
    'OBJECT'       => "$XS_OBJECTS",
    'LIBS'         => [ "-lz" ],
);

sub MY::install {
    my $self = shift;
    write_myconfig_file($self->{INSTALLSITEARCH});

    for (qw(ming.h libming.a)){
	$self->{PM}->{"$MING_DIR/$_"} = '$(INST_ARCHLIB)' . "/auto/SWF/include/$_";
    }
    $self->{PM}->{'perl_swf.h'} = '$(INST_ARCHLIB)' . "/auto/SWF/include/perl_swf.h";
    return $self->MM::install;
}

sub ming_is_compiled{
    return undef unless (-e "$MING_DIR/libming.a");
    return 1;
}

sub compile{
    print "Compiling ming ...\n";
    my $cur_dir = cwd;
    chdir $MING_DIR;
    system "make static";
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
	   if( $ming_version && version_supported($ming_version) ){
	       $MING_DIR = $src_dir;
	       last;
	   }
	}
	else {
	    print "Can't stat `$src_dir'\n";
	}
    }
}

sub get_ming_version{
    my $d = shift;
    my $filename = shift || "ming.h";

    my $file = "$d/$filename";
    print "Checking $file\n" if $is_soheil;

    return undef unless (-e $file);

    #print "File exists\n";

    local *FH;
    open FH, "$file" or die "can't open $file $!";

    my $version;
    while(<FH>) {
	next unless /^#define/;
	next unless s/^#define\s+MING_VERSION\s+(.*)/$1/;
	chomp($version=$_);
	print "MING VERSION = $version\n" if $is_soheil;
        return $version;
    }
    close(FH);
    return undef;
}

sub write_myconfig_file{
    my $dir = shift;
    local *FH;
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
    open FH, ">Boot.xsh" or die "can't open Boot.xsh $!";
    print FH "BOOT:\n{\n";
    print FH join "\n", map "extern void boot_SWF__$_(CV *cv);", @SWF_MODULES;;
    print FH "\n", join "\n", map "callXS(boot_SWF__$_, cv, mark);", @SWF_MODULES;
     print FH "\n}\n";
    close FH;
}

# We don't really need this function anymore. 
# We may add $dir to INC of WriteMakefile()
sub write_swf_h {
    my $dir = shift;
    local *FH;
    open FH, ">SWF.h" or die "can't open SWF.h $!";
    print FH <<EOF;
/* ====================================================================
 * Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 */

#include "$dir/src/ming.h"
EOF
    close FH;
}


sub write_config_test {
    my $dir = shift;
    $dir = abs_path($dir);
    unlink "t/config.pl";
    local *FH;
    open FH, ">t/config.pl" or die "can't open t/config.pl $!";
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
    




