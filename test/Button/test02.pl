#!/usr/bin/perl -w

=head ButtonRecord Test

This test references a shape 3 times in a button as ButtonRecord and set some properties 
for each ButtonRecord.
You should see 
1 triangle by viewing the SWF, 
2 triangles when hovering over the first triangle, and 
3 triangles when you press the triangle button.

=cut

use strict;
use SWF (':ALL');
use SWF::Constants qw(:Button);
my $m = new SWF::Movie;

my $s = new SWF::Shape;
#$s->addSolidFill(255, 255, 0,255);
$s->setLine(1, 255, 0, 0, 255);
$s->setRightFill(255, 255, 0, 255);
$s->drawLine(100, 0);
$s->drawLine(0, 40);
$s->drawLineTo(0, 0);

my $b = new SWF::Button;
my $br1 = $b->addCharacter($s, 
	SWF::Constants::SWFBUTTON_HIT|SWF::Constants::SWFBUTTON_UP|SWF::Constants::SWFBUTTON_OVER|SWF::Constants::SWFBUTTON_DOWN);

my $br2 = $b->addCharacter($s, 
	SWF::Constants::SWFBUTTON_OVER|SWF::Constants::SWFBUTTON_DOWN);
$br2->rotate(10);
$br2->move(20,0);

my $br3 = $b->addCharacter($s, SWF::Constants::SWFBUTTON_DOWN);
$br3->rotate(20);
$br3->move(40,0);

my $d = $m->add($b);
$d->moveTo(50, 100);

$m->addExport($b, "ButtonExport");
$m->writeExports();

$m->nextFrame;
$m->save("test02.swf");
