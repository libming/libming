#!/usr/bin/perl -w

use strict;
use SWF (':ALL');
use SWF::Constants (':Button');
my $m = new SWF::Movie;

my $s = new SWF::Shape;
$s->setLine(10, 255, 0, 0, 255);
$s->setRightFill(255, 255, 0, 255);
$s->movePenTo(-50,-20);
$s->drawLine(100, 0);
$s->drawLine(0, 40);
$s->drawLine(-100, 0);
$s->drawLineTo(-50, -20);

# animation by relative scaling
my $mc1= new SWF::MovieClip;
my $si1=$mc1->add($s);
$mc1->nextFrame;
$si1->scale(0.8);
$mc1->nextFrame;
$si1->scale(0.8);
$mc1->nextFrame;
$si1->scale(0.8);
$mc1->nextFrame;
$si1->scale(0.8);
$mc1->nextFrame;
$si1->scale(0.8);
$mc1->nextFrame;

# another animation by absolute scaling
my $mc2= new SWF::MovieClip;
my $si2=$mc2->add($s);
$si2->scaleTo(0.2);
$mc2->nextFrame;
$si2->scaleTo(0.4);
$mc2->nextFrame;
$si2->scaleTo(0.6);
$mc2->nextFrame;
$si2->scaleTo(1);
$mc2->nextFrame;

my $b = new SWF::Button;
my $br1 = $b->addCharacter($s, 
	SWF::Constants::SWFBUTTON_HIT|SWF::Constants::SWFBUTTON_UP);
my $br2 = $b->addCharacter($mc1, SWF::Constants::SWFBUTTON_OVER);
my $br3 = $b->addCharacter($mc2, SWF::Constants::SWFBUTTON_DOWN);

my $d = $m->add($b);
$d->moveTo(150, 100);

$m->nextFrame;
$m->save("test03.swf");
