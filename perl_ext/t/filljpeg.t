BEGIN { $| = 1; print "1..2\n"; }

use SWF qw(Shape Bitmap);
require 't/config.pl';

my $s = new SWF::Shape();

eval{
    my $b = new SWF::Bitmap(ming_dir() . '/examples/common/backyard.jpg');
    my $f = $s->addFill($b);
    $s->setRightFill($f);
};

ok($@);

$s->drawLine(640, 0);
$s->drawLine(0, 480);
$s->drawLine(-640, 0);
$s->drawLine(0, -480);

$m = new SWF::Movie();
$m->setDimension(640, 480);
$m->add($s);

ok();

