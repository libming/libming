# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Gradient;
use SWF();

$SWF::Gradient::VERSION = $SWF::VERSION;

1;

=head1 NAME

SWF::Gradient - SWF gradient class

=head1 SYNOPSIS

	use SWF::Gradient;
	use SWF::Constants qw(:Gradient);
	use SWF::Shape;
	$g = new SWF::Gradient();
	$g->addEntry(0.0,255,0,0,200);
	$g->addEntry(0.16,255,255,0,200);
	$g->addEntry(0.32,0,255,0,200);
	$g->addEntry(0.5,0,255,255,200);
	$g->addEntry(0.66,0,50,255,200);
	$g->addEntry(0.83 ,255,0,255,200);
	$g->addEntry(1.0,255,0,0,200);
	# ..make some kind of rainbow..
	$sh = new SWF::Shape();
	$fill = $sh->addGradientFill($g,SWFFILL_LINEAR_GRADIENT);

=head1 DESCRIPTION

SWF::Gradient is a helper class most useful for filling shapes (i.e. SWF::Shape objects),
and also for filtering via SWF::DisplayItem methods using GradientGlowFilter or GradientBevelFilter,
both are SWF::Filter objects.

=head1 METHODS

=over

=item new SWF::Gradient()

Creates and returns a gradient object. This object is an 'empty' gradient, to be filled
with control point entries via up to 15 addEntry() method calls. 
(For SWF version 7 are only 8 control points possible.)
For gradients used for filters also only 8 control points are possible.

=item addEntry($ratio, $r, $g, $b [, $a])

Adds a gradient control point. The ratio parameter is a floating point value for 
controlling the position of the following RGBA-color value (consiting of red, green, 
blue and optional alpha parts) in an increasing row between 0.0 and 1.0 . 
Adding more than 15 entries will be useless and ignored.

=item setSpreadMode($mode)

For version >= 8 only: set a gradient spread mode: one of 
SWF_GRADIENT_PAD (default), SWF_GRADIENT_REPEAT, SWF_GRADIENT_REFLECT .

=item setInterpolationMode($mode)

For version >= 8 only: set a gradient interpolation mode: one of 
SWF_GRADIENT_NORMAL (default), SWF_GRADIENT_LINEAR .

=item setFocalPoint($focalpoint)

Makes a focal gradient 'asymetric'. A focalpoint value of -1.0 means the focal point 
is close to the left border, 0.0 is in the center and 1.0 is close to the right border.
Also this feature is available for version >= 8 only.  
For best results use the SWFFILL_FOCAL_GRADIENT flag when adding to v4 shapes, i.e. like

	$g = new SWF::Gradient();
	$g->setFocalPoint(-0.9);
	$g->addEntry(0.000000,0xff,0x00,0x00,0xff);
	$g->addEntry(1.000000,0x00,0xff,0x00,0xff);
	$shape->useVersion(4);
	$shape->addGradientFill($g,SWFFILL_FOCAL_GRADIENT); 

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Shape, SWF::Fill, SWF::Filter, SWF::Constants

=cut
