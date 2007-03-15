#!/usr/bin/php -c.
<?php
$m = new SWFMovie();

$m->setFrames(2);

/* SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

/* SWF_DOACTION */
$m->add(new SWFAction("var a = 1;
") );

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 2 */

/* SWF_END */

$m->save("test02.swf");
?>
