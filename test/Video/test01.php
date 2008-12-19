#!/usr/bin/php -c.
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);

$video = new SWFVideoStream($srcdir . "/../Media/video01.flv");
$video->setDimension(200, 200);
$m->add($video);
$frames = $video->getNumFrames();
for($i = 0; $i < $frames; $i++)
	$m->nextFrame();
$m->save("test01.swf");
?>
