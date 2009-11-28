#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = new SWF::Movie();
$t = new SWF::Text();

$f = new SWF::Font($mediadir."/font01.fdb");
$t->setFont($f);
$t->setColor(0,0,0,0xff);
$t->setHeight(10);
$t->moveTo(10, 100);
$t->addString("The quick brown fox jumps over the lazy dog. 1234567890");

$m->add($t);
$m->nextFrame();
$m->save("test01.swf");

