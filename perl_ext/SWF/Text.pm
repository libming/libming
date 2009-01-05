# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Text;
use SWF ();

$SWF::Text::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Text - SWF Text class

=head1 SYNOPSIS

	use SWF::Text;
	$text = new SWF::Text();

=head1 DESCRIPTION

Draw simple static texts.

=head1 NOTES:

    None.

=head1 METHODS 

=over

=item new SWF::Text()

Creates a Text object.

=item $text->addString($string)

=item $text->addUTF8String($UTF8string)

=item $text->addWideString($widestring,$widestring_len)

Writes the given $string into this Text object at the current pen position,
using the current font, height, spacing, and color:

	$t->addString("Thai");
	$t->addUTF8String("\x{0E44}\x{0E17}\x{0E22}");
	$t->addWideString("\x{44}\x{0E}\x{17}\x{0E}\x{22}\x{0E}",3);                                                                                                                                                      

=item $text->setColor(red, green, blue [, alpha])

Set the color of the text. An 8 bit value for each parameter. You can use 
he hex or decimal notation. Even mixed.

	$text->setColor(0xcc,0,0x33);
	$text->setColor(204,0,51,255);

=item $text->setFont($font)

Sets an SWF::Font object -$font- to be used in the Text.

=item $text->setHeight($height)

Sets this Text object's current height to given height.

=item $text->moveTo($x, $y)

Move the Text's location to ($x, $y).

=item $text->setSpacing($spacing)

Sets this Text object's current letterspacing to given spacing.

=item $text->getWidth($string)

=item $text->getStringWidth($string)

=item $text->getUTF8StringWidth($UTF8string)

=item $text->getWideStringWidth($widestring)

Returns width of given string in pixels.

=item $text->getAscent()

Returns the ascent of the current font at its current size, or 0 if not available.

=item $text->getDescent()

Returns the descent of the current font at its current size, or 0 if not available.

=item $text->getLeading()

Returns the leading of the current font at its current size, or 0 if not available.

=back

=head1 AUTHOR

developers of ming
ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Font, SWF::TextField, ISO 10646 (Unicode)

=cut

