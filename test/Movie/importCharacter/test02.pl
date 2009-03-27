#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = SWF::Movie::newSWFMovieWithVersion(8);

$character1 = $m->importCharacter("test.swf", "test");

$i1 = $m->add($character1);
$i1->setDepth(1);

$m->nextFrame(); # end of frame 1 

$m->save("test02.swf");
