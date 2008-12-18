#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;

$m = SWF::Movie::newSWFMovieWithVersion(7);

$stream1 = new SWF::VideoStream();
$m->add($stream1);

$stream2 = new SWF::VideoStream();
$m->add($stream2);

$m->save("test02.swf");

