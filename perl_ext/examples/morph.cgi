#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;

use SWF qw(Movie Shape Morph);
SWF::setScale(20);

my $p = new SWF::Morph();

my $s = $p->getShape1();
$s->setLine(18,0,0xFF,0);
$s->setLeftFill(0xff, 0, 0);
$s->drawLine(100,0);
$s->drawLine(0,100);
$s->drawLine(-100,0);
$s->drawLineTo(0,0);

my $s1 = $p->getShape2();
$s1->setLine(10,0,0,0);
$s1->setLeftFill(0, 0, 0x99);
$s1->drawLine(100,50);
$s1->drawLine(0,50);
$s1->drawLine(-150,0);
$s1->drawLineTo(0,0);

my $m = new SWF::Movie();
$m->setDimension(300,200);
$m->setBackground(0xff, 0xff, 0x0);

my $i = $m->add($p);
$i->moveTo(100,50);

for(my $r=0; $r<=10; ++$r){
    $i->setRatio($r/10);
    $m->nextFrame();
}
for(my $r=0; $r<=10; ++$r){
    $i->setRatio(1-$r/10);
    $m->nextFrame();
}
$m->nextFrame();

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
