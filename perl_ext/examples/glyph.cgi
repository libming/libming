#!/usr/bin/perl
use strict;

use SWF (':ALL');
SWF::setScale(1.0);

########## ADD path to your test.fdb file #######################
my $filename = '/PATH/TO/test.fdb';

print "Content-type: application/x-shockwave-flash\n\n";


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
$m->output();
