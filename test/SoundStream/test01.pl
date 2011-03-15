#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

SWF::setScale(1.0);
$m->setRate(1.000000);

$stream = new SWF::SoundStream($mediadir.'/sound1.mp3');

$mc = new SWF::MovieClip(); # 2 frames 
$mc->setSoundStream($stream, $m->getRate());

$mc->nextFrame(); # end of clip frame 1 
$mc->nextFrame(); # end of clip frame 2 

$m->add($mc);
$m->nextFrame(); # end of frame 1 

$m->save("test01.swf");
