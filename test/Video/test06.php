#!/usr/bin/php -c.
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);

$stream = new SWFVideoStream($srcdir . "/../Media/video01.flv");

$stream->setFrameMode(SWF_VIDEOSTREAM_MODE_MANUAL);
$stream->setDimension(200, 200);

$m->add($stream);

$off = 0;
for($i = 0; $i < 50; $i++)
{
	if ( $i == 30 )
		$off = $stream->seek(50, SEEK_SET);

	if ( $i == 40 )
		$stream->seek($off, SEEK_SET);
	
	$stream->nextFrame();
	$m->nextFrame();
}

$m->save("test06.swf");

?>
