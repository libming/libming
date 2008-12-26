#!/usr/bin/perl -w
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(Movie Font TextField);
use SWF::Constants qw(SWFTEXTFIELD_DRAWBOX);
# Add path to your *.fdb file
my $filename = '../common/_sans.fdb';

my $f= new SWF::Font($filename);

my $tf= new SWF::TextField(SWFTEXTFIELD_DRAWBOX);
$tf->setFont($f);
$tf->setColor(0xff, 0x0, 0);
$tf->setHeight(20);
$tf->addString("ming!");

my $m = new SWF::Movie();
$m->setDimension(300, 200);
$m->setBackground(0xff, 0xff, 0x0);

my $tfi=$m->add($tf);
$tfi->moveTo(100,100);
$m->nextFrame;

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}


