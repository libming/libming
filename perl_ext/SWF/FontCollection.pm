# ====================================================================
# Copyright (c) 2008 by Klaus Rechert. All rights reserved.
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::FontCollection;

use SWF();

$SWF::FontCollection::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::FontCollection - SWF font collection class

=head1 SYNOPSIS

	use SWF::SWF::FontCollection;
	$fontcollection = new SWF::FontCollection("fonts.ttc");

=head1 DESCRIPTION

SWF::FontCollection is an experimental helper class useful for font utilities.

=head1 METHODS

=over

=item $fontcollection = new SWF::FontCollection($filename)

Creates a SWF::FontCollection object. 

=item $count = $fontcollection->getFontCount()

Returns font count. 

=item $font = $fontcollection->getFont( $index )

Returns SWF::Font object of number $index.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Font

=cut
