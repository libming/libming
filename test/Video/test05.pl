#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::VideoStream($mediadir."/video01.flv");
$numFrames = $stream->getNumFrames();

$mc = new SWF::MovieClip();
$mc->add($stream);

for ($i=0; $i<$numFrames; $i++)
{
	$mc->nextFrame(); 
}

$m->add($mc);
$m->nextFrame();

$m->save("test05.swf");

