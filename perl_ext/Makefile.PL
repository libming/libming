use ExtUtils::MakeMaker;

use Cwd qw(abs_path cwd);

my @DEFAULT_DIR = qw(../.. ./src ../ ../ming-0.0.9 ../ming-0.0.9b ./ming-0.0.9 ./ming-0.0.9b);

my $is_soheil = (defined($ENV{USER}) && ($ENV{USER} eq "soheil"));
#$Verbose=1 if $is_soheil;
my @supported_ming_vers = qw(0.0.9 0.0.9b);
my ($MING_DIR, $ming_version);



print <<EOF;

You need ming source code to install this software. If you haven't installed
ming before, please download it from http://www.opaque.net (Please read the
README file).

I'll try to find Ming src directory. If I fail, I'll ask you for the Ming 
directory.

Searching for ming src code...
EOF


foreach my $src_dir (@DEFAULT_DIR) {
    next unless -d $src_dir;
    my $abs_src_dir = abs_path($src_dir);
    print "    Trying $abs_src_dir ... ";
    $ming_version = get_ming_version($src_dir);

    if( $ming_version && version_supported($ming_version) ){
	$MING_DIR = $src_dir;
        print "Yes\n";
	last;
    }
    print "No\n";
}

ask_ming_dir() unless ($MING_DIR);

print "Will use ming version $ming_version in ". abs_path($MING_DIR) ."\n";
compile() unless ming_is_compiled();
write_swf_h($MING_DIR);
write_config_test ($MING_DIR);

WriteMakefile(
    'NAME'	   => 'SWF',
    'VERSION_FROM' => 'SWF.pm', # finds $VERSION
    'LIBS'	   => [''],   # e.g., '-lm' 
    'DEFINE'	   => '',     # e.g., '-DHAVE_SOMETHING' 
    'INC'	   => '',     # e.g., '-I/usr/include/other'
    'MYEXTLIB'     => $MING_DIR . '/libming.a',
);

sub version_supported{
    my $ver = shift;
    foreach (@supported_ming_vers){
	return 1 if $ver eq $_;
    }
    return 0;
}

sub ming_is_compiled{
    return undef unless (-e "$MING_DIR/libming.a");
    return 1;
}

sub compile{
    print "Compiling ming ...\n";
    my $cur_dir = cwd;
    chdir $MING_DIR;
    system "make";
    chdir $cur_dir;
}

sub ask_ming_dir{
    while(1) {
       print "Please tell me where I can find your Ming src (type q to quit): " ; 
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

    my $file = "$d/src/$filename";
    #print "Checking $file\n";

    return undef unless (-e $file);

    #print "File exists\n";

    local *FH;
    open FH, "$file" or die "can't open $file $!";

    my $version;
    while(<FH>) {
	next unless /^#define/;
	next unless s/^#define\s+MING_VERSION\s+(.*)/$1/;
	chomp($version=$_);
	print "MING VERSION = $version\n" if $Is_soheil;
        return $version;
    }
    close(FH);
    return undef;
}



sub write_swf_h {
    my $dir = shift;
    unlink "SWH.h";
    local *FH;
    open FH, ">SWF.h" or die "can't open SWF.h $!";
    print FH <<EOF;
/* ====================================================================
 * Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 */

#include "$dir/src/ming.h"
#include "Exports.c"
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
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
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

}

EOF
    close FH;
}
    




