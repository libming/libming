# ====================================================================
# Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
#           (c) 2009 A.Kleine
# This program is free software; you can redistribute it and/or modify
# it under the same terms as Perl itself.
# ====================================================================

# $Author$
# $Id$

package SWF::Bitmap;
use SWF();

$SWF::Bitmap::VERSION = $SWF::VERSION;

1;

__END__

=head1 NAME

SWF::Bitmap - Bitmap class

=head1 SYNOPSIS

	use SWF::Bitmap;
	$bitmap = new SWF::Bitmap('filename');
	$fill = $shape->addBitmapFill($bitmap);       

=head1 DESCRIPTION

SWF::Bitmap enables you to add JPG and DBL files.
It is a helper class most useful for filling shapes 
(i.e. SWF::Shape objects).
DBL files ('Define Bitmap Lossless') are produced from
PNG or GIF files by using png2dbl or gif2dbl tools.

=head1 METHODS

=over 4

=item new SWF::Bitmap($file, [$alpha])

Creates a new SWF::Bitmap object using 'file'. The file extension 
has to be one of ".jpg",".jpeg" or ".dbl" (not case sensitive). 
In all other cases use either newSWFDBLBitmap() or newSWFJpeg...()
constructors. The alpha parameter represents a filename for opacity
data ('alpha' channel), but only for JPG files.  Alpha data files
are produced by tool gif2mask. The original GIF used for gif2mask input
file containing alpha information should have same dimensions (width x height)
as the JPG file.

=item newSWFDBLBitmap SWF::Bitmap($file)

Creates a new SWF::Bitmap object using DBL file. 
Use this constructor if you have a file with DBL data, 
but not with .dbl filename extension.

=item newSWFJpegBitmap SWF::Bitmap($file)

Creates a new SWF::Bitmap object using JPG file. 
Use this constructor if you have a file with JPG data, 
but not with .jpg or .jpeg filename extension.

=item newSWFJpegWithAlpha SWF::Bitmap($file,$alpha)

Creates a new SWF::Bitmap object using JPG file similar newSWFJpegBitmap()
with additional opacity information in 'alpha' file. Use this constructor
if you have a file with JPG data, but not with .jpg or .jpeg filename extension
and if there is opacity information available in 'alpha' file.  
Alpha data files are produced by gif2mask tool. The original GIF file
containing alpha information used for gif2mask input should have same dimensions
as the JPG file.

=item $width = $bitmap->getWidth();

Returns width of bitmap in pixels.

=item $height = $bitmap->getHeight();

Returns height of bitmap in pixels.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca) and
developers of ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Fill, SWF::Shape, SWF::Button, SWF::Constants, png2dbl, gif2dbl, gif2mask

=cut
