#!/usr/bin/php -c.
<?php
$m = new SWFMovie(9);

ming_setscale(1.0);
$m->setFrames(10);
/*Unknown block type 69*/

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 2 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 3 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 4 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 5 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 6 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 7 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 8 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 9 */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 10 */

/* SWF_DEFINESCENEANDFRAMEDATA */
$m->defineScene(0, "test0");
$m->defineScene(5, "test1");

/* SWF_END */

$m->save("test01.swf");
?>
