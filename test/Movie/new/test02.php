#!/usr/bin/php
<?php
$m = new SWFMovie(4);


/*  SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);
$m->nextFrame(); /*  end of frame 1 */

$m->save("test02.swf");
?>
