/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

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

#include <stdlib.h>
#include "font.h"
#include "rect.h"

#define SWF_LOAD_FONT_HASLAYOUT    (1<<7)
#define SWF_LOAD_FONT_SHIFTJIS     (1<<6)
#define SWF_LOAD_FONT_UNICODE      (1<<5)
#define SWF_LOAD_FONT_ANSI         (1<<4)
#define SWF_LOAD_FONT_WIDEOFFSETS  (1<<3)
#define SWF_LOAD_FONT_WIDECODES    (1<<2)
#define SWF_LOAD_FONT_ITALIC       (1<<1)
#define SWF_LOAD_FONT_BOLD         (1<<0)

static int readUInt16(FILE *f)
{
  return fgetc(f) + (fgetc(f)<<8);
}

static int readSInt16(FILE *f)
{
  return fgetc(f) + fgetc(f)*256;
}

static unsigned long readUInt32(FILE *f)
{
  return (unsigned long)(fgetc(f) + (fgetc(f)<<8) + (fgetc(f)<<16) + (fgetc(f)<<24));
}

int buffer;
int bufbits = 0; /* # of bits in buffer */

static void byteAlign()
{
  if(bufbits > 0)
  {
    bufbits = 0;
    buffer = 0;
  }
}

static int readBits(FILE *f, int number)
{
  int ret = buffer;

  if(number == bufbits)
  {
    bufbits = 0;
    buffer = 0;
    return ret;
  }

  if(number > bufbits)
  {
    number -= bufbits;

    while(number>8)
    {
      ret <<= 8;
      ret += fgetc(f);
      number -= 8;
    }

    buffer = fgetc(f);

    if(number>0)
    {
      ret <<= number;
      bufbits = 8-number;
      ret += buffer >> (8-number);
      buffer &= (1<<bufbits)-1;
    }

    return ret;
  }

  ret = buffer >> (bufbits-number);
  bufbits -= number;
  buffer &= (1<<bufbits)-1;

  return ret;
}

static int readSBits(FILE *f, int number)
{
  int num = readBits(f, number);

  if(num & (1<<(number-1)))
    return num - (1<<number);
  else
    return num;
}

void readBounds(FILE *file, SWFRect bounds)
{
  int nBits;
  byteAlign();

  nBits = readBits(file, 5);
  bounds->minX = readSBits(file, nBits);
  bounds->maxX = readSBits(file, nBits);
  bounds->minY = readSBits(file, nBits);
  bounds->maxY = readSBits(file, nBits);
}

void readKernInfo(FILE *file, struct KernInfo *kernInfo)
{
  kernInfo->code1 = fgetc(file);
  kernInfo->code2 = fgetc(file);
  kernInfo->adjustment = readSInt16(file);
}

SWFFont loadSWFFontFromFile(FILE *file)
{
  /* pull font definition from fdb (font def block) file */

  SWFFont font = newSWFFont();
  int namelen, flags, i, nGlyphs;
  byte *p;

  if(file==NULL)
    return NULL;

  fgetc(file); /* header whatever */
  fgetc(file);
  fgetc(file);
  fgetc(file);

  flags = fgetc(file);

/* this isn't right, and I don't know why..

  if(flags & SWF_LOAD_FONT_SHIFTJIS)
  	font->flags |= SWF_FONT_SHIFTJIS;
  if(flags & SWF_LOAD_FONT_ANSI)
  	font->flags |= SWF_FONT_ANSI;
  if(flags & SWF_LOAD_FONT_UNICODE)
  	font->flags |= SWF_FONT_UNICODE;
  if(flags & SWF_LOAD_FONT_ITALIC)
  	font->flags |= SWF_FONT_ISITALIC;
  if(flags & SWF_LOAD_FONT_BOLD)
  	font->flags |= SWF_FONT_ISBOLD;
*/

  font->flags |= SWF_FONT_SHIFTJIS;

  fgetc(file); /* "reserved" */

  namelen = fgetc(file);
  font->name = malloc(namelen);

  for(i=0; i<namelen; ++i)
    font->name[i] = fgetc(file);

  nGlyphs = readUInt16(file);

  font->nGlyphs = nGlyphs;

  font->bounds = (SWFRect)malloc(nGlyphs * SWFRECT_SIZE);

  if(flags & SWF_LOAD_FONT_WIDEOFFSETS)
    for(i=0; i<=nGlyphs; ++i)
      font->glyphOffset[i] = (byte *)(readUInt32(file) - 4*nGlyphs - 4);
  else
    for(i=0; i<=nGlyphs; ++i)
      font->glyphOffset[i] = (byte *)(readUInt16(file) - 2*nGlyphs - 2);

  /* it helps to allocate the right amount. (thanks, Tim!) */
  font->shapes = malloc(font->glyphOffset[nGlyphs] - font->glyphOffset[0] + 1);

  /* adjust offset table to point to shapes */
  for(i=0; i<=nGlyphs; ++i)
  	font->glyphOffset[i] += (unsigned int)font->shapes;

  p = font->shapes;

  for(i=(int)(font->glyphOffset[nGlyphs]-font->glyphOffset[0]); i>0; --i)
    *(p++) = fgetc(file);

  for(i=0; i<nGlyphs; ++i)
    font->codeTable[fgetc(file)] = i;

  if(flags & SWF_LOAD_FONT_HASLAYOUT)
  {
    font->ascent = readSInt16(file);
    font->descent = readSInt16(file);
    font->leading = readSInt16(file);

    /* get advances */
    for(i=0; i<nGlyphs; ++i)
      font->advances[i] = readSInt16(file);

    /* get bounds */
    for(i=0; i<nGlyphs; ++i)
      readBounds(file, &(font->bounds[i]));

    /* get kern table */
    font->kernCount = readUInt16(file);
    font->kernTable = malloc(sizeof(struct KernInfo) * font->kernCount);

    p = (byte *)font->kernTable;

    for(i=0; i<font->kernCount; ++i)
      readKernInfo(file, &(font->kernTable[i]));
  }

  font->nGlyphs = 0; /* prepare for adding chars.. */

  return font;
}
