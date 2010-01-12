#!/usr/bin/php -c.
<?php

$srcdir = $argv[1];

$m = new SWFMovie(7);
$m->setRate(1);
$m->setDimension(200, 150);

$stream1 = new SWFVideoStream();
$stream1->setDimension(200, 150);
$it = $m->add($stream1);
$it->setName("video");

$action = new SWFAction("
	nc = new NetConnection ();
	nc.connect (null);
	ns = new NetStream (nc);
	video.attachVideo (ns);
	ns.play (\"video.flv\");
");

$m->add($action);

$m->save("test03.swf");

?>
