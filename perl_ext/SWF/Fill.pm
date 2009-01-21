# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Fill;
use SWF ();

$SWF::Fill::VERSION = $SWF::VERSION;

1;

=head1 NAME

SWF::Fill - SWF fill class

=head1 SYNOPSIS

	use SWF::Fill;
	usw SWF::Shape;
	$shape = new SWF::Shape();
	$fill = $shape->addGradientFill($gradient,SWFFILL_LINEAR_GRADIENT); 

=head1 DESCRIPTION

There is no object constructor because SWF::Fill objects are always returned by
method calls of SWF::Shape objects, like

	$fill = $shape->addSolidFill($r, $g, $b, $a);
	$fill = $shape->addGradientFill($gradient, flags);
	$fill = $shape->addBitmapFill($bitmap, $flag);

You can now modify that SWF::Fill object using following methods.

=head1 NOTE

None of the following methods is designed or useful for solid fill objects,
because it does not make any sense to rotate or scale a solid fill.

=head1 METHODS

=over

=item $fill->moveTo($x, $y)

Move $fill to ($x, $y) in global co-ordinates.

=item $fill->scaleTo($x [,$y])

Set $fill scale to $x in the x-direction and $y in the y-direction. 
If $y is not specified, $y=$x is assumed.

=item $fill->scale($x [,$y])

Multiply $fill scale by $x in the x-direction and $y in the y-direction. 
If $y is not specified, $y=$x is assumed.

=item $fill->rotateTo($degrees)

Set $fill rotation to $degrees.

=item $fill->rotate($degrees)

Rotate $fill by $degrees.

=item $fill->skewX($x)

Add $x to the current x-skew.

=item $fill->skewXTo($x)

Set x-skew to $x. 1.0 is 45-degree forward slant. 
More is more forward while less is more backward.

=item $fill->skewY($y)

Add $y to the current y-skew.

=item $fill->skewYTo($y)

Set y-skew to $y. 1.0 is 45-degree upward slant. 
More is more upward while less is more downward.

=item $fill->setMatrix($a, $b, $c, $d, $e, $f)

Do an operation of rotating/skewing (b,c), moving (e,f) and scaling (a,d) at once.
The default initial values of an SWF::Fill object's matrix are 1.0, 0, 0, 1.0, 0, 0 . 
So calling setMatrix with these defaults (I<setMatrix(1.0, 0, 0, 1.0, 0, 0);>)
will reset results of earlier calls of SWF::Fill methods (like rotate(45) etc. etc.)

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Shape, SWF::DisplayItem, SWF::Constants

=cut
