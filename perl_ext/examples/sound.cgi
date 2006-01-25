#!/usr/bin/perl
use strict;

#use lib("/home/peter/ming3/lib/perl5/site_perl");

$|=1;
use SWF qw(:ALL);
use SWF::Constants qw(SWF_SOUND_22KHZ SWF_SOUND_11KHZ SWF_SOUND_16BITS SWF_SOUND_8BITS SWF_SOUND_MONO);

my $soundfile="../common/beep.wav";

# some misuse of one wavfiles ;-)
my $sound = new SWF::Sound($soundfile, SWF_SOUND_22KHZ|SWF_SOUND_8BITS|SWF_SOUND_MONO);
my $sound2 = new SWF::Sound($soundfile, SWF_SOUND_11KHZ|SWF_SOUND_8BITS|SWF_SOUND_MONO);


my $m = new SWF::Movie();
$m->setDimension(200, 100);
$m->setBackground(0xff, 0xff, 0x0);
$m->setRate(12.0);


# now let's do some funny noisepattern...
my $soundinstance=$m->startSound($sound);
$soundinstance->loopCount(20);
$soundinstance->loopOutPoint(20000);
$soundinstance->noMultiple();
$m->nextFrame();


for(0..5){
	$m->nextFrame();
}
my $soundinstance2=$m->startSound($sound2);
$soundinstance2->loopCount(20);
$soundinstance2->loopInPoint(0);
$soundinstance2->loopOutPoint(4000);
$soundinstance2->noMultiple();

for(0..5){
	$m->nextFrame();
}

# decide if its called from commandline or as cgiscript
if (exists $ENV{"REQUEST_URI"}){
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output();
}
else {
	$m->save("$0.swf");
	print "Generated file written to $0.swf\n";
}
