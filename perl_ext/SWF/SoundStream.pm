# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::SoundStream;
use SWF();

use strict;

$SWF::SoundStream::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::SoundStream - SWF Sound Stream class

=head1 SYNOPSIS

	use SWF::SoundStream;
	my $soundstream = new SWF::SoundStream("wau.mp3"); 
	print $soundstream->getDuration();
	##  print $soundstream->getFrames();	# won't work until it belongs to movie
	$movie->setSoundStream($soundstream);
	print $soundstream->getFrames();	# but _now_ it is okay (but anyway deprecated)

=head1 DESCRIPTION

SWF::SoundStream is a helper class useful for adding sound into SWF applications.

=head1 METHODS

=over

=item $soundstream = new SWF::SoundStream($filename)

Creates a SWF::SoundStream object. If the file can't be opened the 
constructor will return an undef value of course. The filename is
the valid name of any mp3 or flv file.

=item $dura = $soundstream->getDuration()

This function returns the duration of a given stream in ms.
Works for streams of mp3, flv types only. On other cases it returns 0.

=item $frames = $soundstream->getFrames()

Using getFrames() is deprecated, better use getDuration(). getFrames() returns the
number of movie frames necessary to play the full sound stream. For this reason
it works only if the sound stream object was added to a movie, and also only for 
streams of mp3 -files.

=back

=head1 AUTHOR

	developers of ming.sourceforge.net, Albrecht Kleine

=head1 SEE ALSO

SWF, SWF:: Movie, SWF::MovieClip, SWF::Button, SWF::Sound, SWF::SoundInstance, SWF::Constants,
SWF::Videostream

=cut
