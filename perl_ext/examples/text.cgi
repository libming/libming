#!/usr/bin/perl
use strict;

use SWF ('Font');
SWF::setScale(1.0);

########## ADD path to your test.fdb file #######################
my $filename = '/PATH/TO/test.fdb';

print "Content-type: application/x-shockwave-flash\n\n";

my $f= new SWF::Font($filename);

my $t= new SWF::Text();
$t->setFont($f);
$t->moveTo(200, 2400);
$t->setColor(0xff, 0xff, 0);
$t->setHeight(1200);
$t->addString("fnar! fnar!");

my $m = new SWF::Movie();
$m->setDimension(5400, 3600);

$m->add($t);
$m->output();


