#!/usr/bin/php -c.

<?

$m = new SWFMovie(7);
$s = new SWFShape();

$s->setLine(4, 25, 0, 0, 128);
$s->movePenTo(5, 5);
$s->drawLineTo(0, 10);

$bl = new SWFBlur(5, 5, 2);
$f = new SWFFilter(SWFFILTER_TYPE_BLUR, $bl);

$bu = new SWFButton();
$bu->addShape($s, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
$item = $m->add($bu);
$item->addFilter($f);
$m->save("test01.swf");
?>
