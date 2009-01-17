# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Shadow;
use SWF ();

$SWF::Shadow::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Shadow - SWF shadow class

=head1 SYNOPSIS

	use SWF::Shadow;
	$shadow = new SWF::Shadow(2.5, 12, 2);
	$filter = SWF::Filter::newDropShadowFilter( \@colors, $blur, $shadow, 0);

=head1 DESCRIPTION

SWF::Shadow is a helper class useful for SWF::Filter.

=head1 METHODS

=over

=item new SWF::Shadow($angle, $distance, $strength)

Creates a Shadow object. The object is controlled by
angle (in radians), distance (in px) and strength.
Useful values for strength are between 1 and 10. 
Use the created object as parameter for construction of some of the 
SWF::Filter objects.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Filter, SWF::Blur, SWF::DisplayItem

=cut
