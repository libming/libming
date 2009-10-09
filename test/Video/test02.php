#!/usr/bin/php -c php.ini
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);

$stream1 = new SWFVideoStream();
$m->add($stream1);

$stream2 = new SWFVideoStream();
$m->add($stream2);

$m->save("test02.swf");
?>
