# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 by Albrecht Kleine
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
	$dispitem = $movie->add($shape);
	$dispitem->rotate(45);

=head1 DESCRIPTION

When you place an SWF object (one of the types that can be seen with eyes by user) 
in a frame of a SWF::Movie or SWF::MovieClip, the return value will be
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

Set $displayItem scale to $x in the x-direction and $y in the y-direction. If $y is not specified, $y=$x is assumed.

=item $displayItem->scale($x [,$y]);

Multiply $displayItem scale by $x in the x-direction and $y in the y-direction. If $y is not specified, $y=$x is assumed.

=item $displayItem->rotateTo($degrees);

Set $displayItem rotation to $degrees.

=item $displayItem->rotate($degrees);

Rotate $displayItem by $degrees.

=item $displayItem->skewX($x);

Add $x to the current x-skew.

=item $displayItem->skewXTo($x);

Set x-skew to $x. 1.0 is 45-degree forward slant. More is more forward while less is more backward.

=item $displayItem->skewY($y);

Add $y to the current y-skew.

=item $displayItem->skewYTo($y);

Set y-skew to $y. 1.0 is 45-degree upward slant. More is more upward while less is more downward.

=item $displayItem->setMatrix($a, $b, $c, $d, $e, $f)

Do an operation of rotating/skewing (b,c), moving (e,f) and scaling (a,d) at once.
The default initial values of an SWF::DisplayItem object's matrix are 1.0, 0, 0, 1.0, 0, 0 . 
So calling setMatrix with these defaults (I<setMatrix(1.0, 0, 0, 1.0, 0, 0);>)
will reset results of earlier calls of SWF::DisplayItem methods (like rotate(45) etc. etc.)

=item $displayItem->setDepth($depth);

Set Z-order of $displayItem to $depth.

=item $displayItem->setRatio($ratio);

Useful for SWF::Morph. Sets $displayItem ratio to $ratio.

=item $displayItem->setColorAdd($r, $g, $b [,$a]))

=item $displayItem->addColor($r, $g, $b [,$a]);

Add RGB color to the $displayItem's color transform. Default value of $a is 1.0

=item $displayItem->setColorMult($r, $g, $b [,$a]))

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

Sets a mask level: display items with lower or equal depth are masked, 
any other display items are not masked. 
Use setDepth() to control desired masking.

=item $displayItem->endMask()

End masking started by prior setMask() call.

=item $displayItem->addAction( $action, $flags )

Add $action, an object of SWF::Action class.
The flags are exported from SWF::Constants.

	SWFACTION_ONLOAD
	SWFACTION_ENTERFRAME
	SWFACTION_UNLOAD
	SWFACTION_MOUSEMOVE
	SWFACTION_MOUSEDOWN
	SWFACTION_MOUSEUP
	SWFACTION_KEYDOWN
	SWFACTION_KEYUP
	SWFACTION_DATA

Using this flags you have control at which events the action will run.

=item $displayItem->setBlendMode($mode)

Set an alternative blend mode instead of default alpha blend.
Possible modes are:

	SWFBLEND_MODE_NULL
	SWFBLEND_MODE_NORMAL
	SWFBLEND_MODE_LAYER
	SWFBLEND_MODE_MULT
	SWFBLEND_MODE_SCREEN
	SWFBLEND_MODE_DARKEN
	SWFBLEND_MODE_LIGHTEN
	SWFBLEND_MODE_ADD
	SWFBLEND_MODE_SUB
	SWFBLEND_MODE_DIFF
	SWFBLEND_MODE_INV
	SWFBLEND_MODE_ALPHA
	SWFBLEND_MODE_ERASE
	SWFBLEND_MODE_OVERLAY
	SWFBLEND_MODE_HARDLIGHT

Here comes some demonstration code: 

	use SWF::Constants qw(:DisplayItem); 
	# ....
	$sh=new SWF::Shape();
	$fill = $sh->addFill(255, 0, 0, 255);  				# red
	$sh->setRightFill($fill);
	$sh->drawLine(440, 0);
	$sh->drawLine(0, 380);
	$sh->drawLine(-440, 0);
	$sh->drawLine(0, -380);
	#
	$sh2=new SWF::Shape();
	$fill2 = $sh2->addFill(0, 255, 0, 255);  			# green
	$sh2->setRightFill($fill2);
	$sh2->drawLine(240, 0);
	$sh2->drawLine(0, 280);
	$sh2->drawLine(-240, 0);
	$sh2->drawLine(0, -280);

	$di=$m->add($sh);
	$di2=$m->add($sh2);
	#  $di2->setBlendMode( SWFBLEND_MODE_NORMAL);			# would be green  ( as you have expected )
	$di2->setBlendMode(  SWFBLEND_MODE_ADD);			# y e l l o w  ( surprising, a litle bit )

=item $displayItem->cacheAsBitmap($flag)

Set a flag (value 0 or 1) showing the character can be cached as a bitmap. 
This might improve rendering speed, if the object does no change often.
This feature is available for SWF version >= 8 only.

=item $displayItem->flush()

Writes the SWF::DisplayItem object immediately to the blocklist.
Usually MING waits with writing a display item until a frame is closed
through a nextFrame() call, because a display items state could be altered 
for the current frame. By using the flush() method MING does not wait 
and writes the frame immediately. Therefore an user can influence the 
swf tag order. Changing a display items state after calling flush() takes 
effect in the next frame.

=item $matrix = $displayItem->getMatrix()

Returns an associated SWF::Matrix object.

=item $character = $displayItem->getCharacter()

Returns the associated SWF::Character object.

=item $displayItem->addFilter( $filter )

Process the DisplayItem object thru a prepared filter:
an object of SWF::Filter class, e.g. BlurFilter or DropShadowFilter.
Filters are available since player version 8.

=item $displayItem->setCXform( $cxform )

Process the DisplayItem object thru $cxform: a prepared color 
transformation object of SWF::CXform class.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil AT users.sourceforge.net)
Peter Liscovius
Albrecht Kleine

=head1 SEE ALSO

SWF, SWF::Button, SWF::Movie, SWF::MovieClip, SWF::Shape, SWF::Text, SWF::TextField,
SWF::Filter, SWF::CXform, SWF::Matrix, SWF::Action, SWF::Morph, SWF::Character,
SWF::Constants

=cut
