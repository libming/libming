#!/usr/bin/perl

use SWF;

$m  = new SWF::Movie();

$m->setRate(24.00);

$m->save("test01.swf");

