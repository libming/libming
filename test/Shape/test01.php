#!/usr/bin/php -c.
<?php

$m = new SWFMovie(8);

$shape1 = new SWFShape();
$shape2 = new SWFShape();

$shape2->useVersion(SWF_SHAPE4);


$m->add($shape1);
$m->add($shape2);

$m->save("test01.swf");
?>
