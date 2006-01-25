#!/usr/bin/perl -w
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(Movie Font Text);
SWF::setScale(1.0);

# Add path to your *.fdb file
my $filename = '../common/_sans.fdb';

my $f= new SWF::Font($filename);

my $t= new SWF::Text();
$t->setFont($f);
$t->moveTo(200, 2400);
$t->setColor(0xff, 0x0, 0);
$t->setHeight(1200);
$t->addString("ming!");

my $m = new SWF::Movie();
$m->setDimension(5400, 3600);

$m->add($t);

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}


