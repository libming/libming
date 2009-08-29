#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = SWF::Movie::newSWFMovieWithVersion(7);

$sound = new SWF::Sound($mediadir."/sound1.mp3",
	SWF::Constants::SWF_SOUND_MP3_COMPRESSED);

$m->addExport($sound,'sound1.mp3');
$m->startSound($sound);

$sound2 = new SWF::Sound($mediadir."/sound1.mp3",
	SWF::Constants::SWF_SOUND_MP3_COMPRESSED);

$sound2->setInitialMp3Delay(2048);

$m->addExport($sound2,'delaySeeked');


$m->save("test03.swf");
