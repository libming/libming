#!/usr/bin/php
<?php
$m = new SWFMovie(8);

ming_setscale(1.0);
/*Unknown block type 69*/

/* SWF_DEFINESHAPE3 */

/* Shape 1 (TYPE=3, RECT=0,0 0,0)*/
$character1 = new SWFShape();
/*2 fillstyle(s)*/
$character1_f0_red   = 0x20;
$character1_f0_green = 0x40;
$character1_f0_blue  = 0x80;
$character1_f0_alpha = 0xdf;
$character1_f0 = $character1->addSolidFill($character1_f0_red, $character1_f0_green, $character1_f0_blue, $character1_f0_alpha ); /*SWFFILL_SOLID*/
$character1_f1_red   = 0x40;
$character1_f1_green = 0x80;
$character1_f1_blue  = 0x20;
$character1_f1_alpha = 0x18;
$character1_f1 = $character1->addSolidFill($character1_f1_red, $character1_f1_green, $character1_f1_blue, $character1_f1_alpha ); /*SWFFILL_SOLID*/
/*0 linestyles(s)*/

/* SWF_PLACEOBJECT2 */
$i1 = $m->add($character1);
$i1->setDepth(1);
/* PlaceFlagHasMatrix */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test01.swf");
?>
