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

SWF::Bitmap enables you to add JPG, PNG, GIF and DBL files.
It is a helper class most useful for filling shapes 
(i.e. SWF::Shape objects).
DBL files ('Define Bitmap Lossless') may be produced from
PNG or GIF files by using png2dbl or gif2dbl tools.

=head1 METHODS

=over 4

=item $bitmap = new SWF::Bitmap($filename, [$alpha])

Creates a new SWF::Bitmap object using 'filename'. 
The alpha parameter represents a filename for opacity
data ('alpha' channel), but only for JPG files.  Alpha data files
are produced by tool gif2mask. The original GIF used for gif2mask input
file containing alpha information should have same dimensions (width x height)
as the JPG file.

=item $bitmap = newBitmapFromFileHandle($filehandle)

Creates a new SWF::Bitmap object using an open file handle $filehandle .
This way you could use STDIN, piped data etc. etc. for input.
A simple example:

	open (PICT,'flowers.jpg') or die;
	$bmp = newBitmapFromFileHandle SWF::Bitmap(PICT);
	# .....
	$m->save("my_flowers.swf");
	close (PICT);
	# Please note: do not close the PICT file too early.

=item $bitmap = newSWFDBLBitmap SWF::Bitmap($filename)

Creates a new SWF::Bitmap object using DBL file. 
Use this constructor if you have a file with DBL data, 
but not with .dbl filename extension.

=item $bitmap = newSWFJpegBitmap SWF::Bitmap($filename)

Creates a new SWF::Bitmap object using JPG file. 
Use this constructor if you have a file with JPG data, 
but not with .jpg or .jpeg filename extension.

=item $bitmap = newSWFJpegWithAlpha SWF::Bitmap($filename,$alpha_filename)

Creates a new SWF::Bitmap object using JPG file similar newSWFJpegBitmap()
with additional opacity information in 'alpha' file. Use this constructor
if you have a file with JPG data, but not with .jpg or .jpeg filename extension
and if there is opacity information available in 'alpha_filename' file.  
Alpha data files are produced by gif2mask tool. The original GIF file
containing alpha information used for gif2mask input should have same dimensions
as the JPG file.

=item $width = $bitmap->getWidth();

Returns width of bitmap in pixels.

=item $height = $bitmap->getHeight();

Returns height of bitmap in pixels.

=back

=head1 AUTHOR

Soheil Seyfaie (soheil@netcom.ca), Albrecht Kleine and developers of ming.sourceforge.net

=head1 SEE ALSO

SWF, SWF::Fill, SWF::Shape, SWF::Button, SWF::Constants, png2dbl, gif2dbl, gif2mask

=cut
