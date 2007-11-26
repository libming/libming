#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = new SWF::Movie();
$t = new SWF::Text();

$f = new SWF::Font("../Media/test.ttf");
$t->setFont($f);
$t->setColor(0,0,0,0xff);
$t->setHeight(20);
$t->moveTo(100,100);
$t->addString("The quick brown fox jumps over the lazy dog. 1234567890");

$m->add($t);
$m->nextFrame();
$m->save("test02.swf");

