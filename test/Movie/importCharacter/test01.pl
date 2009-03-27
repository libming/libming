#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = new SWF::Movie();

$character1 = $m->importCharacter("test.swf", "test");

$i1 = $m->add($character1);
$i1->setDepth(1);
# PlaceFlagHasMatrix 

# SWF_SHOWFRAME 
$m->nextFrame(); # end of frame 1 

# SWF_END 

$m->save("test01.swf");
