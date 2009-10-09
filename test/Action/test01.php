#!/usr/bin/php -c php.ini
<?php

$m = new SWFMovie();

$a = new SWFAction("trace('');");
$init = new SWFInitAction($a);
$m->add($init);

$m->save("test01.swf");

?>
