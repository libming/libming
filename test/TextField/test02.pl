#!/usr/bin/perl -w

use SWF qw(:ALL);
use SWF::Constants qw(:Text);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

$m = new SWF::Movie();

$f = new SWF::Font($mediadir."/test.ttf");

$tf = new SWF::TextField();
$tf->setFont($f);
$tf->setColor(0,0,0,0xff);
$tf->setHeight(20);

$m->add($tf);
$m->nextFrame();
$m->save("test02.swf");
