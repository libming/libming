#!/usr/bin/php -c.

<? 

$srcdir = $argv[1];

$m = new SWFMovie(7);
$sound = new SWFSoundStream($srcdir . "/../../Media/mpeg1.mp3");

$m->setSoundStream($sound);

for($i = 0; $i < 12; $i++)
	$m->nextFrame();

$m->save("test04.swf");

?>
