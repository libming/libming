#!/usr/bin/perl

use SWF;

$m  = new SWF::Movie();

$m->setNumberOfFrames(2);

$m->save("test01.swf");

