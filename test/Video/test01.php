#!/usr/bin/php -c.
<?php
$m = new SWFMovie(7);

$video = new SWFVideoStream("../Media/video01.flv");
$video->setDimension(200, 200);
$m->add($video);
$frames = $video->getNumFrames();
for($i = 0; $i < $frames; $i++)
	$m->nextFrame();
$m->save("test01.swf");
?>
