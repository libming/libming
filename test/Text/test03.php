#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie();

$f2 = new SWFFont($mediadir . "/font01.fdb");
$f3 = new SWFFont($mediadir . "/test.ttf");

$t = new SWFText(2);
$t->setFont($f2);
$t->setHeight(20);
$t->setColor(0x00, 0x00, 0x00, 0xff);
$t->moveTo(10, 20); 
$t->addString("abc");
$t->moveTo(0, 40); 
$t->addString("bca");
$t->moveTo(60, 0); 
$t->addString("cab");

$m->add($t);
$m->nextFrame(); 

$m->save("test03.swf");
?>
