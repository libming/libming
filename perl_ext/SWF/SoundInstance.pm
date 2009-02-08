# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::SoundInstance;
use SWF();

use strict;

$SWF::SoundInstance::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::SoundInstance - SWF Sound Instance class

=head1 SYNOPSIS

	use SWF::Sound;
	use SWF::SoundInstance;
	$sound = new SWF::Sound("test.mp3");
	$snd_instance = $movie->startSound($sound);
	$snd_instance->loopInPoint(4000);
	$snd_instance->loopOutPoint(8000);
	$snd_instance->loopCount(2);

=head1 DESCRIPTION

SWF::SoundStream is a helper class useful for setting information
about a sound instance. There is no constructor method,
SWF::SoundStream objects are available only thru:

=over

=item startSound() method of SWF::Movie,

=item startSound() method of SWF::MovieClip,

=item addSound()   method of SWF::Button.

=back

=head1 METHODS

=over

=item $snd_instance->noMultiple()

Prohibits starting sound if sound is already playing.

=item $snd_instance->loopInPoint($point)

Sets the loop start $point counted in samples.

=item $snd_instance->loopOutPoint($point)

Sets the loop's last sample to play.

=item $snd_instance->loopCount($count)

Sets loop count. The default value is 1 loop.

=item $snd_instance->addEnvelope($mark44, $left, $right)

Adds a SoundEnvelope to event sound. Parameters are $mark44, the  Position in 44khz samples,
and volumes both for left and right stereo channel.
Next example will mute the right channel:

	$snd_instance->addEnvelope(0,10000,0);

=back

=head1 AUTHOR

developers of ming.sourceforge.net, Albrecht Kleine

=head1 SEE ALSO

SWF, SWF:: Movie, SWF::MovieClip, SWF::Button, SWF::Sound, SWF::SoundStream, SWF::Constants
=cut
