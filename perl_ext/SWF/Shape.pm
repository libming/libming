# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
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

sub setLine{
    my $self = shift;

    return $self->setLineStyle(0, 0, 0, 0) unless scalar @_;
    $self->setLineStyle(@_);
}

1;

__END__

=head1 NAME

SWF::Shape - Shape class

=head1 SYNOPSIS

 use SWF::Shape;
 my $shape = new SWF::Shape();


=head1 DESCRIPTION

SWF::Shape is the fundamental object of a flash movie.

=head1 METHODS

=over 4

=item new SWF::Shape();

Creates a new Shape object.

=item $shape->moveTo($x, $y); or $shape->movePenTo($x, $y);

Move the Shape's (imaginary) pen location to ($x, $y) in the Shape's coordinate space.

=item $shape->moveToRelative($dx, $dy); or $shape->movePen($dx, $dy);

Add ($dx, $dy) to the Shape's pen current location.

=item $shape->drawLineTo($x, $y);

Draw a line (using current line style - see L<"setLine()"> method) from the current position to ($x, $y).

=item $shape->drawLine($dx, $dy);

Draw a line (using current line style - see L<"setLine()"> method) from the current position to displacement ($dx, $dy).

=item $shape->drawCurveTo($controlx, $controly, $anchorx, $anchory);

Draw a quadratic curve (using current line style - see L<"setLine()"> method) from current pen position to ($anchorx, $anchory) using control point ($controlx, $controly).

=item $shape->drawCurve($controldx, $controly, $anchorx, $anchory);

Draw a quadratic curve (using current line style - see  L<"setLine()"> method) from current pen position to displacement ($anchordx, $anchordy) using displacement control point ($controldx, $controldy).

=item $shape->setLine($width, $r, $g, $b [,$a]);

Sets Shape's line width and color (RGB). To remove the Shape's line style call this method without any arguments, i.e. $shape->setLine().

=item $fill = $shape->addFill($r, $g, $b [,$a]);

Adds a solid fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the  L<"setLine()"> method described below. The default value of $a is 0.

=item $fill = $shape->addFill($bitmap [,FLAG]);

Adds a bitmap fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the L<"setLine()"> method described below. 

$bitmap is an SWF::Bitmap object while FLAG arguemnt can be SWFFILL_TILED_BITMAP or SWFFILL_CLIPPED_BITMAP. Make sure to import these constants to your package before useing it, i.e. use the following statement:

     use SWF::Fill <list of constants>;

The default value of FLAG is SWFFILL_TILED_BITMAP.


=item $fill = $shape->addFill($gradient [,FLAG]);

Adds a gradient fill to $shape list of fill styles and returns an SWF::Fill object. This object is used with the L<"setLine()"> method described below. 

$gradient is an SWF::Gradient object while FLAG arguemnt can be SWFFILL_LINEAR_GRADIENT or SWFFILL_RADIAL_GRADIENT.Make sure to import these constants to your package before useing it, i.e. use the following statement:

     use SWF::Fill <list of constants>;

The default value of FLAG is SWFFILL_LINEAR_GRADIENT.


=item $shape->setLeftFill($fill);

Sets the fill on the left side of the edge. $fill is an SWF::Fill object returned from L<"addFill"> method.

=item $shpae->setLeftFill($r, $g, $b [,$a]);

This is another way of saying:

     my $fill = $shape->addSolidFill($r, $g, $b [,$a]);
     $shape->setLeftFill($fill);


=item $shape->setRightFill($fill);

Sets the fill on the right side of the edge. $fill is an SWF::Fill object returned from L<"addFill"> method.

=item $shpae->setRightFill($r, $g, $b [,$a]);

This is another way of saying:

     my $fill = $shape->addSolidFill($r, $g, $b [,$a]);
     $shape->setRightFill($fill);


=item $shape->drawCircle($r) - Don't use. Interface may change.

Draws a circle with radious $r.

=item $shape->drawArc($r, $startAngle, $endAngle)  - Don't use. Interface may change.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Button, SWF::Movie, .


=cut
