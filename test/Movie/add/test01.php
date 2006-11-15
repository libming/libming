#!/usr/bin/php
<?php
$m = new SWFMovie();


/* SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);

/* SWF_DOACTION */
$m->add(new SWFAction("
var a = 1;
") );

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

$m->save("test01.swf");
?>
