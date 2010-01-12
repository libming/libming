#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie();

$f = new SWFFont($mediadir . "/test.ttf");

$tf = new SWFTextField();
$tf->setFont($f);
$tf->setColor(0x0, 0x00, 0x00, 0xff);
$tf->setHeight(20);

$m->add($tf);
$m->nextFrame(); /* end of frame 1 */
$m->save("test02.swf");
?>
