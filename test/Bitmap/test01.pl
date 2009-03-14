#!/usr/bin/perl -w

use SWF qw(:ALL);

die "Usage: ".$0." <sourcedir>\n" unless @ARGV;
$mediadir="$ARGV[0]/../Media";

use SWF qw(:ALL);

# Just copy from a sample, needed to use Constants like SWFFILL_RADIAL_GRADIENT
#use SWF::Constants qw(:Text :Button :DisplayItem :Fill);

$m = SWF::Movie::newSWFMovieWithVersion(8);

SWF::setScale(1.0);
#Unknown block type 69

# SWF_DEFINELOSSLESS 

$character1 = new SWF::Bitmap($mediadir."/image01.png");

# SWF_DEFINESHAPE3 

# Shape 2 (TYPE=3, RECT=0,400 0,400)
$character2 = new SWF::Shape();
#1 fillstyle(s)
# BitmapID: 1 
$character2_f0 = $character2->addBitmapFill($character1); #,SWFFILL_BITMAP);
$character2_f0->scaleTo(20.000000);
#0 linestyles(s)
$character2->setRightFill($character2_f0);
$character2->drawLine(400, 0);
$character2->drawLine(0, 400);
$character2->drawLine(-400, 0);
$character2->drawLine(0, -400);

$m->add($character2);

# SWF_SHOWFRAME 
$m->nextFrame(); # end of frame 1 

# SWF_END 

$m->save("test01.swf");
