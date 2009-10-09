#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = SWF::Movie::newSWFMovieWithVersion(8);

$shape1 = new SWF::Shape();
$shape2 = new SWF::Shape();

$shape2->useVersion(4);

$m->add($shape1);
$m->add($shape2);

$m->save("test01.swf");
