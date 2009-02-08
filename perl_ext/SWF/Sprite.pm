# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Sprite;
use SWF();
use strict;

@SWF::Sprite::ISA = qw(SWF::MovieClip);
$SWF::Sprite::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Sprite - Sprite (MovieClip) Class

=head1 SYNOPSIS

	use SWF::Sprite;
	my $sprite = new SWF::Sprite();

=head1 DESCRIPTION

SWF::Sprite allows you to add animated objects to your Fash movies. 
You may use SWF::MovieClip instead of SWF::Sprite (i.e. SWF::Sprite
inherits all it's methods from SWF::MovieClip).
In other words: SWF::Sprite is deprecated! Do NOT use anymore.

=head1 METHODS

=over 4

=item new SWF::Sprite();

Returns an SWF::Sprite object.

=item $displayItem = $sprite->add($s);

Add $s to the $sprite object and returns an SWF::DisplayItem object, i.e. $displayItem . $s may be an SWF::Shape, SWF::Text, SWF::Button, or another sprite object.

=item $sprite->remove($displayItem);

Removes $displayItem from the display list.

=item $sprite->nextFrame();

Move to the next frame of the animation.

=item $sprite->setFrames($i);

Sets total number of $sprite frames to $i

=item $sprite->labelFrame($name);

Sets frame name to $name

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca).

=head1 SEE ALSO

SWF, SWF::MovieClip, SWF::Button, SWF::DisplayItem, SWF::Shape, SWF::Sprite, SWF::Movie
=cut
