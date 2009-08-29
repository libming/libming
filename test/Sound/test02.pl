#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream = new SWF::SoundStream($mediadir."/video02.flv");

$sound = new SWF::Sound($stream);

$m->addExport($sound,'video02.flv');

$m->startSound($sound);

$m->save("test02.swf");
