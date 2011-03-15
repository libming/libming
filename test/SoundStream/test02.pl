#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

SWF::setScale(1.0);

$stream = new SWF::SoundStream($mediadir.'/audio01.mp3');

$mc = new SWF::MovieClip(); 
$mc->setSoundStream($stream, $m->getRate());
for ($i=0; $i<200; $i++) {
	$mc->nextFrame();
}

$m->add($mc);

$m->nextFrame(); 

$m->save("test02.swf");
