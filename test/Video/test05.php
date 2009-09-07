#!/usr/bin/php -c.
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);

$stream = new SWFVideoStream($srcdir . "/../Media/video01.flv");
$numFrames = $stream->getNumFrames();

$mc = new SWFMovieClip();
$mc->add($stream);
for($i = 0; $i < $numFrames; $i++)
{
	$mc->nextFrame();
}

$m->add($mc);
$m->save("test05.swf");

?>
