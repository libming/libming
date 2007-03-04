#!/usr/bin/perl -w
use SWF qw(:ALL);
use SWF::Constants qw(:Text :Button :DisplayItem :Fill);

$m = SWF::Movie::newSWFMovieWithVersion(8);
$m->save("test06.swf");
