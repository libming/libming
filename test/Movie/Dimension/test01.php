#!/usr/bin/php -c.
<?php
$m = new SWFMovie();

$m->setDimension(640.000000, 480.000000);

/* SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test01.swf");
?>
