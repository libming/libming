# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
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
 my $sprite = new SWF::MovieClip();

=head1 DESCRIPTION

SWF::MovieClip allows you to add animated objects to your Fash movies.


=head1 METHODS

=over 4

=item $mc= new SWF::MovieClip();

Returns an SWF::MovieClip object.

=item add($swfobject);

$di = $mc->add($swfobject);

When you add following types of objects to the $mc they will return a SWF::DisplayItem:

SWF::Button
SWF::PrebuiltClip (a whole external swf file)
SWF::MovieClip (you can nest them like a tree)
SWF::Shape
SWF::Text
SWF::TextField
SWF::VideoStream

When you add a SWF::Sound the return value is a SWF::SoundInstance:

$si = $mc->add($sound);

=item $mc->remove($di);

Removes SWF::DisplayItem $di from the display list.

=item $mc->nextFrame();

Move to the next frame in the timeline of SWF::MovieClip $mc

=item $mc->setFrames($i);

Sets total number of $mc frames to $i
This is an optional autofiller, e.g. when you want to be sure that 2 different movieclips
have 100 frames when playing, but the number of nextFrame() calls in these 2 movieclips is unknown or
dynamic (dependent on data from databases, whatever)

By default a movieclip the number of frames
in the timeline is how often you called $mc->nextFrame
for this clip.

=item $mc->labelFrame($name);

Sets frame name to $name

You are then able to access this frame by name in ActionScript, not just by frame number.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca)
Peter Liscovius

see AUTHORS of ming distribution (ming.sf.net)

=head1 SEE ALSO

SWF::DisplayItem for how you can modify the instances in a SWF::MovieClip or SWF::Movie

SWF, SWF::Action, SWF::Button, SWF::Movie, SWF::Shape, SWF::Sound, SWF::Text, SWF::TextField, SWF::VideoStream

=cut
