#!/usr/bin/php -c.
<?php
$m = new SWFMovie(7);

ming_setscale(1.0);

/* SWF_SETTABINDEX */
$m->setTabIndex(1, 2);

/* SWF_SETTABINDEX */
$m->setTabIndex(2, 2);

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test01.swf");
?>
