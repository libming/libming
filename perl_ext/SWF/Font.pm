# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Font;
use SWF();

$SWF::Font::VERSION = $SWF::VERSION;


1;

=head1 NAME

SWF::Font - SWF Font class

=head1 SYNOPSIS

	use SWF::Font;
	$font = new SWF::Font();

=head1 DESCRIPTION

Font object for later usage in Text and TextField objects.

=head1 NOTES

Ming 0.3's old getShape($font, $code) has been removed from interface.
For getting details of glyph information you could draw a character 
into a shape and call dumpOutline() method, just like

	$sh=new SWF::Shape();
	$sh->drawGlyph($font, $char_nr);
	print $sh->dumpOutline();

=head1 METHODS 

=over

=item new SWF::Font($filename)

Creates a Font object. $filename selects either True-Type Font '(ttf')
file or Font Definition Block ('fdb') file as written by makefdb tool.

=item $font->getStringWidth($string)

=item $font->getUTF8StringWidth($UTF8string)

Returns width of given string in pixels.

=item $font->getAscent()

Returns the ascent of the current font, or 0 if not available.

=item $font->getDescent()

Returns the descent of the current font, or 0 if not available.

=item $font->getLeading()

Returns the leading of the current font, or 0 if not available.

=item $font->getName()

Returns the name of the current font, do not confuse with font filename.

=item $font->getGlyphCount()

Returns the glyph count of the current font.

=back

=head1 AUTHOR

developers of ming
ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::BrowserFont, SWF::Text, SWF::TextField, SWF::Shape, ISO 10646 (Unicode)

=cut
