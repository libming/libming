#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;

use SWF qw(:ALL);
SWF::setScale(1.0);

# Add path to your *.fdb file
my $filename = '../common/_sans.fdb';


my $s = new SWF::Shape();
my $f = new SWF::Font($filename);

$s->setRightFill(0xff, 0, 0);
$s->setLine(40, 0x7f, 0, 0);
$s->drawGlyph($f, unpack('C','!'));
$s->movePen($f->getWidth('!'), 0);

$s->setRightFill(0xff, 0x7f, 0);
$s->setLine(40, 0x7f, 0x3f, 0);
$s->drawGlyph($f, unpack('C','#'));
$s->movePen($f->getWidth('#'), 0);

$s->setRightFill(0xff, 0xff, 0);
$s->setLine(40, 0x7f, 0x7f, 0);
$s->drawGlyph($f, unpack('C','%'));
$s->movePen($f->getWidth('%'), 0);

$s->setRightFill(0, 0xff, 0);
$s->setLine(40, 0, 0x7f, 0);
$s->drawGlyph($f, unpack('C','*'));
$s->movePen($f->getWidth('*'), 0);

$s->setRightFill(0, 0, 0xff);
$s->setLine(40, 0, 0, 0x7f);
$s->drawGlyph($f, unpack('C','@'));

my $m = new SWF::Movie();
$m->setDimension(3000,2000);
$m->setRate(12.0);
my $i = $m->add($s);
$i->moveTo(1500-$f->getWidth("!#%*@")/2, 1000+$f->getAscent()/2);

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
