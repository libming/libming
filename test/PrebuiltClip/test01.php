#!/usr/bin/php -c.
<?php
$m = new SWFMovie();
$m->save("empty.swf");

$m = new SWFMovie();
$pbc1 = new SWFPrebuiltClip(fopen("empty.swf", "rb"));
$m->add($pbc1);

$pbc2 = new SWFPrebuiltClip(fopen("empty.swf", "rb"));
$m->add($pbc2);

$m->save("test01.swf");
?>
