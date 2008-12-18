#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::VideoStream($mediadir."/video01.flv");
$stream->setDimension(200, 200);
$stream->setFrameMode(SWF::Constants::SWFVIDEOSTREAM_MODE_MANUAL);

$m->add($stream);

$numFrames = $stream->getNumFrames();
for ($i=0; $i<$numFrames; $i++)
{
	if ($i % 2) { $stream->nextFrame(); }
	$m->nextFrame();
}

$m->save("test04.swf");
