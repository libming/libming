/*
    Ming, an SWF output library
    Copyright (C) 2000  Opaque Industries - http://www.opaque.net/

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

#include <sys/stat.h>
#ifdef unix
  #include <unistd.h>
#endif
#include <stdlib.h>
#include "jpeg.h"
#include "block.h"

/* JPEG stream markers: */
#define JPEG_MARKER 0xFF

/* Start of Image, End of Image */
#define JPEG_SOI  0xD8
#define JPEG_EOI  0xD9

#define JPEG_JFIF 0xE0

/* encoding markers, quantization tables and Huffman tables */
#define JPEG_QUANT 0xDB
#define JPEG_HUFF  0xC4

/* image markers, start of frame and start of scan */
#define JPEG_SOF0 0xC0
#define JPEG_SOF1 0xC1
#define JPEG_SOF2 0xC2
#define JPEG_SOS  0xDA

#define JPEG_ED   0xED /* app13 */
#define JPEG_EE   0xEE /* app14 */
#define JPEG_DD   0xDD /* ??? */

int completeSWFJpegBitmap(SWFBlock block)
{
  return ((SWFJpegBitmap)block)->length;
}

#define BUFFER_SIZE 1024

/* clumsy utility function.. */
void dumpJpegBlock(byte type, FILE *f,
		   SWFByteOutputMethod method, void *data)
{
  int i, l, l0, l1, length;
  byte buffer[BUFFER_SIZE];

  method(JPEG_MARKER, data);
  method(type, data);

  method((l0 = fgetc(f)), data);
  method((l1 = fgetc(f)), data);

  length = (l0<<8) + l1 - 2;

  while(length > 0)
  {
    l = fread(buffer, sizeof(byte), min(BUFFER_SIZE, length), f);

    /* SWFByteOutputMethod should have multibyte output.. */
    for(i=0; i<l; ++i)
      method(buffer[i], data);

    length -= l;
  }
}
int skipJpegBlock(FILE *f)
{
  int length, l;

  length = l = (fgetc(f)<<8) + fgetc(f);

  for(l-=2; l>0; --l)
    fgetc(f);

  return length;
}

void methodWriteJpegFile(FILE *f, SWFByteOutputMethod method, void *data)
{
  byte buffer[BUFFER_SIZE];
  int l, c, i;

  rewind(f);

  if((c=fgetc(f)) != 0xFF)
    error("Initial Jpeg marker not found!");

  if((c=fgetc(f)) != 0xD8)
    error("Jpeg SOI not found!");

  /*
  method(JPEG_MARKER, data);
  method(JPEG_SOI, data);
  method(JPEG_MARKER, data);
  method(JPEG_EOI, data);
  */

  method(JPEG_MARKER, data);
  method(JPEG_SOI, data);

  for(;;)
  {
    if(fgetc(f) != 0xFF)
      error("Jpeg marker not found where expected!");

    switch(c=fgetc(f))
    {
      case JPEG_EOI:
	error("Unexpected end of Jpeg file (EOI found)!");

	/*      case JPEG_JFIF: */
      case JPEG_QUANT:
      case JPEG_HUFF:
      case JPEG_DD:

	/*	if(!finishedEncoding) */
	dumpJpegBlock(c, f, method, data);
	/* else
	   error("Encoding tables found in Jpeg image section!"); */

	break;

      case JPEG_SOF0:
      case JPEG_SOF1:
      case JPEG_SOF2:
	/*
	if(!finishedEncoding)
	{
	  finishedEncoding = TRUE;
	  method(JPEG_MARKER, data);
	  method(JPEG_EOI, data);
	  method(JPEG_MARKER, data);
	  method(JPEG_SOI, data);
	  method(JPEG_MARKER, data);
	  method(c, data);
	}
	*/
	dumpJpegBlock(c, f, method, data);
	break;

      case JPEG_SOS:
	/*
	if(!finishedEncoding)
	  error("Found SOS before SOF in Jpeg file!");
	*/
	break;

      default:
	/* dumpJpegBlock(c, f, method, data); */
	skipJpegBlock(f);
    }

    if(c == JPEG_SOS)
      break;

    if(feof(f))
      error("Unexpected end of Jpeg file (EOF found)!");
  }

  if(c != JPEG_SOS)
    error("SOS block not found in Jpeg file!");

  /* rest is SOS, dump to end of file */
  method(JPEG_MARKER, data);
  method(c, data);

  while((l=fread(buffer, sizeof(byte), BUFFER_SIZE, f)) > 0)
    for(i=0; i<l; ++i)
      method(buffer[i], data);
}

void writeSWFJpegBitmapToMethod(SWFBlock block,
				SWFByteOutputMethod method, void *data)
{
  SWFJpegBitmap jpeg = (SWFJpegBitmap)block;
  FILE *f = jpeg->file;
  /*  byte finishedEncoding = FALSE; */

  methodWriteUInt16(CHARACTERID(jpeg), method, data);
  methodWriteJpegFile(f, method, data);
}

void writeSWFJpegWithAlphaToMethod(SWFBlock block,
				   SWFByteOutputMethod method, void *data)
{
  SWFJpegWithAlpha jpeg = (SWFJpegWithAlpha)block;
  FILE *f = jpeg->file;
  FILE *alpha = jpeg->alpha;
  byte buffer[BUFFER_SIZE];
  int l, i;

  methodWriteUInt16(CHARACTERID(jpeg), method, data);
  methodWriteUInt32(jpeg->jpegLength, method, data);
  methodWriteJpegFile(f, method, data);

  /* now write alpha file.. */

  rewind(alpha);

  while((l=fread(buffer, sizeof(byte), BUFFER_SIZE, alpha)) > 0)
    for(i=0; i<l; ++i)
      method(buffer[i], data);
}

void destroySWFJpegBitmap(SWFBlock block)
{
  free(CHARACTER(block)->bounds);
  free(block);
}

struct jpegInfo
{
  int width;
  int height;
  int length;
};

struct jpegInfo *scanJpegFile(FILE *f)
{
  int length = 0, l, c;
  long pos, end;

  struct jpegInfo *info = malloc(sizeof(struct jpegInfo));

  /* scan file, get height and width, make sure it looks valid,
     also figure length of block.. */

  if(fgetc(f) != 0xFF)
    error("Initial Jpeg marker not found!");

  if(fgetc(f) != 0xD8)
    error("Jpeg SOI not found!");

  for(;;)
  {
    if(fgetc(f) != 0xFF)
      error("Jpeg marker not found where expected!");

    switch(c=fgetc(f))
    {
      case JPEG_EOI:
	error("Unexpected end of Jpeg file (EOI found)!");

	/*      case JPEG_JFIF: */
      case JPEG_QUANT:
      case JPEG_HUFF:
      case JPEG_DD:
	/*
	if(finishedEncoding)
	  error("Encoding tables found in Jpeg image section!");
	*/
	length += skipJpegBlock(f) + 2;
	break;

      case JPEG_SOF1:
      case JPEG_SOF2:
	error("Only baseline (frame 0) jpegs are supported!");

      case JPEG_SOF0:
	/*
	if(finishedEncoding)
	  error("Found second SOF in Jpeg file!")
	else
	{
	  finishedEncoding = TRUE;
	  length += 4;*/ /* end image, start image */
	/*}*/

	l = (fgetc(f)<<8) + fgetc(f);
	fgetc(f); /* precision */
	info->height = (fgetc(f)<<8) + fgetc(f);
	info->width = (fgetc(f)<<8) + fgetc(f);
	length += l + 2;
	l-=7;
	for(; l>0; --l)
	  fgetc(f);
	break;

      case JPEG_SOS:
	/*
	if(!finishedEncoding)
	  error("Found SOS before SOF in Jpeg file!");
	*/
	break;

      default:
	/* length += */ skipJpegBlock(f) /* + 2 */;
    }

    if(c == JPEG_SOS)
      break;

    if(feof(f))
      error("Unexpected end of Jpeg file (EOF found)!");
  }

  if(c != JPEG_SOS)
    error("SOS block not found in Jpeg file!");

  /* rest is SOS, dump to end of file */

  length += 2; /* SOS tag */

  pos = ftell(f);
  fseek(f, 0, SEEK_END);
  end = ftell(f);

  length += end - pos;

  info->length = length;

  return info;
}

SWFJpegBitmap newSWFJpegBitmap(FILE *f)
{
  SWFJpegBitmap jpeg;
  struct jpegInfo *info;

  jpeg = (SWFJpegBitmap)malloc(SWFJPEGBITMAP_SIZE);
  memset(jpeg, 0, SWFJPEGBITMAP_SIZE);

  CHARACTERID(jpeg) = ++SWF_gNumCharacters;
  BLOCK(jpeg)->writeBlock = writeSWFJpegBitmapToMethod;
  BLOCK(jpeg)->complete = completeSWFJpegBitmap;
  BLOCK(jpeg)->dtor = destroySWFJpegBitmap;
  BLOCK(jpeg)->type = SWF_DEFINEBITSJPEG2;
  CHARACTER(jpeg)->bounds = newSWFRect(0, 0, 0, 0);

  jpeg->file = f;

  info = scanJpegFile(f);

  CHARACTER(jpeg)->bounds->maxX = info->width;
  CHARACTER(jpeg)->bounds->maxY = info->height;

  jpeg->length = info->length + 4;

  free(info);

  return jpeg;
}


/* f is a jpeg file, alpha is zlib-compressed data */

SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha)
{
  SWFJpegWithAlpha jpeg;
  struct jpegInfo *info;
  struct stat sbuf;

  jpeg = (SWFJpegWithAlpha)malloc(SWFJPEGWITHALPHA_SIZE);
  memset(jpeg, 0, SWFJPEGWITHALPHA_SIZE);

  CHARACTERID(jpeg) = ++SWF_gNumCharacters;
  BLOCK(jpeg)->writeBlock = writeSWFJpegWithAlphaToMethod;
  BLOCK(jpeg)->complete = completeSWFJpegBitmap; /* can use same complete */
  BLOCK(jpeg)->dtor = destroySWFJpegBitmap;      /* ditto here */
  BLOCK(jpeg)->type = SWF_DEFINEBITSJPEG3;
  CHARACTER(jpeg)->bounds = newSWFRect(0, 0, 0, 0);

  jpeg->file = f;
  jpeg->alpha = alpha;

  info = scanJpegFile(f);

  CHARACTER(jpeg)->bounds->maxX = info->width;
  CHARACTER(jpeg)->bounds->maxY = info->height;

  jpeg->jpegLength = info->length + 2; /* ?? */

  free(info);

  if(fstat(fileno(alpha), &sbuf) != 0)
    error("couldn't fstat alpha file!");

  jpeg->length = jpeg->jpegLength + sbuf.st_size + 6;

  return jpeg;
}
