#!/usr/bin/php -c.

<?

$m = new SWFMovie(7);
$s = new SWFShape();

$s->setLine(4, 25, 0, 0, 128);
$s->movePenTo(5, 5);
$s->drawLineTo(0, 10);

$bl = new SWFBlur(5, 5, 2);

$c = Array();
$c["red"] = 0;
$c['green'] = 0;
$c['blue'] = 0;
$c['alpha'] = 0xff;

$f = new SWFFilter(SWFFILTER_TYPE_GLOW, $c, $bl, 1.0, SWFFILTER_MODE_INNER | SWFFILTER_MODE_KO);


$bu = new SWFButton();
$bu->addShape($s, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
$item = $m->add($bu);
$item->addFilter($f);
$m->save("test03.swf");
?>
