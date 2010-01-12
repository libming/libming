#!/usr/bin/php -c.

<? 

$srcdir = $argv[1];

$m = new SWFMovie(7);
$sound = new SWFSoundStream($srcdir . "/../../Media/nellymoser.flv");

$m->setSoundStream($sound);

for($i = 0; $i < 200; $i++)
	$m->nextFrame();

$m->save("test03.swf");

?>
