#!/usr/bin/php -c.
<?php
$m = new SWFMovie(8);

ming_setscale(1.0);
/*Unknown block type 69*/

/* SWF_SCRIPTLIMITS */
$m->setScriptLimits(10, 11);

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_END */

$m->save("test01.swf");
?>
