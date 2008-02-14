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
#include <stdlib.h>

#include "bitmap.h"
#include "rect.h"
#include "jpeg.h"
#include "dbl.h"
#include "error.h"
#include "input.h"
#include "libming.h"

#if USE_ZLIB
#include "zlib.h"

// workarround for zlib previous 1.2.x
#ifndef compressBound
#define compressBound(__slen) ((__slen) + ((__slen) >> 12) + ((__slen) >> 14) + 11)
#endif

#endif

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


SWFBitmap newSWFBitmap_fromRawImg(unsigned char *raw, 
                                  SWFRawImgFmt srcFmt, SWFBitmapFmt dstFmt,
                                  unsigned short width, unsigned short height)
{
#if USE_ZLIB
	struct dbl_data image;
	uLongf insize, outsize;
	int ret;
	SWFBitmap return_data;
	unsigned char *tmp;

	switch(srcFmt)
	{
		case SWF_RAWIMG_ARGB:
			tmp = raw;
			break;
		default:
			SWF_warn("newSWFBitmap_fromRawImg: unknown img format\n");
			return NULL;
	}

	image.width = width;
	image.height = height;
	image.hasalpha = 1;
	image.format = 5;
	
	insize = width * height * 4;
	outsize = compressBound(insize);
	image.data = (unsigned char*) malloc(outsize);

	/* If malloc failed, return NULL to signify this */
	if (NULL == image.data)
		return NULL;

	ret = compress2(image.data, &outsize, (Bytef *)tmp, insize, 9);
	if(ret != Z_OK)
	{
		free(image.data);
		return NULL;
	}
	image.length = outsize;

	return_data = (SWFBitmap)newSWFDBLBitmapData_fromData(&image);

	/* If newSWFDBLBitmapData_fromData() failed, return NULL to signify this */
	if (NULL == return_data)
	{
		free(image.data);
		return NULL;
	}

	return return_data;
#else
	SWF_warn("newSWFBitmap_fromRawImg: depends on zlib support\n");
	return NULL;
#endif
} 

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
