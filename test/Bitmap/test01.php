#!/usr/bin/php -c.

<?php

$srcdir = $argv[1];
$mediadir = $srcdir . "/../Media";

$m = new SWFMovie(8);

$b = new SWFBitmap($mediadir . '/image01.png');

$m->addExport($b, "BitmapExport");
$m->writeExports();

$m->save("test01.swf");
?>
