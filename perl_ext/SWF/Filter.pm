# ====================================================================
# Copyright (c) 2008 Klaus Rechert All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Filter;
use SWF ();

$SWF::Filter::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Filter - SWF filter class

=head1 SYNOPSIS

	use SWF::Blur;
	use SWF::Shadow;
	use SWF::Constants qw(:Filter);
	@color= (12,45,99); 
	$blur = new SWF::Blur(15, 15, 2);
	$shadow = new SWF::Shadow(2.5, 12, 2);
	$filter = SWF::Filter::newDropShadowFilter( \@color, 
	          $blur, $shadow, SWF_FILTER_MODE_KO);
	$displayitem->addFilter( $filter);

=head1 DESCRIPTION

Filter objects are available since player version 8. They can be applied to buttons, movieclips and text instances
by the addFilter() method of SWF::DisplayItem objects. Several types of filters exist. Each of them
has its own method of construction, see below.

=head1 METHODS

=over

=item newDropShadowFilter($colorref, $blur, $shadow, $flags)

Creates a DropShadowFilter object. The object is controlled by color, blur object,
shadow object and flags (SWF_FILTER_MODE_INNER, SWF_FILTER_MODE_KO).

=item newBlurFilter($blur)

Creates a BlurFilter object. The object is controlled by the blur object.

=item newGlowFilter($colorref, $blur, $strength, $flags)

Creates a GlowFilter object, a simple variant of DropShadowFilter.
Use SWF_FILTER_MODE_INNER, SWF_FILTER_MODE_KO as flag values.

=item newBevelFilter($shadow_colorref, $highlight_colorref, $blur, $shadow, $flags)

Just another (complex) variant of DropShadowFilter.
Use SWF_FILTER_MODE_INNER, SWF_FILTER_MODE_KO or
FILTER_MODE_ONTOP as flag values.

=item newGradientGlowFilter($gradient, $blur, $shadow, $flags)

Just another (more complex) variant of DropShadowFilter.
As flag use SWF_FILTER_MODE_INNER, SWF_FILTER_MODE_KO
or SWF_FILTER_MODE_ONTOP .

=item newGradientBevelFilter($gradient, $blur, $shadow, $flags)

Just another (slightly more complex) variant of DropShadowFilter, 
extending newBevelFilter() by using a gradient instead of simple colors.
Use SWF_FILTER_MODE_INNER, SWF_FILTER_MODE_KO or FILTER_MODE_ONTOP 
as flag values.

=item newColorMatrixFilter($filtermatrix)

The filtermatrix is an object of SWF::FilterMatrix class sized 5 columns x 4 rows 
for RGBA values, useful for transformations like color conversion to
greyscale pictures.

=item newConvolutionFilter($filtermatrix, $divisor, $bias, $colorref, $flags)

Another filter for nice picture effects like blur etc. 
The filtermatrix is an object of SWF::FilterMatrix class. 
As flag use either use SWF_FILTER_FLAG_CLAMP or SWF_FILTER_FLAG_PRESERVE_ALPHA.

=back

=head1 AUTHOR

	developers of 
	ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Filter, SWF::Blur, SWF::Shadow, SWG::Gradient, SWF::FilterMatrix, SWF::DisplayItem, SWF::Constants, SWF::MovieClip, SWF::Button

=cut

