#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::VideoStream($mediadir."/video01.flv");

$m->add($stream);

$numFrames = $stream->getNumFrames();
for ($i=0; $i<$numFrames; $i++)
{
	$m->nextFrame(); 
}

$m->save("test01.swf");
