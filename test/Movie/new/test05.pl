#!/usr/bin/perl

use SWF::Movie;

$m  = SWF::Movie::newSWFMovieWithVersion(7);

$m->save("test05.swf");

