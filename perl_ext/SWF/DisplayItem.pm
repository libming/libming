# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::DisplayItem;
use SWF ();

$SWF::DisplayItem::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::DisplayItem - SWF DisplayItem class

=head1 SYNOPSIS


 use SWF::DisplayItem;


=head1 DESCRIPTION

When you place an SWF object (one of the types that can be seen with eyes by user) 
in a frame of a SWF::Movie or SWF::movieClip, the return value will be
in a SWF::DisplayItem.

You can now modify that item in current and every following frames of the clip where you added the SWF object.

Further it is accessible by ActionScript too. 
Just give the DisplayItem a name with method setName($name) after you added the SWF object to a SWF::Movie or SWF::MovieClip


=head1 METHODS

=over 4

=item $displayItem->moveTo($x, $y);

Move $displayItem to ($x, $y) in global co-ordinates.

=item $displayItem->move($x, $y);

Displace $displayItem by ($x, $y)

=item $displayItem->scaleTo($x [,$y]);

Set $displyItem scale to $x in the x-direction and $y in the y-direction. If $y is not specified, $y=$x is assumed.

=item $displayItem->scale($x [,$y]);

Multiply $displyItem scale by $x in the x-direction and $y in the y-direction. If $y is not specified, $y=$x is assumed.

=item $displayItem->rotateTo($degrees);

Set $displayItem rotation to $degrees.

=item $displayItem->rotate($degrees);

Rotate $displayItem by $degrees.

=item $displayItem->skewX($x);

Add $x to the current x-skew.

=item $displayItem->skewXTo($x);

Set x-skew to $x. 1.0 is 45-degree forward slant. More is more forwad while less is more backward.

=item $displayItem->skewY($y);

Add $y to the current y-skew.

=item $displayItem->skewYTo($y);

Set y-skew to $y. 1.0 is 45-degree upward slant. More is more upward while less is more downward.

=item $displayItem->setDepth($depth);

Set Z-order of $displayItem to $depth.

=item $displayItem->setRatio($ratio);

Useful for SWF::Morph. Sets $displayItem ratio to $ratio.

=item $displayItem->addColor($r, $g, $b [,$a]);

Add RGB color to the $displayItem's color transform. Default value of $a is 1.0

=item $displayItem->multColor($r, $g, $b [,$a]);

Multiplies the $displayItem's color transform by the given values. Default value of $a is 1.0

=item $displayItem->setName($name);

Set $displayItem's name to $name (used for targetting with SWF::Action).

=item $displayItem->remove();

Remove $displayItem from the movie display list.

=item ($x, $y) = $displayItem->getPosition(();

Returns displace coordinates of $displayitem.

=item $degrees = $displayItem->getRotation();

Returns rotation of $displayItem.

=item ($x, $y) = $displayItem->getScale();

Returns scale of $displayItem in x- and y-direction.

=item ($x, $y) = $displayItem->getSkew();

Returns x- and y-skew of $displayItem.

=item $depth = $displayItem->getDepth();

Returns Z-order of $displayItem.

=item $displayItem->setMask($level);

=back

=head1 AUTHOR

Soheil Seyfaie (soheil AT users.sourceforge.net)
Peter Liscovius

=head1 SEE ALSO

SWF, SWF::Button, SWF::Movie, SWF::MovieClip, SWF::Shape, SWF::Text, SWF::TextField

=cut
