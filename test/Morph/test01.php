#!/usr/bin/php -c.
<?php
$m = new SWFMovie(8);

$morph = new SWFMorph();

$shape1 = $morph->getShape1();
$shape2 = $morph->getShape2();

$shape1->setLine(4, 25, 0, 0, 128);
$shape1->movePenTo(5, 5);
$shape1->drawLineTo(50, 30);

$shape2->setLine(4, 25, 0, 0, 128);
$shape2->movePenTo(5, 5);
$shape2->drawLineTo(50, 130);

$item = $m->add($morph);
for ($i = 0; $i < 10; $i++)
{
	$item->setRatio(0.1 * $i);
	$m->nextFrame();
}

$m->save("test01.swf");

?>
