BEGIN {$| = 1; print "1..2\n"; }

use SWF qw(Button Action);
use SWF::Constants ':Button';

require 't/config.pl';

my $b;

eval{
    $b = new SWF::Button();
};
print "$@\n";
ok($@);

eval{
    $b->addAction(new SWF::Action("setTarget('/label'); gotoFrame(1);"),
		  SWFBUTTON_MOUSEUP);
};
print "$@\n";
ok($@);
