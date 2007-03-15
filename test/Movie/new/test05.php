#!/usr/bin/php -c.
<?php
$m = new SWFMovie(7);


/* SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test05.swf");
?>
