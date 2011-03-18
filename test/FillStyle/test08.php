#!/usr/bin/php
<?php
$m = new SWFMovie(8);

$shape = new SWFShape();
$gradient = new SWFGradient();
$gradient->addEntry(0, 255, 0, 0, 255);
$gradient->addEntry(0.25,0x80,0x20,0x20,0xff);
$gradient->addEntry(0.8,0x40,0x40,0x40,0xff);
$fill = $shape->addGradientFill($gradient,SWFFILL_LINEAR_GRADIENT);
#$fill->moveTo(-163, -163); # this is added by swftoscript, but is wrong
$shape->setLine(1,0,0,0,255);
$shape->setRightFill($fill);
$shape->drawLine(100, 0);
$shape->drawLine(0, 100);
$shape->drawLine(-100, 0);
$shape->drawLine(0, -100);

$m->add($shape);

$m->save("test08.swf");
?>
