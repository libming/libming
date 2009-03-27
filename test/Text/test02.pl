#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = new SWF::Movie();

$f2 = new SWF::Font($mediadir."/font01.fdb");
$f3 = new SWF::Font($mediadir."/test.ttf");

# SWF_DEFINETEXT2 
$t = new SWF::Text(2);
$t->setFont($f2);
$t->setHeight(20);
$t->setColor(0x00, 0x00, 0x00, 0xff);
$t->addString("abc");
$t->setFont($f3);
$t->setHeight(40);
$t->setColor(0xff, 0x00, 0x00, 0xff);
$t->addString("def");

# SWF_PLACEOBJECT2 
$i1 = $m->add($t);
$i1->setDepth(1);
# PlaceFlagHasMatrix 

# SWF_SHOWFRAME 
$m->nextFrame(); # end of frame 1 

# SWF_END 

$m->save("test02.swf");
