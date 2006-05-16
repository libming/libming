#!/usr/bin/perl

use SWF::Movie;

$m  = SWF::Movie::newSWFMovieWithVersion(6);

$m->save("test04.swf");

