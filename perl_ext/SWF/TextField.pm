# ====================================================================
# Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
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
	$textfield = new SWF::TextField([$flags]);



=head1 DESCRIPTION

Unlike SWF::Text objects, TextFields cannot be rotated, scaled non-proportionally, or skewed. However, TextFields can be used as form entries and they can use browser-defined fonts. 

=head1 NOTES:

    TextField is implemented into Flash since Flash3. 
    Most of features for TextFields are available since Flash4.
    Simple HTML-tags in TextField are possible since Flash5.

=head1 METHODS 

=over

=item new SWF::TextField([$flags]);

Creates a TextField object whose behaviour is dictated by C<flags> (see also SWF::Constants): 

  SWFTEXTFIELD_NOEDIT:        Non-editable Textfiled
  SWFTEXTFIELD_PASSWORD:      Obscure user input with astricts.
  SWFTEXTFIELD_DRAWBOX:       Draw a border around TextField
  SWFTEXTFIELD_MULTILINE:     TextFiels may contain multiple lines
  SWFTEXTFIELD_WORDWRAP:      Warp text when text reaches TextField border
  SWFTEXTFIELD_NOSELECT:      TextField is not selected when user clicks on it
  SWFTEXTFIELD_ALIGN_LEFT:    Align text to the left
  SWFTEXTFIELD_ALIGN_RIGHT:   Align text to the right
  SWFTEXTFIELD_ALIGN_CENTER:  Align text to the center
  SWFTEXTFIELD_ALIGN_JUSTIFY: Justify text
  SWFTEXTFIELD_HTML:          Add HTML markup string
  SWFTEXTFIELD_HASLENGTH:     
  SWFTEXTFIELD_USEFONT:       Want to embed font
  SWFTEXTFIELD_AUTOSIZE:


Flags may be combined with the bitwise OR operation. For example:

    my $t = new SWF::TextField(SWFTEXTFIELD_MULTILINE | SWFTEXTFIELD_WORDWRAP);


=item $textfield->addString($string)

=item $textfield->addUTF8String($UTF8string)

Add $string to TextField. Text is appended to the existing text.

=item $textfield->align($alignment)

=item $textfield->setAlignment($alignment)

Set the alignment of the text in the textfield. Possible values are

	SWFTEXTFIELD_ALIGN_LEFT
	SWFTEXTFIELD_ALIGN_RIGHT
	SWFTEXTFIELD_ALIGN_CENTER
	SWFTEXTFIELD_ALIGN_JUSTIFY

By default a SWF::TextField ist left aligned.

=item $textfield->setBounds($width, $height)

Sets the width and height of the textfield.

=item $textfield->setColor($red, $green, $blue [, $alpha])

Set the color of the text. An 8 bit value for each parameter. You can use 
the hex or decimal notation. Even mixed.

	$tf->setColor(0xcc,0,0x33);
	$tf->setColor(204,0,51,255);

=item $textfield->setFont($font)

Sets an SWF::Font object -$font- to be used in the TextField.

=item $textfield->setPadding($padding)

Set TextField padding.

=item $textfield->setHeight($height)

Set the height of font in your textfield.

=item $textfield->setIndentation($indentation)

Set the indentation of the first line of a paragraph.

=item $textfield->setLeftMargin($left)

Sets the left margin of the textfield.

=item $textfield->setRightMargin($right)

Sets the right margin of the textfield.

=item $textfield->setMargins($left, $right)

Sets left and right margins of $textfield.

=item $textfield->setLineSpacing($linespacing)

Sets the space between lines. 

=item $textfield->setName($name)

=item $textfield->setVariableName($name)

Assigns a name to the TextField which could be used to reference the TextField within ActionScript.

=item $textfield->addChars($string)

=item $textfield->addUTF8Chars($UTF8string)

Adds characters to a font that will be available within a textfield.

=item $textfield->setFlags($flags)

Set TextField behaviour by C<flags> (see above new SWF::TextField) For example:

	$textfield->setFlags(SWFTEXTFIELD_PASSWORD); 

=item $textfield->setLength($length)

Sets limit for user input size to $length.

=item $textfield->setFieldHeight($height)

Sets height limit for visible part of text field. If $height is set to 0, the height is calculated by used font, see I<setHeight()> method.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil at users.sourceforge.net).

=head1 SEE ALSO

SWF, SWF::Action, SWF::Button, SWF::Constants, SWF::DisplayItem, SWF::Font, SWF::Movie, SWF::MovieCip, SWF::Text

=cut

