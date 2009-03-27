#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$sound = new SWF::SoundStream($mediadir."/video02.flv");

$m->setSoundStream($sound);
for($i = 0; $i < 200; $i++) {
	$m->nextFrame();
}

$m->save("test02.swf");
