BEGIN { $| = 1; print "1..4\n"; }

use SWF qw(Shape Movie Gradient);
require 't/config.pl';

my  $m = new SWF::Movie();
$m->setDimension(320, 240);
my $s = new SWF::Shape();


# first gradient- black to white
my $g;
eval{
    $g = new SWF::Gradient();
};

ok($@);

$g->addEntry(0.0, 0, 0, 0);
$g->addEntry(1.0, 0xff, 0xff, 0xff);

my $f;
eval{
    $f = $s->addFill($g);
};

ok($@);

$f->scaleTo(0.01);

$f->moveTo(160, 120);
$s->setRightFill($f);
$s->drawLine(320, 0);
$s->drawLine(0, 240);
$s->drawLine(-320, 0);
$s->drawLine(0, -240);

$m->add($s);

ok();

eval{
    $m->save('t/gradient.swf');
};

ok($@);
