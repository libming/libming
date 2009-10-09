#!/usr/bin/perl -w

use SWF qw(:ALL);
use SWF::Constants qw(:Shape);

$m = SWF::Movie::newSWFMovieWithVersion(8);

$shape1 = new SWF::Shape();
$shape1->setLine2(1, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
$shape1->movePenTo(5, 5);
$shape1->drawLineTo(50, 30);

$shape2 = new SWF::Shape();
$shape2->setLine2(1, 25, 100, 100, 255, SWF_LINESTYLE_FLAG_HINTING, 0);
$shape2->movePenTo(5, 5);
$shape2->drawLineTo(50, 130);

$item = $m->add($shape1);
$m->nextFrame();
$m->replace($item, $shape2);
$m->nextFrame();

$m->save("test01.swf");

