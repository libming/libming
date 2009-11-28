#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../../Media";

$m = SWF::Movie::newSWFMovieWithVersion(9);

SWF::setScale(1.0);

$f2 = new SWF::Font($mediadir."/font01.fdb");

$character1 = new SWF::Text();
$character1->setFont($f2);
$character1->setHeight(200);
$character1->setColor(0x00, 0x00, 0x00);
$character1->moveTo(200, 2000);
$character1->addString("The quick brown fox jumps over the lazy dog. 1234567890");

$i1 = $m->add($character1);
$i1->setDepth(1);
# PlaceFlagHasMatrix 

$m->nextFrame(); # end of frame 1 

$m->assignSymbol($character1, "mytext");

# SWF_END 

$m->save("test01.swf");
