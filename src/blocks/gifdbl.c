/* $Id$ */


#include "ming_config.h"
#include "libming.h"
#include "error.h"

#if !(USE_GIF) // {

SWFDBLBitmapData newSWFDBLBitmapData_fromGifInput(SWFInput input)
{
	SWF_error("newSWFDBLBitmapData_fromGifInput can't be used (no gif compiled into this build of Ming).\n");
	return NULL;
}

SWFDBLBitmapData newSWFDBLBitmapData_fromGifFile(const char * fileName)
{
	SWF_error("newSWFDBLBitmapData_fromGifFile can't be used (no gif compiled into this build of Ming).\n");
	return NULL;
}

#else // def USE_GIF }{

#include "bitmap.h"
#include "dbl.h"
#include "input.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gif_lib.h>
#include <zlib.h>


/*void error(char *msg)
{
  printf("%s:\n\n", msg);
  PrintGifError();
  exit(-1);
}*/

/**
 * Get transparency color from graphic extension block
 *
 * Return: transparency color or -1
 */
int getTransparentColor(GifFileType * file)
{
	int i,returnvalue=-1;
	ExtensionBlock * ext = file->SavedImages[0].ExtensionBlocks;
 
	for (i=0; i < file->SavedImages[0].ExtensionBlockCount; i++, ext++) {

		if (ext->Function == GRAPHICS_EXT_FUNC_CODE) {
			if (ext->Bytes[0] & 1){   /* there is a transparent color */
				if (!ext->Bytes[3]) returnvalue=255; // exception
				else returnvalue=ext->Bytes[3]&0xff;
			}
		}
	}
	return returnvalue;
}

int
readGif(GifFileType *file, dblData result)
{	
	ColorMapObject *colorMap;
	unsigned char *bits;
	unsigned char *data;
	unsigned char *p;
	int i, nColors, size, alpha, bgColor, alignedWidth, bytesPerColor;
	unsigned long outsize;

	if(DGifSlurp(file) != GIF_OK)
	//	error("Error slurping file");
		return 0;

	/* data should now be available */

	/* Local colormap has precedence over Global colormap */
	colorMap = file->Image.ColorMap ? file->Image.ColorMap : file->SColorMap;

	nColors = colorMap->ColorCount;
	alpha = getTransparentColor(file);

	/* bgColor is the background color to fill the bitmap with
	 * if an image is smaller than screen coordinates
	 */
	if (file->SColorMap)			/* There is a GlobalColorMap */
		bgColor = file->SBackGroundColor;   /* The BackGroundColor is meaningful */
	else
	if (alpha >= 0)			/* There is a transparency color */
		bgColor = alpha;			/* set the bgColor to tranparent */
	else
		bgColor = 0;			/* Don't know what to do here.
					 * If this doesn't work, we could
					 * create a new color and set the
					 * alpha-channel to transparent
					 * (unless we are using all the 256
					 * colors, in which case either we
					 * give up, or move to 16-bits palette
					 */
	result->hasalpha = 0;
	bytesPerColor = 3;
	if (alpha >= 0) {
		bytesPerColor += 1;	/* if tranparent color, use the alpha
							 * channel (RGBA instead of RGB)
							 */
		result->hasalpha = 1;
	}
	
	/* Ah! The Flash specs says scanlines must be DWORD ALIGNED!
	 * (but image width is the correct number of pixels)
	 */
	alignedWidth = (file->SWidth + 3) & ~3;

	/* size = size-of-colormap + size-of-bitmap */
	size = (nColors * bytesPerColor) + (alignedWidth * file->SHeight);
	data = malloc(size);

	result->format = 3;
	result->width = file->SWidth;
	result->height = file->SHeight;
	result->format2 = nColors-1;			/* size(colorMap) - 1 */

	p = data;

	/* create ColorMap */
	for(i=0; i<nColors; ++i)
	{
		GifColorType c = colorMap->Colors[i];

		if (bytesPerColor == 3) {	/* RGB */
			*p++ = c.Red;
			*p++ = c.Green;
			*p++ = c.Blue;
		} else {			/* RGBA */
#if 1				/* You would think that an alpha value of 0
				 * would mean fully transparent, but Flash
				 * player doesn't seem to think so.
				 * So, we just set the transparency color 
				 * as 0,0,0,0
				 */
			if (i != alpha) {
				*p++ = c.Red;
				*p++ = c.Green;
				*p++ = c.Blue;
				*p++ = 255;	/* Fully opaque */
			} else {
				*p++ = 0;		/* red */
				*p++ = 0;		/* green */
				*p++ = 0;		/* blue */
				*p++ = 0;		/* Fully transparent */
    		}
#else
			*p++ = c.Red;
			*p++ = c.Green;
			*p++ = c.Blue;
			*p++ = (i != alpha) ? 255 : 0; /* set alpha to 0 for transparent color */
#endif
		}
	}

	bits = file->SavedImages[0].RasterBits;

	if (alignedWidth == file->SWidth
	  && file->Image.Width == file->SWidth 
	  && file->Image.Height == file->SHeight) {

	/* we are all nicely aligned and don't need to move the bitmap around.
	 * Just copy the bits into the output buffer.
	 */
		memcpy(p, bits, file->SWidth * file->SHeight);

	} else {
	/* here we need to pad the scanline or to move the bitmap around
	 * (if the image is not at 0,0 in the logical screen)
	 */
		int screenWidth = file->SWidth;
		int screenHeight = file->SHeight;

		int imageTop = file->Image.Top;
		int imageBottom = file->Image.Top + file->Image.Height;
		int imageLeft = file->Image.Left;
		int imageWidth = file->Image.Width;

		for (i=0; i < screenHeight; i++, p += alignedWidth) {

		/* the image is smaller than the logical "screen":
		 * Fill the reminder with the background color.
	 	*/
			if (imageWidth != screenWidth)
				memset(p, bgColor, screenWidth);

		/* finally, copy scanline
		 */
			if (i >= imageTop && i < imageBottom) {
				memcpy(p + imageLeft, bits, imageWidth);
				bits += imageWidth;
			}
		}
	}

	/* Done! */
#if GIFLIB_MAJOR >= 5 && GIFLIB_MINOR >= 1
	if (DGifCloseFile(file, NULL) != GIF_OK)
	    return 0;
#else
	DGifCloseFile(file);
#endif

	result->data = malloc(outsize = (int)floor(size*1.01+12));

	/* zlib-compress the gif data */
	compress2(result->data, &outsize, data, size, 9);
	result->length = outsize;

	free(data);
	return 1;
}

/*
 *	giflib provides
 *	DGifOpenFileName - use for open from file
 *	DGifOpenFileHandle
 *	DGifOpen - use for open from input
 */

SWFDBLBitmapData newSWFDBLBitmapData_fromGifFile(const char *fileName)
{	GifFileType *file;
	SWFDBLBitmapData ret;
	struct dbl_data gifdata;

#if GIFLIB_MAJOR >= 5
	if((file = DGifOpenFileName(fileName, NULL)) == NULL)
#else
	if((file = DGifOpenFileName(fileName)) == NULL)
#endif
		return NULL;
	if(!readGif(file, &gifdata))
		return NULL;
	ret = newSWFDBLBitmapData_fromData(&gifdata);
	// ret->input = NULL;
	return ret;
}

static int gifReadFunc(GifFileType *gif, unsigned char *buf, int len)
{	SWFInput input = gif->UserData;
	return SWFInput_read(input, buf, len);
}

SWFDBLBitmapData newSWFDBLBitmapData_fromGifInput(SWFInput input)
{	GifFileType *file;
	SWFDBLBitmapData ret;
	struct dbl_data gifdata;

#if GIFLIB_MAJOR >= 5
	if((file = DGifOpen(input, (InputFunc) gifReadFunc, NULL)) == NULL)
#else
	if((file = DGifOpen(input, (InputFunc) gifReadFunc)) == NULL)
#endif
		return NULL;
	if(!readGif(file, &gifdata))
		return NULL;
	ret = newSWFDBLBitmapData_fromData(&gifdata);
	// ret->input = NULL;
	return ret;
}

#endif // def USE_GIF }
