#!/usr/bin/perl

use mingc;

$s = mingc::newSWFShape();
$l = mingc::SWFShape_addLineStyle($s, 40, 0x7f, 0, 0, 0xff);
$f = mingc::SWFShape_addSolidFill($s, 0xff, 0, 0, 0xff);
mingc::SWFShape_moveTo($s, 200, 200);
mingc::SWFShape_setLineStyle($s, $l);
mingc::SWFShape_setFillStyle1($s, $f);
mingc::SWFShape_lineTo($s, 6200, 200);
mingc::SWFShape_lineTo($s, 6200, 4600);
mingc::SWFShape_curveTo($s, 200, 4600, 200, 200);

$m = mingc::newSWFMovie();
mingc::SWFMovie_setDimension($m,6400, 4800);
mingc::SWFMovie_setRate($m,12.0);
mingc::SWFMovie_add($m,$s);
mingc::SWFMovie_nextFrame($m);

mingc::SWFMovie_save($m,"test.swf");
