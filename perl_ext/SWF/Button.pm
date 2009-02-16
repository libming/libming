# ====================================================================
# Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Button;
use SWF();

$SWF::Button::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Button - SWF button class

=head1 SYNOPSIS

	use SWF::Button;
	$button = new SWF::Button();                                                                                                                                                                                      
	$button->setUp($shape1);  
	$button->setDown($shape2);

=head1 DESCRIPTION

Creates buttons for flash movies. Buttons are controlled by various
methods for visible design and triggered actions, see below:

=head1 METHODS

=over 4

=item new SWF::Button()

Creates a new Button object.

=item $button->addShape($shape, FLAG)

Adds $shape to the button. Using this method is not recommended, 
better use addCharacter(), see next item for details.
Also unlike addCharacter() this method does not return any SWF::ButtonRecord objects

=item $buttonrecord = $button->addCharacter($character [, $flags])

Adds character $shape to the button. Valid FLAGs are:

	SWFBUTTON_HIT
	SWFBUTTON_UP
	SWFBUTTON_DOWN
	SWFBUTTON_OVER

Unlike addShape() this method returns an object of SWF::ButtonRecord class.
The flag states can be combined using the binary or operator.

=item $buttonrecord = $button->setOver($shape)

Shortcut for $button->addCharacter($shape, SWFBUTTON_OVER);

=item $buttonrecord = $button->setHit($shape)

Shortcut for $button->addCharacter($shape, SWFBUTTON_HIT);

=item $buttonrecord = $button->setUp($shape)

Shortcut for $button->addCharacter($shape, SWFBUTTON_UP);

=item $buttonrecord = $button->setDown($shape)

Shortcut for $button->addCharacter($shape, SWFBUTTON_DOWN);

=item $button->addAction($action [,FLAG])

=item $button->setAction($action [,FLAG])

Adds $action object (see SWF::Action). Valid FLAGs are:

	SWFBUTTON_MOUSEUP
	SWFBUTTON_MOUSEOVER
	SWFBUTTON_MOUSEOUT
	SWFBUTTON_MOUSEDOWN
	SWFBUTTON_DRAGOVER
	SWFBUTTON_DRAGOUT

SWFBUTTON_MOUSEUP is the default value of FLAG.

=item $button->setMenu(0|1)

Parameter is 0 (off) or 1(on).
Can be used for a slight different behavior of buttons.

=item $soundinstance = $button->addSound($sound, $flags)

Plays a prepared SWF::Sound object and returns an object of SWF::SoundInstance.
Once more valid flags are:

	SWFBUTTON_MOUSEUP
	SWFBUTTON_MOUSEOVER
	SWFBUTTON_MOUSEOUT
	SWFBUTTON_MOUSEDOWN

For example:

	$sound = new SWF::Sound("wow.mp3", SWF_SOUND_MP3_COMPRESSED );  
	$soundinstance = $button->addSound($sound, SWFBUTTON_MOUSEDOWN );

=item $button->setScalingGrid($x, $y, $w, $h)

This function (available from SWF>=8) sets a 9 slice scaling grid:
1 2 3 
4 5 6 
7 8 9
X, y, w and h define a rectangle, which is the dimension of the center slice
(5). All other slices are determined out of the characters bounds and the
defined rect. While slice 5 is scaled vertical and horizontal, slice 2 and 8
are only scaled horizontal. Slice 4 and 6 only vertical. The 4 corner slices
are not scaled (1, 3, 7, 9).

=item $button->removeScalingGrid()

Removes scaling grid rectangles.

=back

=head1 AUTHOR

wrapper written by

Soheil Seyfaie   (soheil at users dot sourceforge dot net)
Peter Liscovius (peterdd at users dot sourceforge dot net)
and many others.

=head1 SEE ALSO

SWF, SWF::Action, SWF::Shape, SWF::Movie, SWF::MovieClip, SWF::Constants, SWF::Sound, SWF::SoundInstance

=cut
