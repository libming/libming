#!/usr/bin/php
<?php
$m = new SWFMovie();


/* SWF_SETBACKGROUNDCOLOR */
$m->setBackground(0xff, 0xff, 0xff);

/* SWF_DEFINESPRITE */

	/*  MovieClip 1 */
$sp1 = new SWFMovieClip(); /* 1 frames */

/* SWF_SHOWFRAME */
$sp1->nextFrame(); /* end of clip frame 1 */

/* SWF_END */

/* SWF_PLACEOBJECT2 */
/* PlaceFlagHasCharacter */
$m->add($sp1);
/* PlaceFlagHasMatrix */
/* outputSWF_MATRIX is broken, so it is being skipped.. */

/* SWF_SHOWFRAME */
$m->nextFrame(); /* end of frame 1 */

$m->save("test02.swf");
?>
