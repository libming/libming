#!/usr/bin/php -c.
<?php
	$m = new SWFMovie();
	$import = $m->importChar("test.swf", "test");
	$m->add($import);
	$m->save("test01.swf");
?>
