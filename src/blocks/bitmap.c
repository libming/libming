/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* $Id$ */

#include "bitmap.h"
#include "rect.h"
#include "jpeg.h"
#include "dbl.h"
#include "error.h"
#include "input.h"
#include "libming.h"

void destroySWFBitmap(SWFBitmap bitmap)
{
	// this will acall bitmap->dtor;
	destroySWFBlock((SWFBlock) bitmap);
}


int SWFBitmap_getWidth(SWFBitmap b)
{
	return SWFRect_getWidth(CHARACTER(b)->bounds);
}


int SWFBitmap_getHeight(SWFBitmap b)
{
	return SWFRect_getHeight(CHARACTER(b)->bounds);
}


#define JPEG_MARKER 0xFF
#define JPEG_SOI	0xD8

SWFBitmap newSWFBitmap_fromInput(SWFInput input)
{
	int c1 = SWFInput_getChar(input);
	int c2 = SWFInput_getChar(input);
	int c3 = SWFInput_getChar(input);

	SWFInput_rewind(input);

	if(c1 == JPEG_MARKER && c2 == JPEG_SOI)
		return (SWFBitmap)newSWFJpegBitmap_fromInput(input);

	if(c1 == 'D' && c2 == 'B')
		return (SWFBitmap) newSWFDBLBitmap_fromInput(input);

	if(c1 == 'G' && c2 == 'I')
#if USE_GIF
		return (SWFBitmap) newSWFDBLBitmapData_fromGifInput(input);
#else
		SWF_error("GIF images must be translated into DBL files for now");
#endif

	if(c2 == 'P' && c3 == 'N')
#if USE_PNG
		return (SWFBitmap) newSWFDBLBitmapData_fromPngInput(input);
#else
		SWF_error("PNG images must be translated into DBL files for now");
#endif

	SWF_error("Unrecognised file type");

	return NULL;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
