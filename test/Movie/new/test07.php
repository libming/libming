#!/usr/bin/php -c php.ini
<?php
$m = new SWFMovie(8);

$m->setBackground(0, 0, 0);
$m->save("test07.swf");
?>
