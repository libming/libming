BEGIN { $| = 1; print "1..1\n";}
END{ ok(!$loaded); }
require 't/config.pl';

$loaded = 0;
use SWF;
$loaded = 1;
