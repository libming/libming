#!/usr/bin/php -c.
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);

$stream = new SWFVideoStream($srcdir . "/../Media/video01.flv");
$stream->setDimension(200, 200);
$stream->setFrameMode(SWF_VIDEOSTREAM_MODE_MANUAL);

$m->add($stream);
$numFrames = $stream->getNumFrames();
for($i = 0; $i < $numFrames; $i++)
{
	if ($i % 2) $stream->nextFrame();
	$m->nextFrame();
}

$m->save("test04.swf");

?>
