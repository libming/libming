#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie();

ming_setscale(1.0);

/* SWF_DEFINEFONT2 */
$f2 = new SWFFont($mediadir . "/test.ttf");
/* init font 2 code table*/

/* SWF_DEFINETEXT */
$character1 = new SWFText(1);
$character1->setFont($f2);
$character1->setHeight(400);
$character1->setColor(0x00, 0x00, 0x00);
$character1->moveTo(2000, 2000);
$character1->addString("The quick brown fox jumps over the lazy dog. 1234567890");

/* SWF_PLACEOBJECT2 */
$i1 = $m->add($character1);
$i1->setDepth(1);
/* PlaceFlagHasMatrix */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test02.swf");
?>
