# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Matrix;
use SWF ();

$SWF::Matrix::VERSION = $SWF::VERSION;
1;

__END__

=head1 NAME

SWF::Matrix - SWF matrix class

=head1 SYNOPSIS

	use SWF::Matrix;
	$matrix = $displayItem->getMatrix();
	$scaleX = $matrix->getScaleX();

=head1 DESCRIPTION

SWF::Matrix is a helper class useful for getting information
about a display items's matrix. There is no constructor method,
SWF::Matrix objects are available only thru SWF::DisplayItem's 
getMatrix() method. The methods below give you read access
to the six matrix elements.

=head1 METHODS

=over 4

=item $scaleX = $matrix->getScaleX()

Returns X scale.

=item $scaleY = $matrix->getScaleY()

Returns Y scale.

=item $rot0 = $matrix->getRotate0()

Returns rotation value 0.

=item $rot1 = $matrix->getRotate1()

Returns rotation value 1.

=item $trX = $matrix->getTranslateX()

Returns X translation value.

=item $trY = $matrix->getTranslateY()

Returns Y translation value.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::DisplayItem

=cut
