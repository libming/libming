# ====================================================================
# Copyright (c) 2006 by Peter Liscovius. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::FontCharacter;
use SWF();

use strict;

$SWF::FontCharacter::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::FontCharacter - SWF Font Helper Class

=head1 SYNOPSIS

	use SWF::FontCharacter;
	$font = new SWF::Font('Arial.fdb');
	$fontcharacter = $movie->addFont($font);
	$fontcharacter = $movie->importFont('textField.swf', 'Arial337a'); 

=head1 DESCRIPTION

SWF::FontCharacter is a font helper class.
The objects are available thru SWF::Movie methods(), 
so no constructor is present here.

=head1 METHODS

=over

=item $fontcharacter->addChars($string)

Adds characters from string.

=item $fontcharacter->addUTF8Chars($string)

Adds UTF8 characters from string and
will throw an error message like 
"Character not found 12345" if any needed glyphs
are not present inside font definition.

=item $fontcharacter->addAllChars()

Adds all characters...
[ToDo: add some demo code here]

=back

=head1 AUTHOR

	developers of ming 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Movie, SWF::Font, SWF::Text, SWF::TextField

=cut
