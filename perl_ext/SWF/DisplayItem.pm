# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::DisplayItem;
use SWF ();

1;

__END__

=head1 NAME

SWF::DisplayItem - SWF DisplayItem class

=head1 SYNOPSIS


 use SWF::DisplayItem();   # Do not import any constant

 #import all constants to the current package
 use SWF::DisplayItem qw(:DisplayItem);

=head1 DESCRIPTION

SWF::DisplayItem is used for animation. To create an SWF::DisplayItem object, you may use either L<SWF::Movie/"add"> or L<SWF::Sprite/"add"> method.


By default, SWF::DisplayItem will not import anything to your package, i.e. you have to explicitly indicate which constants you need. Here is the list of constants that you may import to your package:

    SWFACTION_ONLOAD
    SWFACTION_ENTERFRAME
    SWFACTION_UNLOAD
    SWFACTION_MOUSEMOVE
    SWFACTION_MOUSEDOWN
    SWFACTION_MOUSEUP
    SWFACTION_KEYDOWN
    SWFACTION_KEYUP
    SWFACTION_DATA

For example, if you want to import SWFACTION_ONLOAD only, use the following statement:

    use SWF::Button qw(SWFACTION_ONLOAD);

You may use the following shortcut, if you want to import all constants

    use SWF::DisplayItem ':DisplayItem';

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

=item $displayItem->setMask($level);

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::Button, SWF::Shape, SWF::Movie, .

=cut
