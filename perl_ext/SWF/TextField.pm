# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::TextField;
use SWF ();

$SWF::TextField::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::TextField - SWF TextField class

=head1 SYNOPSIS

	use SWF::TextField;

	use SWF::TextField(FLAG | FLAG | ..);

=head1 DESCRIPTION

This is a textfield-object for flash movies.

A TextField can contain a fixed initial string, show values of 
ActionScript-variables and can act as inputfield.

The behavior of a TextField is quite good configurable through setting
the SWFTEXTFIELD-flags. The layout is configurable with the set-methods.

TextField is inplemented into Flash since Flash3. 
Most of features for TextFields are available since Flash4.
Simple HTML-tags in TextField are possible since Flash5.

Remember that Flash uses by default browserfonts.

Add the needed characters explicit into your movie. (experimental feature)

If you use "_sans.fdb" you are safe, because this tells the flashplayer to 
use a Helvetica or Arial-stylish font on every OS. By default a 
times-new-roman-stylish font will be used. (think I)

Oct.2002: There are currently different versions of ming with different 
supportlevel for textfield out there. Hope this will converge soon.

Widefont stuff is not documented for now. 

All values are in pixels here. (default scale)
Currently they will be rounded to integer pixels, but i think this 
pixel/twips-switching should be fixed/done completly in the ming-lib and 
the user should never think about it again. 

=head1 FLAGS

see SWF::Constants

=head1 METHODS 

=over

=item addString($string)

Set an initial string.

	$tf->addString("blablabla"); 


Notice: ming-API currently(CVS Oct.2002) in unstable definition. Maybe 
addUTF8() or addWideString(). Try what works in your version of ming.

=item	setAlignment(SWF::Constants)

Set the alignment of the text in the textfield. Possible values are

	SWFTEXTFIELD_ALIGN_LEFT
	SWFTEXTFIELD_ALIGN_RIGHT
	SWFTEXTFIELD_ALIGN_CENTER
	SWFTEXTFIELD_ALIGN_JUSTIFY
	
By default a SWF::TextField ist left aligned.
	
	$tf->setAlignment(SWFTEXTFIELD_RIGHT);

=item	setBounds(width,height)

Set the width and height of the textfield.
	
	$tf->setBounds(50,40);

=item setColor(red,green,blue,[alpha])

Set the color of the text. An 8 bit value for each parameter. You can use 
the hex or decimal notation. Even mixed.
	
	$tf->setColor(0xcc,0,0x33);
	$tf->setColor(204,0,51,255);

=item setFieldHeight(height)

Set the field height? Combine it with $f->getHeight();

=item setFont(SWF::Font)

By default, the defaultfont of the client will be used.

Please put all possible characters into the movie. (experimental)
use method addChars(); not in official version!

Example: If you have a textfield that shows the current money on 
your account you should add "+-0123456789,. $EUR". Don't forget the 
spacecharacter :-). 

	$tf->setFont($font);

=item setHeight(height)

Set the height of font in your texfield in pixels.

	$tf->setHeight(14);

=item setIndentation(indentation)

Set the indentation of the first line of a paragraph. Makes only sense for 
multiline.

	$tf->setIndentation(2.0);

=item setLeftMargin(leftmargin)

Set the left margin of the textfield.

	$tf->setLeftMargin(2.0);

=item setRightMargin(rightmargin)

Set the right margin of the textfield.

        $tf->setRightMargin(2.0);

=item setLineSpacing(linespacing)

Sets the space between lines. Makes only sense for multiline.

	$tf->setLineSpacing(2.5);

=item setName($string)

$string is the name of a variable.
	
	$tf->setName('var1');
	$tf->setName('_root.totalFrames');

	
	#getfunctions not implemented for now:
	#getBounds(width,height)
        #getHeight(height)
        #getFieldHeight(height)
        #getLeftMargin(leftmargin)
        #getRightMargin(rightmargin)
        #getIndentation(indentation)
        #getLineSpacing(linespacing)	

=back

=head1 EXAMPLE

Two textfields. First shows a changing Actionscript-variable, in the 
second we put a Perl-variable as string into the textfield.
	
	#!/usr/bin/perl -w
	use SWF("Text");
	use SWF::Constants(":Text");
	use English; # $OSNAME instead of cryptic $^O 
	 
	$m = new SWF::Movie();
	$m->setDimension(200,100);
	 
	$f = new SWF::Font("_sans.fdb"); #good, _sans is a good default font. 
	 
	$tf1 = new SWF::TextField(SWFTEXTFIELD_DRAWBOX | SWFTEXTFIELD_WORDWRAP);
	$tf1->setFont($f);
	$tf1->addString("whoaw - a string. :-)"); # here useless but possible
	$tf1->setHeight(14); # font size, default value??
	$tf1->setBounds(70,20); # default values??
	$tf1->setName("_root.currentFrame"); # Actionscript variable	
	 
	tf2 = new SWF::TextField(SWFTEXTFIELD_DRAWBOX | SWFTEXTFIELD_NOEDIT);
	$tf2->setFont($f);
	$tf2->setHeight(11.25);
	$tf2->setBounds(50,14);
	$tf2->addString($OSNAME); # We put the systems name into the field.
	 
	# We put our 2 textfields in the movie.
	$tf1_i= $m->add($tf1);
	$tf2_i= $m->add($tf2);
	$tf1_i->move(20,20);
	$tf2_i->move(20,50);
	 
	# make 10 frame
	for ($i=1;$i<=10;$i++){
		$m->nextFrame();
	}
	$m->save("textfield.swf");

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net)

Documentation: Peter Liscovius (peterdd at users.sourceforge.net)

=head1 SEE ALSO

	SWF::Constants, SWF::Font, SWF::Action

=cut
