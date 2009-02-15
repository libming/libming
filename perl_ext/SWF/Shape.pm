# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Shape;
use SWF ();

$SWF::Shape::VERSION = $SWF::VERSION;

sub setLeftFill{
    my $self = shift;

    return $self->xs_setLeftFill() unless scalar @_;
    return $self->xs_setLeftFill($_[0]) if ( ref($_[0]) eq 'SWF::Fill');
    return $self->xs_setLeftFill($self->addSolidFill(@_));
}

sub setRightFill{
    my $self = shift;

    return $self->xs_setRightFill() unless scalar @_;
    return $self->xs_setRightFill($_[0]) if ( ref($_[0]) eq 'SWF::Fill');
    return $self->xs_setRightFill($self->addSolidFill(@_));
}

1;

__END__

=head1 NAME

SWF::Shape - Shape class

=head1 SYNOPSIS

	use SWF::Shape;
	$shape = new SWF::Shape();
	$dispitem = $movie->add($shape);

=head1 DESCRIPTION

SWF::Shape is the fundamental object of a flash movie.

=head1 METHODS

=over 4

=item new SWF::Shape()

Creates a new Shape object.

=item $shape->movePenTo($x, $y)

Move the Shape's (imaginary) pen location to ($x, $y) in the Shape's coordinate space.

=item $shape->movePen($dx, $dy)

Add ($dx, $dy) to the Shape's pen current location.

=item $x = $shape->getPenX()

Returns Pen's x-coordinates

=item $y = $shape->getPenY()

Returns Pen's y-coordinates

=item ($x, $y) = $shape->getPen()

Returns Pen's (x,y) coordinates

=item $shape->drawLineTo($x, $y)

Draw a line (using current line style, set by one of the setLine...() methods) from the current position to ($x, $y).

=item $shape->drawLine($dx, $dy)

Draw a line (using current line style set by one of the setLine...() methods) from the current position to displacement ($dx, $dy).

=item $shape->drawCurveTo($controlx, $controly, $anchorx, $anchory)

Draw a quadratic curve (using current line style, set by one of the setLine...() methods)
from current pen position to ($anchorx, $anchory) using control point ($controlx, $controly).

=item $shape->drawCurve($controldx, $controly, $anchorx, $anchory)

Draw a quadratic curve (using current line style, set by one of the setLine...() methods)
from current pen position to displacement ($anchordx, $anchordy) using displacement control point ($controldx, $controldy).

=item $shape->drawCubicTo($bx,$by,$cx,$cx,$dx,$dy)

=item $shape->drawCubic($bx,$by,$cx,$cx,$dx,$dy)

Draw a cubic curve (using current line style).
[ToDo: Add some more details and demo code]

=item $shape->setLine($width, $r, $g, $b [,$a])

Sets Shape's line width and color (RGB). To remove the Shape's line style call $shape->hideLine().
The used line width for drawing is a product of $width argument and scale value set, see SWF::setScale($scale).
Arguments $r, $g, $b stand for red, green and blue color parts, optional $a for alpha value.

=item $shape->setLineStyle($width, $r, $g, $b [,$a])

This OLD deprecated method is documented here only for compatibility reason.
The line width -in TWIPS units- is independent of Ming scale value (more bug than feature).

=item $shape->setLine2($width, $r, $g, $b ,$a, $flags, $miterLimit)

A new method available for SWF version >=8 setting shape's line width and color (RGBA) with additional
control flags (see below) and miter limit. To remove the Shape's line style call $shape->hideLine().
The used line width for drawing is a product of $width argument and scale value set, see SWF::setScale($scale).
Arguments $r, $g, $b, $a stand for red, green, blue and alpha color parts. $a is not optional here.

Available flags are:
Line cap style: select one of the following flags (default is round cap style)

	SWF_LINESTYLE_CAP_ROUND 
	SWF_LINESTYLE_CAP_NONE
	SWF_LINESTYLE_CAP_SQUARE 

Line join style: select one of the following flags (default is round join style)

	SWF_LINESTYLE_JOIN_ROUND
	SWF_LINESTYLE_JOIN_BEVEL 
	SWF_LINESTYLE_JOIN_MITER  

Scaling flags: disable horizontal / vertical scaling

	SWF_LINESTYLE_FLAG_NOHSCALE
	SWF_LINESTYLE_FLAG_NOVSCALE 

Enable pixel hinting to correct blurry vertical / horizontal lines:  all anchors will be aligned to full pixels

	SWF_LINESTYLE_FLAG_HINTING  

Disable stroke closure: if no-close flag is set caps will be applied instead of joins.

	SWF_LINESTYLE_FLAG_NOCLOSE

End-cap style: default round

	SWF_LINESTYLE_FLAG_ENDCAP_ROUND
	SWF_LINESTYLE_FLAG_ENDCAP_NONE
	SWF_LINESTYLE_FLAG_ENDCAP_SQUARE

If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor must be set. Miter max length is then calculated as:
max miter len = miter limit * width. If join style is not miter, this value will be ignored.

=item $shape->setLineStyle2($width, $r, $g, $b ,$a, $flags, $miterLimit)

This OLD deprecated method is mentioned here only for compatibility reason, you'd better use setLine2().

=item $shape->setLine2Filled($width, $fill, $flags, $miterLimit)

A new method available for SWF version >=8 setting shape's line width and fill style, $fill is an object
of SWF::Fill class. For other argument details see item $shape->setLine2().

=item $shape->setLine2StyleFilled($width, $fill, $flags, $miterLimit)

This OLD deprecated method is mentioned here only for compatibility reason, you'd better use setLine2Filled().

=item $shape->hideLine()

Remove the Shape's line, i.e. make lines painted after this method call invisible.

=item $fill = $shape->addSolidFill($r, $g, $b [,$a])

=item $fill = $shape->addFill($r, $g, $b [,$a])

Adds a solid fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the "setLine()" method described below. The default value of $a is 0.

=item $fill = $shape->addBitmapFill($bitmap [,FLAG])

=item $fill = $shape->addFill($bitmap [,FLAG])

Adds a bitmap fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the "setLine()" method described below. 

$bitmap is an SWF::Bitmap object while FLAG argument can be SWFFILL_TILED_BITMAP or SWFFILL_CLIPPED_BITMAP. Make sure to import these constants to your package before useing it, i.e. use the following statement:

     use SWF::Fill <list of constants>;

The default value of FLAG is SWFFILL_TILED_BITMAP.

=item $fill = $shape->addGradientFill($gradient [,FLAG])

=item $fill = $shape->addFill($gradient [,FLAG])

Adds a gradient fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the "setLine()" method described below. 

$gradient is an SWF::Gradient object while FLAG argument can be SWFFILL_LINEAR_GRADIENT or SWFFILL_RADIAL_GRADIENT, plus SWFFILL_FOCAL_GRADIENT for V8.
Make sure to import these constants to your package before useing it, i.e. use the following statement:

     use SWF::Fill <list of constants>;

The default value of FLAG is SWFFILL_LINEAR_GRADIENT.

=item $shape->xs_setLeftFill($fill)

=item $shape->setLeftFill($fill)

Sets the fill on the left side of the edge. $fill is an SWF::Fill object returned from "addFill" method.

=item $shape->setLeftFill($r, $g, $b [,$a])

This is another way of saying:

     my $fill = $shape->addSolidFill($r, $g, $b [,$a]);
     $shape->setLeftFill($fill);

=item $shape->xs_setRightFill($fill)

=item $shape->setRightFill($fill)

Sets the fill on the right side of the edge. $fill is an SWF::Fill object returned from "addFill" method.

=item $shape->setRightFill($r, $g, $b [,$a])

This is another way of saying:

     my $fill = $shape->addSolidFill($r, $g, $b [,$a]);
     $shape->setRightFill($fill);

=item $shape->drawCircle($r)

Draws a circle with radious $r, also changes current position.

=item $shape->drawArc($r, $startAngle, $endAngle)

Draws an arc with radious $r form start to end.

=item $shape->useVersion(version)

This function allows to set the shapes version information. The version is only
a hint. If necessary the version is upgraded. Valid values: are 3 and 4.
Value of 3 is default, but 4 if linestyle2 is used. 
Some shape features are available only for shape version 4 and 
SWF file version>=8. One example is the usage of focal gradients:

	$g = new SWF::Gradient();
	$g->setFocalPoint(-0.9);
	$g->addEntry(0.000000,0xff,0x00,0x00,0xff);
	$g->addEntry(1.000000,0x00,0xff,0x00,0xff);
	$shape->useVersion(4);
	$shape->addGradientFill($g,SWFFILL_FOCAL_GRADIENT); 

=item $version = $shape->getVersion()

Gets the current shape object version.

=item $shape->dumpOutline()

Returns a string describing shape's outline. After drawing in a shape 
expect getting a multi-line string like:

	moveto 368,5
	curveto 348,5 327,1
	curveto 307,-3 284,-15
etc. etc.

=item $shape->end()

Stops drawing on this shape and make the shape complete.
Further method calls won't produce any lines, circles etc. 
By using this demo code later no circle is painted:

	$shape->drawLineTo(300,300);
	$shape->end();
	$shape->drawCircle(200);

=item $shape->drawGlyph($font, $cnumber [, $size])

Draws character of $cnumber of $font. Also Unicode fonts are possible, if the font does provide that, just like next example:

	$font=new SWF::Font("Thai-Bold.fdb");
	$sh->drawGlyph($font, 0x0E44, 44);

=item $shape->drawCharacterBounds($character)

Draws an outer rectangle fitting for $character, an object of SWF::Character.

=item $shape->setRenderHintingFlags(flags)

Set render hinting flags. Possible values are 
	SWF::Constants::SWF_SHAPE_USESCALINGSTROKES and
	SWF::Constants::SWF_SHAPE_USENONSCALINGSTROKES	

This method also will set version of this SWF::Shape object to 4.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net), Albrecht Kleine

=head1 SEE ALSO

SWF, SWF::Bitmap, SWF::Button, SWF::Constants, SWF::DisplayItem, 
SWF::Fill, SWF::Font, SWF::Gradient, SWF::Morph, SWF::Movie, SWF::MovieClip,
SWF::TextField, SWF::Text, SWF::Character

=cut
