BEGIN { $| = 1; print "1..2\n"; }

use SWF qw(Movie);
require 't/config.pl';


my $m;
eval {
    $m = new SWF::Movie();
};

ok($@);


$m->setDimension(640, 480);
ok();
