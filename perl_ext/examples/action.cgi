#!/usr/bin/perl -wT
use strict;

use SWF qw(:ALL);
SWF::setScale(1.0);
SWF::setVersion(4);

print "Content-type: application/x-shockwave-flash\n\n";
my $s = new SWF::Shape();
my $f = $s->addFill(0xff, 0, 0);
$s->setRightFill($f);

$s->movePenTo(-500,-500);
$s->drawLineTo(500,-500);
$s->drawLineTo(500,500);
$s->drawLineTo(-500,500);
$s->drawLineTo(-500,-500);

my $p = new SWF::MovieClip();
my $i = $p->add($s);
$i->setDepth(1);
$p->nextFrame();

for(my $n=0; $n<5; ++$n){
    $i->rotate(-15);
    $p->nextFrame();
}

my $m = new SWF::Movie();
$m->setBackground(0xff, 0xff, 0xff);
$m->setDimension(6000,4000);

$i = $m->add($p);
$i->setDepth(1);
$i->moveTo(-500,2000);
$i->setName("box");

$m->add(new SWF::Action("x=-100;"));
$m->nextFrame();
$m->add(new SWF::Action("x+=3; /box._x=x;"));
$m->nextFrame();
$m->add(new SWF::Action("gotoFrame(1); play();"));
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
