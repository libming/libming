#!/usr/bin/perl
use strict;

use SWF qw(:ALL);
SWF::setScale(1.0);

print "Content-type: application/x-shockwave-flash\n\n";

my $p = new SWF::Morph();

my $s = $p->getShape1();
$s->setLine();
$s->setLeftFill(0xff, 0, 0);
$s->movePenTo(-1000,-1000);
$s->drawLine(2000,0);
$s->drawLine(0,2000);
$s->drawLine(-2000,0);
$s->drawLine(0,-2000);

my $s1 = $p->getShape2();
$s1->setLine(60,0,0,0);
$s1->setLeftFill(0, 0, 0xff);
$s1->movePenTo(0,-1000);
$s1->drawLine(1000,1000);
$s1->drawLine(-1000,1000);
$s1->drawLine(-1000,-1000);
$s1->drawLine(1000,-1000);

my $m = new SWF::Movie();
$m->setDimension(3000,2000);
$m->setBackground(0xff, 0xff, 0xff);

my $i = $m->add($p);
$i->moveTo(1500,1000);

for(my $r=0; $r<=10; ++$r){
    $i->setRatio($r/10);
    $m->nextFrame();
}

$m->output();
