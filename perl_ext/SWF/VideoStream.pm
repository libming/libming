# ====================================================================
# Copyright (c) 2006 by Peter Liscovius. All rights reserved.
#           (c) 2009 Albrecht Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::VideoStream;
use SWF();

use strict;

$SWF::VideoStream::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::VideoStream - SWF Video class

=head1 SYNOPSIS

	use SWF::VideoStream;
	$videostream = new SWF::VideoStream("test.flv");

=head1 DESCRIPTION

SWF::VideoStream is a helper class useful for playing videos via SWF applications,
either via embedded video data, or controlled by ActionScript.

=head1 METHODS

=over

=item $videostream = new SWF::VideoStream($filename)

Creates a SWF::VideoStream object. If the file can't be opened the 
constructor will return an empty object. The filename is not limited
to 'flv' extension. 

=item $frames = $videostream->getNumFrames()

This method returns the number of video-frames of an object of SWF::VideoStream,
this works only for embedded streams. In case of error you will get result of -1.

=item $videostream->setDimension(width, height)

This method sets width and height for streamed videos,
this works only on streamed videos (progressive download or rtmp).

=item $bool = $videostream->hasAudio()

A test whether the embedded FLV stream also has audio data.

=item $videostream->setFrameMode($mode)

If the mode == SWFVIDEOSTREAM_MODE_AUTO (default) on every SWF movie frame a video
frame is added. In SWFVIDEOSTREAM_MODE_MANUAL mode, the user needs to call
the nextFrame() method to change the video's frame. This works only with embedded video streams.
Does return the previous mode or -1 if an invalid mode was passed.

=item $result = $videostream->nextFrame()

Switch to next video frame. Works only with embedded video streams. Returns -1 if an error happend.
Here follows some demo code how to use SWF::VideoStream objects (without ActionScript):

	use SWF qw(:ALL); 			# to be lazy
	$movie = new SWF::Movie();
	$movie->setRate( 25 );
	# $movie->setRate( 5 );			# e.g. 5 for slow motion 
	# here movie set background etc. etc.
	#
	$video=new SWF::VideoStream('MyTestVideo.flv');
	die if (-1 == $v->getNumFrames()); 	# abort if something went wrong
	#
	$video->setFrameMode(SWF::Constants::SWFVIDEOSTREAM_MODE_MANUAL);
	$video->seek(1000, 0); 			# for example only
	$displayitem = $movie->add($video);
	#
	# 250 for a 10 seconds movie part (at rate of 25 per minute)
	for(my $n = 0; $n < 250; ++$n)	
	{
  	 $video->nextFrame();
	 $movie->nextFrame();
	}
	$movie->save("MyTestVideo.swf",9);
	# that's all, folks

=item $result = $videostream->seek($frame, $whence)

This functions allows seeking $frame in video stream, returning the old video frame position.
As value of $whence use one of the following:

	0 for seeking from beginning
	1 for seeking from current position
	2 for seeking from end of file

=back

=head1 AUTHOR

	developers of ming 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Action, SWF::Movie, SWF::MovieClip, SWF::Sound, SWF::SoundStream, SWF::Constants

=cut
