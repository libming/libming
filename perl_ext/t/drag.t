BEGIN {$| = 1; print "1..4\n"; }

use SWF qw(Button Shape);

require 't/config.pl';

#$Test::Harness::switches = '';

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

$b->addShape($s, SWF::Button::SWFBUTTON_HIT | SWF::Button::SWFBUTTON_UP | SWF::Button::SWFBUTTON_DOWN | SWF::Button::SWFBUTTON_OVER);
$b->addAction(new SWF::Action("startDrag('/test', 0);"), SWF::Button::SWFBUTTON_MOUSEDOWN);
$b->addAction(new SWF::Action("stopDrag();"), SWF::Button::SWFBUTTON_MOUSEUP);

ok();

my $p;
eval{
    $p = new SWF::MovieClip();
};

ok();

$p->add($b);
$p->nextFrame();

$m = new SWF::Movie();
$i = $m->add($p);
$i->setName('test');
$i->moveTo(1000,1000);
ok();
