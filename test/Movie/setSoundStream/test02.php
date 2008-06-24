#!/usr/bin/php -c.

<? 

$m = new SWFMovie(7);
$sound = new SWFSoundStream("../../Media/video02.flv");

$m->setSoundStream($sound);

for($i = 0; $i < 200; $i++)
	$m->nextFrame();

$m->save("test02.swf");

?>
