BEGIN {$| = 1; print "1..4\n"; }

use strict;

use SWF qw(Button Shape Movie);
use SWF::Constants ':Button';
require 't/config.pl';

#$Test::Harness::switches = '';

SWF::setVersion(4);
my $s = new SWF::Shape();
$s->setRightFill(0xff, 0, 0);
$s->drawLine(1000,0);
$s->drawLine(0,1000);
$s->drawLine(-1000,0);
$s->drawLine(0,-1000);

my $b;
eval{
    $b = new SWF::Button();
};
ok($@);

$b->addShape($s, SWFBUTTON_HIT | SWFBUTTON_UP | SWFBUTTON_DOWN | SWFBUTTON_OVER);
$b->addAction(new SWF::Action("startDrag('/test', 0);"), SWFBUTTON_MOUSEDOWN);
$b->addAction(new SWF::Action("stopDrag();"), SWFBUTTON_MOUSEUP);

ok();

my $p;
eval{
    $p = new SWF::MovieClip();
};

ok();

$p->add($b);
$p->nextFrame();

my $m = new SWF::Movie();
my $i = $m->add($p);
$i->setName('test');
$i->moveTo(1000,1000);
ok();
