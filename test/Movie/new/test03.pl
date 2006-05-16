#!/usr/bin/perl

use SWF::Movie;

$m  = SWF::Movie::newSWFMovieWithVersion(5);

$m->save("test03.swf");

