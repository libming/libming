#!/usr/bin/perl
use strict;

use SWF::Shape ();
use SWF::Sprite();

SWF::setScale(1.0);
print "Content-type: application/x-shockwave-flash\n\n";

my $s = new SWF::Shape();
$s->setRightFill($s->addFill(0xff, 0, 0));
$s->movePenTo(-500,-500);
$s->drawLineTo(500,-500);
$s->drawLineTo(500,500);
$s->drawLineTo(-500,500);
$s->drawLineTo(-500,-500);

#my  $p = new SWF::MovieClip();
my  $p = new SWF::Sprite();
my $i = $p->add($s);

for(my $j=0; $j<17; ++$j)
{
    $p->nextFrame();
    $i->rotate(5);
}
$p->nextFrame();

my $m = new SWF::Movie();
$i = $m->add($p);
$i->moveTo(1500,1000);
$i->setName("blah");

$m->setBackground(0xff, 0xff, 0xff);
$m->setDimension(3000,2000);
$m->output();
