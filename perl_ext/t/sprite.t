BEGIN {$| = 1; print "1..3\n"; }

use SWF qw(Sprite);

require 't/config.pl';


my $p;

eval{
    $p = new SWF::Sprite();
};
print "$@\n";
ok($@);

$p->setFrames(5);

ok();

$p->labelFrame('soheil');

ok();
