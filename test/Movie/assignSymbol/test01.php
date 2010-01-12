#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../../Media";

$m = new SWFMovie(9);

ming_setscale(1.0);

$f2 = new SWFFont($mediadir . "/font01.fdb");

/* SWF_DEFINETEXT */
$character1 = new SWFText(1);
$character1->setFont($f2);
$character1->setHeight(200);
$character1->setColor(0x00, 0x00, 0x00);
$character1->moveTo(200, 2000);
$character1->addString("The quick brown fox jumps over the lazy dog. 1234567890");

/* SWF_PLACEOBJECT2 */
$i1 = $m->add($character1);
$i1->setDepth(1);
/* PlaceFlagHasMatrix */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_SYMBOLCLASS */
$m->assignSymbol($character1, "mytext");

/* SWF_END */

$m->save("test01.swf");
?>
