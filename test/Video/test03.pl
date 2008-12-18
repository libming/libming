#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;

$m = SWF::Movie::newSWFMovieWithVersion(7);
$m->setRate(1);
$m->setDimension (200, 150);

$stream1 = new SWF::VideoStream();
$stream1->setDimension(200, 150);
$it = $m->add($stream1);
$it->setName("video");

$action = new SWF::Action("
	nc = new NetConnection ();
	nc.connect (null);
	ns = new NetStream (nc);
	video.attachVideo (ns);
	ns.play (\"video.flv\");
");
$m->add($action);

$m->save("test03.swf");


