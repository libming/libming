BEGIN { $| = 1;}

use SWF qw(:ALL);
#use SWF;
require 't/config.pl';

my $filename = ming_dir() . '/examples/common/distortobass.mp3';
skip_test() unless (-e $filename);
print "1..3\n";
my $movie = new SWF::Movie();
$movie->setRate(12.0);
my $sound = new SWF::Sound($filename);
ok();

eval{
    $movie->setSoundStream($sound);
};

ok($@);

my $movie2 = new SWF::Movie();
$movie2->setRate(12.0);
$movie2->streamMp3(ming_dir() . '/examples/common/distortobass.mp3');
ok();
