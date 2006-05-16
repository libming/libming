#!/usr/bin/perl

use SWF::Movie;

$m  = SWF::Movie::newSWFMovieWithVersion(4);

$m->save("test02.swf");

