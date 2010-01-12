#!/usr/bin/php -c.
<?php

$m = new SWFMovie(8);

$shape1 = new SWFShape();
$shape1->setLine2(1, SWF_LINESTYLE_FLAG_HINTING, 0, 25, 0, 0, 128);
$shape1->movePenTo(5, 5);
$shape1->drawLineTo(50, 30);

$item = $m->add($shape1);
$m->nextFrame();
$m->replace($item, $shape1);
$m->nextFrame();

$m->save("test02.swf");

?>
