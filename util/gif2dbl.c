/* gif2dbl: convert a gif file to a DBL file using a RGB palette
 * for non-transparent gifs and RGBA palette for transparent ones.
 *
 * author: Raffaele Sena (raff@aromatic.org)
 *
 * (derived from gif2mask.c)
 */

/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ming_config.h>
#include <gif_lib.h>
#include <zlib.h>

#include "libming.h"

void error(char *msg, int errorCode)
{
  printf("%s:\n\n", msg);
  PrintGifError(errorCode);
  exit(-1);
}

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

unsigned char *readGif(char *fileName, int *length, int *bytesPerColor)
{
  GifFileType *file;
  ColorMapObject *colorMap;
  unsigned char *bits;
  unsigned char *data;
  unsigned char *p;
  int i, nColors, size, alpha, bgColor, alignedWidth;

  int errorCode = 0;

#if GIFLIB_MAJOR < 5
  if((file = DGifOpenFileName(fileName)) == NULL)
    error("Error opening file", errorCode);
#else
  if((file = DGifOpenFileName(fileName, &errorCode)) == NULL)
    error("Error opening file", errorCode);
#endif

  if(DGifSlurp(file) != GIF_OK)
#if GIFLIB_MAJOR < 5
    error("Error slurping file", 0);
#else
    error("Error slurping file", file->Error);
#endif

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

  *bytesPerColor = 3;
  if (alpha >= 0) *bytesPerColor += 1;	/* if tranparent color, use the alpha
					 * channel (RGBA instead of RGB)
					 */
	
	/* Ah! The Flash specs says scanlines must be DWORD ALIGNED!
	 * (but image width is the correct number of pixels)
	 */
  alignedWidth = (file->SWidth + 3) & ~3;

  /* size = size-of-header + size-of-colormap + size-of-bitmap */
  size = 6 + (nColors * *bytesPerColor) + (alignedWidth * file->SHeight);
  data = malloc(size);

  p = data;
  *p++ = 3; 				/* 8-bit image data */
  *p++ = (file->SWidth >> 0) & 0xFF;	/* image width */
  *p++ = (file->SWidth >> 8) & 0xFF;
  *p++ = (file->SHeight >> 0) & 0xFF;	/* image height */
  *p++ = (file->SHeight >> 8) & 0xFF;
  *p++ = nColors-1;			/* size(colorMap) - 1 */

	/* create ColorMap */
  for(i=0; i<nColors; ++i)
  {
    GifColorType c = colorMap->Colors[i];

    if (*bytesPerColor == 3) {	/* RGB */
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
  if (DGifCloseFile(file, &errorCode) != GIF_OK)
    error("Error closing file", errorCode);
#else
  DGifCloseFile(file);
#endif

  *length = size;
  return data;
}

void usage()
{
  printf("gif2dbl - convert a gif image to DefineBitLossless2 bitmap\n");
  printf("\nusage: gif2dbl <file.gif>\n");

  exit(0);
}

int main(int argc, char *argv[])
{
  int len, size, bytesPerColor;
  unsigned long outsize;
  char *outfile;
  unsigned char *data, *outdata;
  FILE *dbl;

  if(argc <= 1)
    usage();

  len = strlen(argv[1]);

  if(strcmp(argv[1]+len-4, ".gif") != 0)
    usage();

  outfile = strdup(argv[1]);

  outfile[len-3] = 'd';
  outfile[len-2] = 'b';
  outfile[len-1] = 'l';

  data = readGif(argv[1], &size, &bytesPerColor);

  outdata = malloc(outsize = (int)floor(size*1.01+12));

  /* zlib-compress the gif data */
  compress2(outdata, &outsize, data+6, size-6, 9);

  /* dump to outfile */
  dbl = fopen(outfile, "wb");

  if (dbl)
  {
	/* DBL header */
    fputc('D', dbl);
    fputc('B', dbl);
    fputc('l', dbl);
    fputc(bytesPerColor==3 ? 1 : 2, dbl);	/* 1: RGB - 2: RGBA */

	/* fill size */
    fputc(((outsize+6) >> 24) & 0xFF, dbl);
    fputc(((outsize+6) >> 16) & 0xFF, dbl);
    fputc(((outsize+6) >> 8) & 0xFF, dbl);
    fputc(((outsize+6) >> 0) & 0xFF, dbl);

	/* write header */
    if(fwrite(data, sizeof(char), 6, dbl) != 6)
    {
      perror(outfile);
      exit(1);
    }
	/* write compressed data */
    if(fwrite(outdata, sizeof(char), outsize, dbl) != outsize)
    {
      perror(outfile);
      exit(-1);
    }

	/* and we are done! */
    fclose(dbl);
  }
  else {
    perror(outfile);
    exit(-1);
  }

  exit(0);
}
