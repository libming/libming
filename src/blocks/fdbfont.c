/*
Ming, an SWF output library
Copyright (C) 2002	Opaque Industries - http://www.opaque.net/

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA
*/

/* $Id$ */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "font.h"
#include "method.h"
#include "utf8.h"
#include "character.h"
#include "error.h"
#include "movie.h"
#include "libming.h"


// #define HAS_MMAP 1

#ifdef HAS_MMAP
	#include <unistd.h>
	#include <sys/mman.h>
#endif


static int
readUInt16(FILE *f)
{
	int low = fgetc(f);
	int high = fgetc(f);
	return low + (high<<8);
}

static int
readSInt16(FILE *f)
{
	int low = fgetc(f);
	int high = fgetc(f);
	return low + high*256;
}


static unsigned long
readUInt32(FILE *f)
{
	return (unsigned long)(fgetc(f) + (fgetc(f)<<8) + (fgetc(f)<<16) + (fgetc(f)<<24));
}


static int buffer;
static int bufbits = 0; /* # of bits in buffer */

static void
byteAlign()
{
	if ( bufbits > 0 )
	{
		bufbits = 0;
		buffer = 0;
	}
}


static int
readBits(FILE *f, int number)
{
	int ret = buffer;

	if ( number == bufbits )
	{
		bufbits = 0;
		buffer = 0;
		return ret;
	}

	if ( number > bufbits )
	{
		number -= bufbits;

		while( number > 8 )
		{
			ret <<= 8;
			ret += fgetc(f);
			number -= 8;
		}

		buffer = fgetc(f);

		if ( number > 0 )
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


static int
readSBits(FILE *f, int number)
{
	int num = readBits(f, number);

	if ( num & (1<<(number-1)) )
		return num - (1<<number);
	else
		return num;
}


static void
readBounds(FILE *file, struct SWFRect_s* bounds)
{
	int nBits;

	byteAlign();

	nBits = readBits(file, 5);
	bounds->minX = readSBits(file, nBits);
	bounds->maxX = readSBits(file, nBits);
	bounds->minY = readSBits(file, nBits);
	bounds->maxY = readSBits(file, nBits);
}


static void
readKernInfo(FILE *file, struct kernInfo *kern)
{
	kern->code1 = fgetc(file);
	kern->code2 = fgetc(file);
	kern->adjustment = readSInt16(file);
}

static void
readKernInfo16(FILE *file, struct kernInfo16 *kern)
{
	kern->code1 = readUInt16(file);
	kern->code2 = readUInt16(file);
	kern->adjustment = readSInt16(file);
}

static void
SWFFont_buildReverseMapping(SWFFont font)
{
	int i;

	if ( font->flags & SWF_FONT_WIDECODES )
	{
		font->codeToGlyph.wideMap = (unsigned short**)malloc(256 * sizeof(unsigned short*));

		for ( i=0; i<256; ++i )
			font->codeToGlyph.wideMap[i] = NULL;

		for ( i=0; i<font->nGlyphs; ++i )
		{
			unsigned short charcode = font->glyphToCode[i];
			byte high = charcode >> 8;
			byte low = charcode & 0xff;

			if ( font->codeToGlyph.wideMap[high] == NULL )
			{
				font->codeToGlyph.wideMap[high] = (unsigned short*)malloc(256 * sizeof(unsigned short));
				memset(font->codeToGlyph.wideMap[high], 0, 256 * sizeof(unsigned short));
			}

			font->codeToGlyph.wideMap[high][low] = i;
		}
	}
	else
	{
		font->codeToGlyph.charMap = (byte*) malloc(256 * sizeof(char));
		memset(font->codeToGlyph.charMap, 0, 256 * sizeof(char));

		for ( i=0; i<font->nGlyphs; ++i )
			font->codeToGlyph.charMap[font->glyphToCode[i]] = i;
	}
}

SWFFont loadSWFFontFromFile(FILE *file)
{
	/* pull font definition from fdb (font def block) file */

	SWFFont font = newSWFFont();
	int namelen, flags, i, nGlyphs;
	int shapeTableLen;
	byte *p;

	if ( file == NULL )
		return NULL;

	fgetc(file); /* header whatever */
	fgetc(file);
	fgetc(file);
	fgetc(file);

	flags = fgetc(file);

/* this isn't right, and I don't know why.. */

	if(flags & SWF_FONT_HASLAYOUT)
		font->flags |= SWF_FONT_HASLAYOUT;
	if(flags & SWF_FONT_SHIFTJIS)
		font->flags |= SWF_FONT_SHIFTJIS;
	if(flags & SWF_FONT_ANSI)
		font->flags |= SWF_FONT_ANSI;
	if(flags & SWF_FONT_SMALLTEXT)
		font->flags |= SWF_FONT_SMALLTEXT;
	if(flags & SWF_FONT_ISITALIC)
		font->flags |= SWF_FONT_ISITALIC;
	if(flags & SWF_FONT_ISBOLD)
		font->flags |= SWF_FONT_ISBOLD;
	if(flags & SWF_FONT_WIDEOFFSETS)
		font->flags |= SWF_FONT_WIDEOFFSETS;
	if(flags & SWF_FONT_WIDECODES)
		font->flags |= SWF_FONT_WIDECODES;

	font->langCode = fgetc(file); 

	namelen = fgetc(file);
	font->name = (byte*) malloc(namelen+1);

	for ( i=0; i<namelen; ++i )
		font->name[i] = fgetc(file);

	font->name[namelen] = '\0';

	nGlyphs = readUInt16(file);

	font->nGlyphs = nGlyphs;

	font->bounds = (struct SWFRect_s*) malloc(nGlyphs * sizeof(struct SWFRect_s));
	font->glyphOffset = (byte**)malloc((nGlyphs + 1) * sizeof(*font->glyphOffset));
	font->glyphToCode = (unsigned short*)malloc(nGlyphs * sizeof(*font->glyphToCode));
	font->advances = (short*) malloc(nGlyphs * sizeof(*font->advances));

	if ( flags & SWF_FONT_WIDEOFFSETS )
	{
		for ( i=0; i<=nGlyphs; ++i )
			font->glyphOffset[i] = (byte *)(readUInt32(file) - 4*nGlyphs - 4);
	}
	else
	{
		for(i=0; i<=nGlyphs; ++i)
			font->glyphOffset[i] = (byte *)(readUInt16(file) - 2*nGlyphs - 2);
	}

	shapeTableLen = font->glyphOffset[nGlyphs] - font->glyphOffset[0];

#if HAS_MMAP
	font->shapes =
		mmap(NULL, shapeTableLen + 1, PROT_READ, MAP_PRIVATE,
				 fileno(file), ftell(file));

	if ( (void*)font->shapes != MAP_FAILED )
	{
		fseek(file, shapeTableLen, SEEK_CUR);
	}
	else
	{
#endif

	/* it helps to allocate the right amount. (thanks, Tim!) */
	font->shapes = (byte*)malloc(font->glyphOffset[nGlyphs] - font->glyphOffset[0] + 1);

	p = font->shapes;

	while ( shapeTableLen > 0 )
	{
		int count = fread(p, 1, shapeTableLen, file);
		shapeTableLen -= count;
		p += count;
	}

#if HAS_MMAP
  }
#endif

	/* adjust offset table to point to shapes */

	for ( i=0; i<=nGlyphs; ++i )
		font->glyphOffset[i] += (unsigned long)font->shapes;

	/* read glyph-to-code table */

	if ( flags & SWF_FONT_WIDECODES )
	{
		for ( i=0; i<nGlyphs; ++i )
			font->glyphToCode[i] = readUInt16(file);
	}
	else
	{
		for ( i=0; i<nGlyphs; ++i )
			font->glyphToCode[i] = fgetc(file);
	}

	// build code-to-glyph table
	// XXX - make new version of FDB with reverse map?
	SWFFont_buildReverseMapping(font);

	/* read metrics */

	if ( flags & SWF_FONT_HASLAYOUT )
	{
		font->ascent = readSInt16(file);
		font->descent = readSInt16(file);
		font->leading = readSInt16(file);

		/* get advances */
		for ( i=0; i<nGlyphs; ++i )
			font->advances[i] = readSInt16(file);

		/* get bounds */
		for ( i=0; i<nGlyphs; ++i )
			readBounds(file, &font->bounds[i]);

		/* get kern table */
		font->kernCount = readUInt16(file);

		if ( font->kernCount > 0 )
			if(font->flags & SWF_FONT_WIDECODES)
				font->kernTable.w = (struct kernInfo16*) malloc(sizeof(struct kernInfo16) * font->kernCount);
			else
				font->kernTable.k = (struct kernInfo*)malloc(sizeof(struct kernInfo) * font->kernCount);
		else
			font->kernTable.k = NULL;

		if(font->flags & SWF_FONT_WIDECODES)
			for ( i=0; i<font->kernCount; ++i )
				readKernInfo16(file, &(font->kernTable.w[i]));
		else
			for ( i=0; i<font->kernCount; ++i )
				readKernInfo(file, &(font->kernTable.k[i]));
	}

	return font;
}



/* retrieve glyph shape of a font */
#include <stdio.h>
#include <stdarg.h>
/* basic IO */

struct out
{	char *buf, *ptr;
	int len;
};

static void oprintf(struct out *op, const char *fmt, ...)
{	va_list ap;
	char buf[256];
	int d, l;
	
	va_start(ap, fmt);
	l = vsprintf(buf, fmt, ap);
	while((d = op->ptr - op->buf) + l >= op->len-1)
	{	op->buf = (char *) realloc(op->buf, op->len += 100);
		op->ptr = op->buf + d;
	}
	for(d = 0 ; d < l ; d++)
		*op->ptr++ = buf[d];
}

static int readBitsP(byte **p, int number)
{
	byte *ptr = *p;
	int ret = buffer;

	if(number == bufbits)
	{
		ret = buffer;
		bufbits = 0;
		buffer = 0;
	}
	else if(number > bufbits)
	{
		number -= bufbits;

		while(number>8)
		{
			ret <<= 8;
			ret += *ptr++;
			number -= 8;
		}

		buffer = *ptr++;

		if(number>0)
		{
			ret <<= number;
			bufbits = 8-number;
			ret += buffer >> (8-number);
			buffer &= (1<<bufbits)-1;
		}
	}
	else
	{
		ret = buffer >> (bufbits-number);
		bufbits -= number;
		buffer &= (1<<bufbits)-1;
	}

	*p = ptr;
	return ret;
}

static int readSBitsP(byte **p, int number)
{
	int num = readBitsP(p, number);

	if(num & (1<<(number-1)))
		return num - (1<<number);
	else
		return num;
}

// return a malloc'ed string describing the glyph shape
char *
SWFFont_getShape(SWFFont font, unsigned short c)
{
	byte *p = SWFFont_findGlyph(font, c);
	byte **f = &p;
	struct out o;
	int moveBits, x = 0, y = 0;
	int straight, numBits;
	int numFillBits, numLineBits;
	int startX = 0;
	int startY = 0;
	int style;

	o.len = 0;
	o.ptr = o.buf = (char *)malloc(1);
	*o.ptr = 0;

	byteAlign();

	if ( (numFillBits = readBitsP(f, 4)) != 1 ) /* fill bits */
		SWF_error("SWFFont_getShape: bad file format (was expecting fill bits = 1)");

	if ( (numLineBits = readBitsP(f, 4)) > 1 ) /* line bits */
		SWF_error("SWFFont_getShape: bad file format (was expecting line bits = 0)");

	/* now we get to parse the shape commands.	Oh boy.
		 the first one will be a non-edge block- grab the moveto loc */

	readBitsP(f, 2); /* type 0, newstyles */
	style = readBitsP(f, 3);
	
	if(readBitsP(f, 1))
	{	moveBits = readBitsP(f, 5);
		x = startX + readSBitsP(f, moveBits);
		y = startY + readSBitsP(f, moveBits);

		oprintf(&o, "moveto %d,%d\n", x, y);
	}
	else if(style == 0)	/* no style, no move => space character */
		return o.buf;

	if ( style & 1 )
		if ( readBitsP(f, numFillBits) != 0 ) /* fill0 = 0 */
			SWF_error("SWFFont_getShape: bad file format (was expecting fill0 = 0)");
	if ( style & 2 )
		if ( readBitsP(f, numFillBits) != 1 ) /* fill1 = 1 */
			SWF_error("SWFFont_getShape: bad file format (was expecting fill1 = 1)");
	if ( style & 4 )
		if ( readBitsP(f, numLineBits) != 0 ) /* line = 1 */
			SWF_error("SWFFont_getShape: bad file format (was expecting line = 0)");

	/* translate the glyph's shape records into drawing commands */

	for ( ;; )
	{
		if ( readBitsP(f, 1) == 0 )
		{
			/* it's a moveTo or a shape end */

			if ( readBitsP(f, 5) == 0 )
				break;

			moveBits = readBitsP(f, 5);
			x = startX + readSBitsP(f, moveBits);
			y = startY + readSBitsP(f, moveBits);

			oprintf(&o, "moveto %d,%d\n", x, y);

			continue;
		}

		straight = readBitsP(f, 1);
		numBits = readBitsP(f, 4)+2;

		if ( straight==1 )
		{
			if ( readBitsP(f, 1) ) /* general line */
			{
				x += readSBitsP(f, numBits);
				y += readSBitsP(f, numBits);
			}
			else
			{
				if ( readBitsP(f, 1) ) /* vert = 1 */
					y += readSBitsP(f, numBits);
				else
					x += readSBitsP(f, numBits);
			}

			oprintf(&o, "lineto %d,%d\n", x, y);
		}
		else
		{
			int controlX = readSBitsP(f, numBits);
			int controlY = readSBitsP(f, numBits);
			int anchorX = readSBitsP(f, numBits);
			int anchorY = readSBitsP(f, numBits);

			oprintf(&o, "curveto %d,%d %d,%d\n",
				 x+controlX, y+controlY,
				 x+controlX+anchorX, y+controlY+anchorY);

			x += controlX + anchorX;
			y += controlY + anchorY;
		}
	}

	*o.ptr = 0;
	return o.buf;
}

