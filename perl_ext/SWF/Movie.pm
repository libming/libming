# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Id$

package SWF::Movie;
use SWF();

$SWF::Movie::VERSION = $SWF::VERSION;

use strict;

sub streamMp3{
    my ($movie,$filename) = @_;
    $movie->setSoundStream(new SWF::SoundStream($filename));
}

sub output{
    #/xxx/ Should we allow use to specify???
    my $output_method = \&xs_output;
    $output_method = \&Apache::SWF::xs_output
	if ( $INC{'Apache/SWF.pm'} && 
	     substr($ENV{GATEWAY_INTERFACE}||'',0,8) eq 'CGI-Perl');
    return &$output_method(@_);
}

1;

__END__

=head1 NAME

SWF::Movie - SWF Movie class

=head1 SYNOPSIS

	use SWF::Movie;

	$m = new SWF::Movie();
	$m = SWF::Movie::newSWFMovieWithVersion(8);

=head1 DESCRIPTION

SWF::Movie is the root object of your flashfile. It contains all your other objects you want in your movie.
Ming supports many Flash4, Flash5 F6,7,8,9.. features.

The SWF::Movie object contains information like the flashversion, the size, the frames per second, the background color. Except that, the SWF::Movie behaves like 
a SWF::MovieClip. It only cannot be added to another SWF::Movie or SWF::MovieClip.

Notice: The version of your flashmovie can be set by the object SWF. See SWF for more details.

=head1 METHODS

=over

=item $m = new SWF::Movie()

Creates a Movie object. 

=item $m = newSWFMovieWithVersion(version)

Creates a Movie object with version value (e.g. 8) set.

=item $displayItem=$m->add($SWFObject)

Add an instance of a SWF-object to your current frame. The returnvalue is a SWF::DisplayItem.

	Some possible SWF-objects are:

	SWF::Action
	SWF::MovieClip
	SWF::Shape
	SWF::Sound
	SWF::Text
	SWF::TextField
	SWF::Bitmap
	SWF::PrebuiltClip

	$m->add($shape); # possible, but not more accessible for us
	$shape_1 = m->add($shape); # good instance 

=item $m->addExport($block, $name)

Prepare linkage to Action script code.

	$m->addExport($mc, 'mc');
	# and later inside action script....:
	$ac = new SWF::Action("_root.attachMovie('mc','mc1',11);");

=item $m->writeExports()

Export prepared names to Action script.

=item remove($displayItem)

Removes the given SWF::DisplayItem instance from the current and following frames of the main timeline. 

	$m->remove($shape_i);

=item nextFrame()

Finish a frame of the main timeline. New added objects will be added in the next frame.

	$m->nextFrame();

=item setNumberOfFrames($frames)
=item setFrames($frames)

Preset the number of frames of the main timeline. n not used frames before finishing the movie will filled with n nextFrame() statements.
If you inserted more nextFrame() commands than you set with setFrames() the setFrames() will be ignored. 

	$m->setFrames(100);

=item $m->setBackground($red,$green,$blue)

Set the background in red, green, blue values. Possible value are 0..255 or hexvalues 0x0..0xff.

	$m-setBackground(0xff,0x99,0x00); # orange bg
	$m-setBackground(0,200,255)       # green bg

A transparency level isn't available I asume. (In Flash MX/F6?). A transparent background can be set in the HTML-code for Internet Explorer. Please 
inform the Authors if that is available for nonIE-browsers.

=item $m->namedAnchor($label)

This function adds a named anchor to the movie.

=item $m->setDimension(x,y)

Set the dimension in pixel.

	#ToDo:
	#min: (0.0,0.0)   # ;-)
	#max: (?,?)
	#default: (0.0?,0.0?)
	#Behavior when values <min: ming sets it to min
	#Behavior when values >max: ming sets it to max

	$m->setDimension(200,100);
	$m->setDimension(123.45,67.89);

=item $m->setRate(int)

Set the playing speed in frames/second. 

	#ToDo:
	#min: 0
	#max: ?
	#default: ?
	#Behavior when value <min: ming sets it to min
	#Behavior when value >max: ming sets it to max

	$m->setRate(30);

=item $m->protect()

A feature that prevents only flashnewbies from importing your movie into Macromedias Flash authoring tools. It cannot protect your content from being loaded by 
other flashmovies, deleting of the protection with a hexeditor or disassembling with tools. 

=item $m->setSoundStream($soundstream, [$skip])

Puts an SWF::SoundStream object as streaming sound into the main timeline.
The optional skip parameter describes the time in seconds to be skiped.

	$m->setSoundStream($soundstream);

=item $m->streamMp3($filename)

Simplifies the setSoundStream(). You only need the filename. 

	$m->streamMp3("iloveyou.mp3");

Hint: If you want to reuse this sound/mp3 later in this movie, create a reusable SWF::SoundStream object and then put it into setSoundstream().

=item $soundinstance = $m->startSound($sound)

Play a prepared SWF::Sound object into the main timeline. Returns an object of SWF::SoundInstance.

	$sound = new SWF::Sound("loveyoutoo.mp3", SWF_SOUND_MP3_COMPRESSED );  
	$si = $m->startSound($sound);

=item $m->stopSound($sound)

Stop playing sound.

=item $m->labelFrame($string)

Gives the current frame a label that can be used by actionscript as anchor. Then you don't have to count your nextFrame() lines anymore. :-)

	$m->labelFrame("win");
	#ToDo:
	#max characters of a label??? 
	#What happen if we put there a big string?? 
	#Which characters are allowed??

=item $m->setScriptLimits($maxRecursion, $timeout)

Set limits for resource usage, related to action script. Setting timeout will protect your computer from too long loops
and stack overflow. So endless scripts like "while (1){};" will be interrupted after timeout seconds.  The default 
recursion depth is 265, the default timeout is 15 seconds.

=item $m->defineScene($offset, $name)

Define scenes for the movies' main timeline using integer value for frame offset and name of the scene.

=item $m->setNetworkAccess($flag)

Sets network / fileaccess policy. If the flag is set to 0 a localy loaded 
movie will be unable to access the network but is allowed to access local files.
If the flag ist set to 1 a localy loaded movie will be unable to access local files
but is allowed to access the network. For SWF >= 8: default is 0,

=item $m->addMetadata($xml)

Adds Metadata to the movie: this function inserts a metadata-tag
into the movie. Only one metadata-tag can be set to a movie. 
Metadata is specified as an XML string. 

=item $m->assignSymbol($character, $name)

Assigns a symbolic name for a SWF::Character object.
Such classes are available for usage in other SWF files
and can be referenced from inside the current movie.
To assign a symbol to the root movie use NULL as character value.

=item $m->setTabIndex($depth, $index)

Sets tab index for specified depth level.

=item $fontchar = $m->addFont($font)

Adds a font to a movie. An object of SWF::FontCharacter class returned.

	$font = new SWF::Font('Arial.fdb');                                                                                                                                                                              
	$fch  = $m->addFont($font);      

=item $fontchar = $m->importFont($file, $name)

Imports a font from an other SWFFile. An object of SWF::FontCharacter class returned.

	$fch = $m->importFont('textField.swf', 'Arial337a'); 

=item $m->importCharacter($url, $name)

Not yet documented (ToDo!)

=item $result = $m->replace($item, $block)

This method replaces a displayable character with a new one.
Returns 0 on success , -1 else.

	$dispitem = $movie->add($shape1);
	$movie->nextFrame();
	print $movie->replace($dispitem, $shape2);
	$movie->nextFrame();

=item $written = $m->xs_output([$compresslevel])

=item $written = $m->output([$compresslevel])

Writes the SWF::Movie to stdout. This is normally used for webservers/cgi-scripts. 
Don't forget to send the "Content-Type:"-lines before writing out the movie.

The parameter compresslevel is optional. compresslevel between 0 and 9. Higher values will be interpreted as 9. Values smaller 0
mean no compression. By default the SWF::Movie will be send without compression. 
Notice: compression available since Flash MX(Flash 6). Ming does not check this for you if you write flash4 or flash5 movies.

Unlike xs_output() the output() method prepared for using via webservers (but not limited to webservers).

Both methods return number of bytes written.

=item $written = $m->save($filename [, $compresslevel]) 

Method saves the SWF::Movie to a file and returns the number of bytes written.
The parameter compresslevel is optional. compresslevel between 0 and 9. Higher values will be interpreted as 9. Values smaller 0 
mean no compression. By default the SWF::Movie will be saved without compression. 
Notice: compression available since Flash MX(Flash 6). Ming does not check this for you if you write flash4 or flash5 movies.

=back

=head1 EXAMPLE

3 examples: a regular minimal SWF::Movie, one with all methods and a more interesting.

=over

=item minimal empty example

	#!/usr/bin/perl -w
	use SWF::Movie;

	$m = new SWF::Movie();
	$m->nextFrame();
	$m->save("emptyminimal.swf");

=item full empty examples

	#!/usr/bin/perl -w
	use SWF::Movie;

	$m = new SWF::Movie();
	$m->setVersion(4);
	$m->setBackground(0xff,0xcc,0x0);
	$m->setDimension(200,100);
	$m->protect();
	$m->setRate(30);
	$m->setFrames(10); # ming fills it automatic with frames

	$m->nextFrame()
	$m->save("empty.swf");

=item streaming example

	#!/usr/bin/perl -w
	use SWF("Shape");
	SWF::setVersion(6);

	$m = new SWF::Movie();
	$m->setBackground(0xff,0xcc,0x0);
	$m->setDimension(200,100);
	$m->setFrames(10);
	$m->streamMp3("forever.mp3");

	$s= new SWF::Shape();
	$s->setLine(40,0xff,0xff,0);
	$s->setRightFill(0xff,0,0); 
	$s->drawLine(50,0);
	$s->drawLine(0,20);
	$s->drawLineTo(0,0);

	$s_1= $m->add($s);
	$s_2= $m->add($s);
	$s_1->move(50,40);
	$s_2->move(10,40);
	for($i=1;$i<5;$i++) {
		$m->nextFrame();
		$s_2->move(0,10);
	}
	$m->nextFrame();
	print "Content-type: application/x-shockwave-flash\n\n";
	$m->output(9); # with compression level 9 (since flash6)

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net), Albrecht Kleine and
developers of ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Action, SWF::Bitmap, SWF::Button, SWF::Constants, SWF::DisplayItem, SWF::Fill, 
SWF::Font, SWF::FontCharacter, SWF::Gradient, SWF::Morph, SWF::MovieClip, SWF::Shape, 
SWF::PrebuiltClip, SWF::Sound, SWF::SoundStream, SWF::TextField, SWF::Text, SWF::Filter, SWF::Character

=cut
