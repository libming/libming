#!/usr/bin/perl -w

use SWF qw(:ALL);

use Fcntl 'SEEK_SET';

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::VideoStream($mediadir."/video01.flv");
$stream->setDimension(200, 200);
$stream->setFrameMode(SWF::Constants::SWFVIDEOSTREAM_MODE_MANUAL);

$m->add($stream);

for ($i=0; $i<50; $i++)
{
	if ($i == 30) { $off = $stream->seek(50, SEEK_SET); }
	if ($i == 40) { $stream->seek($off, SEEK_SET); }
	$stream->nextFrame();
	$m->nextFrame();
}

$m->save("test06.swf");
