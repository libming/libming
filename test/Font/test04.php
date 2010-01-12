#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie();

$f = new SWFFont($mediadir . "/font01.fdb");

$t = new SWFText(1);
$t->setFont($f);
$t->setHeight(20);
$t->setColor(0x00, 0x00, 0xff);
$t->moveTo(100, 100);
$t->addString("1234567890");

$tf = new SWFTextField();
$tf->setFont($f);
$tf->setHeight(20);
$tf->setColor(0xff, 0x00, 0x00);
$tf->addString("1234567890");

$m->add($t);
$it = $m->add($tf);
$it->moveTo(100, 120);

$m->nextFrame(); /* end of frame 1 */

$m->save("test04.swf");
?>
