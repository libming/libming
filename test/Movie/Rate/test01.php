#!/usr/bin/php
<?php
$m = new SWFMovie();

$m->setRate(24.000000);

/*  SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);
$m->nextFrame();  # end of frame 1

$m->save("test01.swf");
?>
