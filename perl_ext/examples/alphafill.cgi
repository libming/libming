#!/usr/bin/perl -wT
use strict;

use SWF qw(:ALL);

my $dir = '/home/soheil/src/ming-0.0.9/examples/common/';


print "Content-type: application/x-shockwave-flash\n\n";

my $s = new SWF::Shape();
my $alpha = new SWF::Bitmap($dir."alphafill.jpg", $dir."alphafill.msk");

my $f = $s->addFill($alpha);
$s->setRightFill($f);
$s->drawLine(640, 0);
$s->drawLine(0, 480);
$s->drawLine(-640, 0);
$s->drawLine(0, -480);

my $c = new SWF::Shape();
$c->setRightFill($c->addFill(0x99, 0x99, 0x99));

$c->drawLine(40, 0);
$c->drawLine(0, 40);
$c->drawLine(-40, 0);
$c->drawLine(0, -40);

my $m = new SWF::Movie();
$m->setDimension(640, 480);
$m->setBackground(0xcc, 0xcc, 0xcc);

# draw checkerboard background
my ($x, $y);
my $i;
for($y=0; $y<480; $y+=40){
    for($x=0; $x<640; $x+=80){
	$i = $m->add($c);
	$i->moveTo($x, $y);
    }
    $y+=40;
    
    for($x=40; $x<640; $x+=80){
	$i = $m->add($c);
	$i->moveTo($x, $y);
    }
}

$m->add($s);
$m->output();
