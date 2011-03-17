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
#include "input.h"
#include "shape.h"

static void
readBounds(SWFInput input, struct SWFRect_s* bounds)
{
	int nBits;

	SWFInput_byteAlign(input);

	nBits = SWFInput_readBits(input, 5);
	bounds->minX = SWFInput_readSBits(input, nBits);
	bounds->maxX = SWFInput_readSBits(input, nBits);
	bounds->minY = SWFInput_readSBits(input, nBits);
	bounds->maxY = SWFInput_readSBits(input, nBits);
}

static void
readKernInfo(SWFInput input, struct kernInfo *kern)
{
	kern->code1 = SWFInput_getChar(input);
	kern->code2 = SWFInput_getChar(input);
	kern->adjustment = SWFInput_getSInt16(input);
}

static void
readKernInfo16(SWFInput input, struct kernInfo16 *kern)
{
	kern->code1 = SWFInput_getUInt16(input);
	kern->code2 = SWFInput_getUInt16(input);
	kern->adjustment = SWFInput_getSInt16(input);
}


static inline void checkShapeHeader(SWFInput input,
                                    int *numFillBits,
                                    int *numLineBits)
{
	SWFInput_byteAlign(input);
	if ( (*numFillBits = SWFInput_readBits(input, 4)) != 1 ) /* fill bits */
		SWF_error("FdbFont read glyph: bad file format (was expecting fill bits = 1)\n");

	if ( (*numLineBits = SWFInput_readBits(input, 4)) > 0 ) /* line bits */
		SWF_error("FdbFont read glyph: bad file format (was expecting line bits = 0)\n");
}

static inline void checkShapeStyle(SWFInput input, char style,
                             int numFillBits, int numLineBits)
{
	if ( style & 1 )
		if ( SWFInput_readBits(input, numFillBits) != 0 ) /* fill0 = 0 */
			SWF_warn("SWFFont_getShape: bad file format (was expecting fill0 = 0)\n");
	if ( style & 2 )
		if ( SWFInput_readBits(input, numFillBits) != 1 ) /* fill1 = 1 */
			SWF_warn("SWFFont_getShape: bad file format (was expecting fill1 = 1)\n");
	if ( style & 4 )
		if ( SWFInput_readBits(input, numLineBits) != 0 ) /* line = 1 */
			SWF_warn("SWFFont_getShape: bad file format (was expecting line = 0)\n");
}

static int translateShapeRecord(SWFInput input, SWFShape shape)
{
	int moveBits, x = 0, y = 0;
	int straight, numBits;

	if ( SWFInput_readBits(input, 1) == 0 )
	{
		/* it's a moveTo or a shape end */
		if ( SWFInput_readBits(input, 5) == 0 )
			return 0; /* shape end */

		moveBits = SWFInput_readBits(input, 5);
		x = SWFInput_readSBits(input, moveBits);
		y = SWFInput_readSBits(input, moveBits);
		SWFShape_moveScaledPenTo(shape, x, y);
		return 1;
	}

	straight = SWFInput_readBits(input, 1);
	numBits = SWFInput_readBits(input, 4) + 2;
	if (straight == 1)
	{
		if ( SWFInput_readBits(input, 1) ) /* general line */
		{
			x = SWFInput_readSBits(input, numBits);
			y = SWFInput_readSBits(input, numBits);
		}
		else
		{
			if ( SWFInput_readBits(input, 1) ) /* vert = 1 */
				y = SWFInput_readSBits(input, numBits);
			else
				x = SWFInput_readSBits(input, numBits);
		}
		SWFShape_drawScaledLine(shape, x, y);
	}
	else
	{
		int controlX = SWFInput_readSBits(input, numBits);
		int controlY = SWFInput_readSBits(input, numBits);
		int anchorX = SWFInput_readSBits(input, numBits);
		int anchorY = SWFInput_readSBits(input, numBits);

		SWFShape_drawScaledCurve(shape, controlX, controlY,
			anchorX, anchorY);
	}
	return 1;
}

static SWFShape readGlyphShape(SWFInput input)
{
	int numFillBits, numLineBits, moveBits, x, y;
	char style;
	SWFShape shape;

	checkShapeHeader(input, &numFillBits, &numLineBits);

	SWFInput_readBits(input, 2); /* type 0, newstyles */
	style = SWFInput_readBits(input, 3);

	shape = newSWFGlyphShape();
	if (SWFInput_readBits(input, 1))
	{	moveBits = SWFInput_readBits(input, 5);
		x = SWFInput_readSBits(input, moveBits);
		y = SWFInput_readSBits(input, moveBits);
		SWFShape_moveScaledPenTo(shape, x, y);
	}
	else if (style == 0)	/* no style, no move => space character */
	{
		return shape;
	}
	checkShapeStyle(input, style, numFillBits, numLineBits);
	
	/* translate the glyph's shape records into drawing commands */
	while (translateShapeRecord(input, shape))
		;

	return shape;
}

static inline int readFontLayout(SWFInput input, SWFFont font)
{
	int i;
	struct SWFRect_s __rect;

	font->advances = (short *)malloc(
			font->nGlyphs * sizeof(short));
	font->ascent = SWFInput_getSInt16(input);
	font->descent = SWFInput_getSInt16(input);
	font->leading = SWFInput_getSInt16(input);
	
	/* get advances */
	for (i = 0; i < font->nGlyphs; ++i )
		font->advances[i] = SWFInput_getSInt16(input);

	// temp hack
	for (i = 0; i < font->nGlyphs; ++i )
		readBounds(input, &__rect);

		/* get kern table */
	font->kernCount = SWFInput_getUInt16(input);

	if ( font->kernCount > 0 )
	{
		if(font->flags & SWF_FONT_WIDECODES)
			font->kernTable.w = (struct kernInfo16*) malloc(sizeof(struct kernInfo16) * font->kernCount);
		else
			font->kernTable.k = (struct kernInfo*)malloc(sizeof(struct kernInfo) * font->kernCount);
	}
	else
		font->kernTable.k = NULL;

	if(font->flags & SWF_FONT_WIDECODES)
	{
		for (i = 0; i < font->kernCount; ++i )
			readKernInfo16(input, &(font->kernTable.w[i]));
	}
	else
	{
		for (i = 0; i < font->kernCount; ++i )
			readKernInfo(input, &(font->kernTable.k[i]));
	}
	return 0;
}

static inline int checkFdbHeader(SWFInput input)
{
	char f, d, b, z;
	f = SWFInput_getChar(input);
	d = SWFInput_getChar(input);
	b = SWFInput_getChar(input);;
	z = SWFInput_getChar(input);

	if(f != 'f' || d != 'd' || b != 'b' || z != '0')
	{	
		SWF_warn("loadSWFFont: not a fdb file\n");
		return -1;
	}
	
	return 0;
}

SWFFont loadSWFFontFromInput(SWFInput input)
{
	SWFFont font; 
	int namelen, flags, i, nGlyphs;

	if(input == NULL)
		return NULL;

	if(checkFdbHeader(input) < 0)
		return NULL;


	font = newSWFFont();	
	flags = SWFInput_getChar(input);
	font->flags = flags;
	font->langCode = SWFInput_getChar(input); 
	namelen = SWFInput_getChar(input);
	font->name = (char *) malloc(namelen + 1);
	for ( i=0; i < namelen; ++i )
		font->name[i] = SWFInput_getChar(input);
	font->name[namelen] = '\0';

	nGlyphs = SWFInput_getUInt16(input);

	font->nGlyphs = nGlyphs;
	font->glyphToCode = (unsigned short*)malloc(nGlyphs * sizeof(short));	
	if ( flags & SWF_FONT_WIDEOFFSETS )
	{
		for (i=0; i < nGlyphs; ++i)
			SWFInput_getUInt32(input); // glyph offset
		SWFInput_getUInt32(input); // code table offset
	}
	else
	{
		for(i=0; i < nGlyphs; ++i)
			SWFInput_getUInt16(input); // glyph offset
		SWFInput_getUInt16(input); // code table offset
	}

	font->shapes = (SWFShape *)malloc(nGlyphs * sizeof(SWFShape));
	for(i = 0; i < nGlyphs; i++)
		font->shapes[i] = readGlyphShape(input);

	/* read glyph-to-code table */
	if ( flags & SWF_FONT_WIDECODES )
	{
		for (i = 0; i < nGlyphs; ++i )
			font->glyphToCode[i] = SWFInput_getUInt16(input);
	}
	else
	{
		for (i = 0; i < nGlyphs; ++i )
			font->glyphToCode[i] = SWFInput_getChar(input);
	}

	if ( flags & SWF_FONT_HASLAYOUT )
		readFontLayout(input, font);
	SWFFont_buildReverseMapping(font);	
	return font;
}

/* pull font definition from fdb (font def block) file */
SWFFont loadSWFFont_fromFdbFile(FILE *file)
{
	SWFInput input;
	SWFFont font;
	if ( file == NULL )
		return NULL;

	input = newSWFInput_file(file);
	font = loadSWFFontFromInput(input);
	destroySWFInput(input);
	return font;
}



#if 0
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
#endif
