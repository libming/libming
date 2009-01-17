# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::FilterMatrix;
use SWF ();

$SWF::FilterMatrix::VERSION = $SWF::VERSION;
1;

__END__

=head1 NAME

SWF::FilterMatrix - SWF FilterMatrix  class

=head1 SYNOPSIS

	use SWF::FilterMatrix;
	$valueref     = [ 0.5, 0.4, 0.3, 0, 0, 
	                  0.5, 0.4, 0.3, 0, 0, 
	                  0.5, 0.4, 0.3, 0, 0, 
	                  0, 0, 0, 1, 0 ];
	$filtermatrix = new SWF::FilterMatrix( 5, 4, $valueref );
	$filter    = SWF::Filter::newColorMatrixFilter( $filtermatrix );

=head1 DESCRIPTION

SWF::FilterMatrix is a helper class useful for construction 
of some SWF::Filter objects: ColorMatrix filter and Convolution filter.

=head1 METHODS

=over

=item new SWF::FilterMatrix($cols, $rows, $valueref)

The only method is a constructor for creation of 
a FilterMatrix object. The object is controlled by
the matrix data dimensions (cols * rows) and the 
data values: a reference to an array of float values.
Note: constructing ColorMatrixFilter needs a 5*4 matrix.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Filter, SWF::Blur, SWF::Shadow

=cut
