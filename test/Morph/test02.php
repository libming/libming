#!/usr/bin/php -c.
<?php
$m = new SWFMovie(8);

$morph = new SWFMorph();

$shape1 = $morph->getShape1();
$shape2 = $morph->getShape2();

$shape1->setLine2(20, SWF_LINESTYLE_FLAG_HINTING, 0, 25, 0, 0, 128);
$shape1->movePenTo(5, 5);
$shape1->drawLineTo(50, 30);

$shape2->setLine2(20, SWF_LINESTYLE_FLAG_HINTING, 0, 25, 100, 100, 255);
$shape2->movePenTo(5, 5);
$shape2->drawLineTo(50, 130);

$item = $m->add($morph);
for ($i = 0; $i < 10; $i++)
{
	$item->setRatio(0.1 * $i);
	$m->nextFrame();
}

$m->save("test02.swf");

?>
