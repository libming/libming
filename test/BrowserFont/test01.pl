#!/usr/bin/perl -w

use SWF qw(:ALL);

$m = new SWF::Movie();

$tf = new SWF::TextField();

$font = new SWF::BrowserFont("_sans" );

$tf->setFont($font);
$tf->setHeight(20);
$tf->setColor(0x00, 0x00, 0x00, 0xff);
$tf->addString('abc');

$m->add($tf);
$m->nextFrame(); 
$m->save("test01.swf");
