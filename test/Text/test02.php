#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie();

ming_setscale(1.0);

$f2 = new SWFFont($mediadir . "/font01.fdb");
$f3 = new SWFFont($mediadir . "/test.ttf");

/* SWF_DEFINETEXT2 */
$character1 = new SWFText(2);
$character1->setFont($f2);
$character1->setHeight(400);
$character1->setColor(0x00, 0x00, 0x00, 0xff);
$character1->addString("abc");
$character1->setFont($f3);
$character1->setHeight(800);
$character1->setColor(0xff, 0x00, 0x00, 0xff);
$character1->addString("def");

$i1 = $m->add($character1);
$i1->setDepth(1);

$m->nextFrame(); /* end of frame 1 */

$m->save("test02.swf");
?>
