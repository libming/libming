# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::MovieClip;
use SWF();
use strict;

$SWF::MovieClip::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::MovieClip - MovieClip Class

=head1 SYNOPSIS

	use SWF::MovieClip;
	my $movieclip = new SWF::MovieClip();

=head1 DESCRIPTION

SWF::MovieClip allows you to add animated objects to your Flash movies.

=head1 METHODS

=over 4

=item $mc= new SWF::MovieClip()

Returns an SWF::MovieClip object.

=item $item = $mc->add($swfobject)

When you add following types of objects to the $mc they will return a SWF::DisplayItem:

=over

=item SWF::Button

=item SWF::PrebuiltClip (a whole external swf file)

=item SWF::MovieClip (you can nest them like a tree)

=item SWF::Shape

=item SWF::Text

=item SWF::TextField

=item SWF::VideoStream

=back

[ToDo: to be verified: 
When you add a SWF::Sound the return value is a SWF::SoundInstance:
$si = $mc->add($sound); ]

=item $si = $mc->startSound($sound)

Starts making noise and returns an object of SWF::SoundInstance class.

=item $mc->stopSound($sound)

Stops sound started by startSound() method.

=item $mc->remove($di)

Removes SWF::DisplayItem $di from the display list.

=item $mc->nextFrame()

Move to the next frame in the timeline of SWF::MovieClip $mc

=item $mc->setNumberOfFrames($i)

=item $mc->setFrames($i)

Sets total number of $mc frames to $i
This is an optional autofiller, e.g. when you want to be sure that 2 different movieclips
have 100 frames when playing, but the number of nextFrame() calls in these 2 movieclips is unknown or
dynamic (dependent on data from databases, whatever). By default a movieclip the number of frames
in the timeline is how often you called $mc->nextFrame for this clip.

=item $mc->labelFrame($name)

Sets frame name to $name.
You are then able to access this frame by name in ActionScript, not just by frame number.

=item $mc->setScalingGrid($x, $y, $w, $h)

This function (available from SWF>=8) sets a 9 slice scaling grid:
1 2 3 
4 5 6 
7 8 9
X, y, w and h define a rectangle, which is the dimension of the center slice
(5). All other slices are determined out of the characters bounds and the
defined rect. While slice 5 is scaled vertical and horizontal, slice 2 and 8
are only scaled horizontal. Slice 4 and 6 only vertical. The 4 corner slices
are not scaled (1, 3, 7, 9).
[ToDo: to be verified]

=item $mc->removeScalingGrid()

Removes scaling grid rectangles.

=item $mc->addInitAction($action)

Adds an initial ActionScript block to MovieClip $mc.
These actions are executed before the MovieClip is available
as a script object.
[ToDo: to be verified]

=item $mc->setSoundStream($sound, $rate, [$skip])

Includes streaming sound to a movie. 
[ToDo: add more doc and a demo here.]

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca)
Peter Liscovius

see AUTHORS of ming distribution (ming.sf.net)

=head1 SEE ALSO

SWF::DisplayItem for how you can modify the instances in a SWF::MovieClip or SWF::Movie
SWF, SWF::Action, SWF::InitAction, SWF::Button, SWF::Movie, 
SWF::Shape, SWF::Sound, SWF::SoundStream, SWF::Text, SWF::TextField, SWF::VideoStream

=cut
