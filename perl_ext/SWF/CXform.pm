# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::CXform;
use SWF ();

$SWF::CXform::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::CXform - SWF CXform class

=head1 SYNOPSIS

	use SWF::CXform;
	$cxf = new SWF::CXform();
	$cxf->setColorAdd(100, 200, 30.5 , -50);
	$dispitem->setCXform($cxf); 

=head1 DESCRIPTION

SWF::CXform is a helper class useful for color transformations
on SWF::DisplayItem objects.

=head1 METHODS

=over

=item new SWF::CXform( [ $rAdd ], [ $gAdd ], [ $bAdd ], [ $aAdd ], [ $rMult ], [ $gMult ], [ $bMult ], [$aMult] )

Creates a CXform object. The object is controlled by a parameter list of RGBA values
both for addition and multiplication. All of them are optional, but could be set later
via setColorAdd() and setColorMult() methods. Default values for the xAdd parameters are 0,
and for xMult defaults are 1. Use the created object as parameter for transformations on 
SWF::DisplayItem objects. 

=item setColorAdd($rAdd, $gAdd, $bAdd, $aAdd)

Set addition values of a CXform object.

=item setColorMult($rMult, $gMult, $bMult, $aMult)

Set multiplication values of a CXform object.

=back

=head1 AUTHOR

	developers of
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::DisplayItem, SWF::Filter

=cut
