#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::SoundStream($mediadir."/audio01.mp3");

$sound = new SWF::Sound($stream);

$m->addExport($sound,'audio01.mp3');

$m->startSound($sound);

$m->save("test01.swf");
