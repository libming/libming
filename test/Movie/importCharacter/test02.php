#!/usr/bin/php -c.
<?php
	$m = new SWFMovie(8);
	$import = $m->importChar("test.swf", "test");
	$m->add($import);
	$m->save("test02.swf");
?>
