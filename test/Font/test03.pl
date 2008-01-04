#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = new SWF::Movie();
$t = new SWF::Text();

$f = new SWF::Font("../Media/test.ttf");
$w = $f->getStringWidth("The quick brown fox jumps over the lazy dog. 1234567890");
$t->setFont($f);
$t->setColor(0,0,0,0xff);
$t->setHeight(20);
$t->moveTo($w, 0);
$t->addString("|");

$m->add($t);
$m->nextFrame();
$m->save("test03.swf");

