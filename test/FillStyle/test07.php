#!/usr/bin/php
<?php
$m = new SWFMovie(8);

ming_setscale(1.0);
/*Unknown block type 69*/

/* SWF_DEFINESHAPE3 */

/* Shape 1 (TYPE=3, RECT=-10,2010 -10,2010)*/
$character1 = new SWFShape();
/*1 fillstyle(s)*/
/* BitmapID: 65535 */
$character1_f0 = $character1->addBitmapFill($character65535,SWFFILL_CLIPPED_BITMAP);
$character1_f0->scaleTo(20.000000);
/*1 linestyles(s)*/
$character1_l0_width = 20;
$character1_l0_red   = 0x00;
$character1_l0_green = 0x00;
$character1_l0_blue  = 0x00;
$character1_l0_alpha = 0xff;
/* StateLineStyle: 1 */
$character1->setLine($character1_l0_width, $character1_l0_red, $character1_l0_green, $character1_l0_blue, $character1_l0_alpha);
$character1->setRightFill($character1_f0);
$character1->drawLine(2000, 0);
$character1->drawLine(0, 2000);
$character1->drawLine(-2000, 0);
$character1->drawLine(0, -2000);

/* SWF_PLACEOBJECT2 */
$i1 = $m->add($character1);
$i1->setDepth(1);
/* PlaceFlagHasMatrix */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test07.swf");
?>
