#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = new SWF::Movie();
$a = new SWF::Action("trace('');");
$init = new SWF::InitAction($a);

$m->add($init);

$m->save("test01.swf");
