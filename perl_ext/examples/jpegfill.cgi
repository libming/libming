#!/usr/bin/perl
use strict;

use SWF qw(:ALL);
SWF::setScale(1.0);

print "Content-type: application/x-shockwave-flash\n\n";

my $s = new SWF::Shape();
my $b = new SWF::Bitmap('/home/soheil/src/ming-0.0.9/examples/common/backyard.jpg');

my $f = $s->addFill($b);
$s->setRightFill($f);

$s->drawLine(640, 0);
$s->drawLine(0, 480);
$s->drawLine(-640, 0);
$s->drawLine(0, -480);

my $m = new SWF::Movie();
$m->setDimension(640, 480);
$m->add($s);
$m->output();
