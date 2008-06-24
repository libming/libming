#!/usr/bin/php -c.

<? 

$m = new SWFMovie(7);
$m->streamMP3("../../Media/audio01.mp3");
for($i = 0; $i < 200; $i++)
	$m->nextFrame();

$m->save("test01.swf");

?>
