#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$sound = new SWF::SoundStream($mediadir."/audio01.mp3");

$m->setSoundStream($sound);
for($i = 0; $i < 200; $i++) { 
	$m->nextFrame();
}

$m->save("test01.swf");
