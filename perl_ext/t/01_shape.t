BEGIN { $| = 1; print "1..3\n"; }

use SWF qw(Shape);
require 't/config.pl';

my $s;
eval {
    $s = new SWF::Shape();
};

ok($@);

$s->setLine(20, 0xff, 0, 0, 0);
$s->drawLine(400,0);
$s->drawLine(0,400);
$s->drawLine(-400,0);
#$s->drawLine(0,-400);

ok();

$s->setLeftFill($s->addFill(0xff, 0, 0));

ok();


#ok($y!=400);
