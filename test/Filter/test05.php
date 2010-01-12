#!/usr/bin/php -c.

<?

$srcdir = $argv[1];

$m = new SWFMovie(9);
$m->setBackground(0xcc, 0xcc, 0xcc);
$s = new SWFShape();
$img = new SWFBitmap($srcdir . "/../Media/image01.dbl");
$fill = $s->addFill($img, SWFFILL_TILED_BITMAP);
$s->setRightFill($fill);
$w = $img->getWidth();
$h = $img->getHeight();

$s->drawLine($w, 0);
$s->drawLine(0, $h);
$s->drawLine(-$w, 0);
$s->drawLine(0, -$h);

$cm = Array();

for($i = 0; $i < 20; $i++)
	$cm[$i] = 0.1;

$cmf = new SWFFilterMatrix(5, 4, $cm);
$filter = new SWFFilter(SWFFILTER_TYPE_COLORMATRIX, $cmf);

$bu = new SWFButton();
$bu->addCharacter($s, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
$item = $m->add($bu);
$item->addFilter($filter);
$m->save("test05.swf");
?>
