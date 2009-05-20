#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie(8);

ming_setscale(1.0);
/*Unknown block type 69*/

$character1 = new SWFBitmap($mediadir . '/image01.png');

/* SWF_DEFINESHAPE3 */

/* Shape 2 (TYPE=3, RECT=0,400 0,400)*/
$character2 = new SWFShape();
/*1 fillstyle(s)*/
/* BitmapID: 1 */
$character2_f0 = $character2->addFill($character1,SWFFILL_TILED_BITMAP);
$character2_f0->scaleTo(20.000000);
/*0 linestyles(s)*/
$character2->setRightFill($character2_f0);
$character2->drawLine(400, 0);
$character2->drawLine(0, 400);
$character2->drawLine(-400, 0);
$character2->drawLine(0, -400);

/* SWF_PLACEOBJECT2 */
$i1 = $m->add($character2);
$i1->setDepth(1);
/* PlaceFlagHasMatrix */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test01.swf");
?>
