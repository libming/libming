# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Sound;
use SWF();

use strict;

$SWF::Sound::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Sound - SWF Sound class

=head1 SYNOPSIS

	use SWF::Sound;
	$sound = new SWF::Sound("test.mp3");
	# also via SWF::SoundStream:
	$sstr  = new SWF::SoundStream("test.mp3"); 
	$sound = new SWF::Sound($sstr);
	# 
	$movie->startSound($sound);

=head1 DESCRIPTION

SWF::Sound is a helper class useful for making noise of every kind...

=head1 METHODS

=over

=item $sound = new SWF::Sound($filename, [$flags])

=item $sound = new SWF::Sound($soundstream)

Both types of constructor method are creating a Sound object. The first one uses 
a filename (e.g. of mp3 file), the second one is operating on an already created SWF::SoundStream 
object. The first constructor type also accepts flags for file description, SWF_SOUND_MP3_COMPRESSED
is one of them. More flags are part of SWF::Constant class. The created object can be used for 
example as parameter in SWF::Movie's startSound() method.

=back

=head1 AUTHOR

	developers of ming.sourceforge.net, Albrecht Kleine

=head1 SEE ALSO

SWF, SWF:: Movie, SWF::MovieClip, SWF::Button, SWF::SoundInstance, SWF::SoundStream, SWF::Constants

=cut
