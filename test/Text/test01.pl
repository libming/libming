#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = new SWF::Movie();

$f = new SWF::Font($mediadir."/font01.fdb");

$t = new SWF::Text(2);
$t->setFont($f);
$t->setHeight(20);
$t->setColor(0x00, 0x00, 0x00, 0xff);
$t->addString("abc");

# SWF_PLACEOBJECT2 
$i1 = $m->add($t);
$i1->setDepth(1);
# PlaceFlagHasMatrix 

# SWF_SHOWFRAME 
$m->nextFrame(); # end of frame 1 

# SWF_END 

$m->save("test01.swf");
