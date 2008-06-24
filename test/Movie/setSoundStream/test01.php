#!/usr/bin/php -c.

<? 

$m = new SWFMovie(7);
$sound = new SWFSoundStream("../../Media/audio01.mp3");

$m->setSoundStream($sound);

for($i = 0; $i < 200; $i++)
	$m->nextFrame();

$m->save("test01.swf");

?>
