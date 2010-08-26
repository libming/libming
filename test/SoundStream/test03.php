#!/usr/bin/php -c.

<? 

$srcdir = $argv[1];

$m = new SWFMovie(7);

$stream = new SWFSoundStream($srcdir . "/../Media/audio01.mp3");
$stream->setInitialMp3Delay(16384);

$mc = new SWFMovieClip();
$mc->setSoundStream($stream, $m->getRate());
for($i =  0; $i < 200; $i++)
	$mc->nextFrame();

$m->add($mc);
$m->nextFrame();

$m->save("test03.swf");

?>
