#!/usr/bin/perl

use SWF;

$m  = new SWF::Movie();

$m->setDimension(640.00,480.00);

$m->save("test01.swf");

