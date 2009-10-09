#!/usr/bin/php -c.
<?php

$m = new SWFMovie();

$a = new SWFAction("trace('');");
$init = new SWFInitAction($a, 99);
$m->add($init);

$m->save("test02.swf");

?>
