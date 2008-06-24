#!/usr/bin/php -c.

<? 

$m = new SWFMovie(7);

$mc = new SWFMovieClip();
$mc->setSoundStream("../Media/audio01.mp3", $m->getRate());
for($i =  0; $i < 200; $i++)
	$mc->nextFrame();

$m->add($mc);
$m->nextFrame();

$m->save("test02.swf");

?>
