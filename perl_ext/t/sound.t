BEGIN { $| = 1; print "1..3\n"; }

use SWF qw(:ALL);
#use SWF;
require 't/config.pl';

my $movie = new SWF::Movie();
$movie->setRate(12.0);
my $sound = new SWF::Sound(ming_dir() . '/examples/common/distortobass.mp3');
ok();

eval{
    $movie->setSoundStream($sound);
};

ok($@);

my $movie2 = new SWF::Movie();
$movie2->setRate(12.0);
$movie2->streamMp3(ming_dir() . '/examples/common/distortobass.mp3');
ok();