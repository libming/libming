#!/usr/bin/perl -w

use SWF qw(:ALL);

use SWF::Constants qw(:Filter :Text :Button :DisplayItem :Fill);

$m = SWF::Movie::newSWFMovieWithVersion(7);

$s = new SWF::Shape();
$s->setLine(4, 25, 0, 0, 128);
$s->movePenTo(5, 5);
$s->drawLineTo(0, 10);

$bl = new SWF::Blur(5, 5, 2);
$f = SWF::Filter::newBlurFilter($bl);

$bu = new SWF::Button();
$bu->addCharacter($s, SWFBUTTON_HIT | SWFBUTTON_DOWN | SWFBUTTON_OVER | SWFBUTTON_UP);

$item = $m->add($bu);
$item->addFilter($f);

$m->save("test01.swf");
