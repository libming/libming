#!/usr/bin/perl
use strict;

#use lib qw(/home/soheil/ming/SWF/blib/arch /home/soheil/ming/SWF/blib/lib);

use SWF qw(:ALL);
#use SWF qw(Shape Movie);

print "Content-type: application/x-shockwave-flash\n\n";

SWF::setScale(1.0);

my $s = new SWF::Shape();
$s->setLine(40, 0x7f, 0, 0);
$s->setRightFill($s->addFill(0xff, 0, 0));
$s->movePenTo(200, 200);
$s->drawLineTo(6200, 200);
$s->drawLineTo(6200, 4600);
$s->drawCurveTo(200, 4600, 200, 200);

my $m = new SWF::Movie();
$m->setDimension(6400, 4800);
$m->setRate(12.0);
$m->add($s);
$m->nextFrame();
$m->output();
