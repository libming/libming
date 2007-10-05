BEGIN {$| = 1; print "1..3\n"; }

use SWF qw(MovieClip);

require 't/config.pl';


my $p;

eval{
    $p = new SWF::MovieClip();
};
print "$@\n";
ok($@);

$p->setFrames(5);

ok();

$p->labelFrame('soheil');

ok();
