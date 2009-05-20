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

#include "libming.h"
#include "linestyle.h"
#include "fillstyle.h"
#include "error.h"

#define SWF_LINESTYLE_FLAG_FILL		(1<<11)

struct SWFLineStyle_s
{
	unsigned short width;
	byte r;
	byte g;
	byte b;
	byte a;

	/* LINESTYLE2 extensions */
	// 16 bit flags
	int flags;
	float miterLimit;
	SWFFillStyle fill;
};


/* 
 * sets simple linestyle 
 * width is set in TWIPS
 */
SWFLineStyle newSWFLineStyle(unsigned short width,
                             byte r, byte g, byte b, byte a)
{
	SWFLineStyle line = (SWFLineStyle)malloc(sizeof(struct SWFLineStyle_s));

	line->width = width;
	line->r = r;
	line->g = g;
	line->b = b;
	line->a = a;
	line->flags = 0;

	return line;
}

/*
 * create Linestyle2 introduce with SWF 8.
 * Linestyle2 extends Linestyle1 with some extra flags:
 *
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
SWFLineStyle newSWFLineStyle2(unsigned short width, byte r, byte g, byte b, byte a, 
                              int flags, float miterLimit)
{
	SWFLineStyle line = (SWFLineStyle)malloc(sizeof(struct SWFLineStyle_s));

	line->width = width;
	line->r = r;
	line->g = g;
	line->b = b;
	line->a = a;

	line->flags = 0 | flags;
	line->miterLimit = miterLimit;
	line->fill = NULL;	
	return line;
}


/*
 * create Linestyle2 introduce with SWF 8.
 * 
 * Instead of providing a fill color, a FillStyle can be applied
 * to a line.
 * 
 * Linestyle2 also extends Linestyle1 with some extra flags:
 *
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
SWFLineStyle newSWFLineStyle2_filled(unsigned short width, SWFFillStyle fill, 
                                     int flags, float miterLimit)
{
	SWFLineStyle line;

	if(fill == NULL)
		return NULL;

	line = (SWFLineStyle)malloc(sizeof(struct SWFLineStyle_s));
	line->width = width;
	line->flags = SWF_LINESTYLE_FLAG_FILL | flags;
	line->miterLimit = miterLimit;
	line->fill = fill;
	return line;
}

byte SWFLineStyle_equals(SWFLineStyle line, unsigned short width,
			 byte r, byte g, byte b, byte a, int flags)
{
//	if(line->width == 0 && width == 0)
//		return TRUE;

	if(line->width == width &&
		 line->r == r &&
		 line->g == g &&
		 line->b == b &&
		 line->a == a &&
                 line->flags == flags)
	{
		return TRUE;
	}

	return FALSE;
}

byte SWFLineStyle_equals2filled(SWFLineStyle line, unsigned short width,
                                SWFFillStyle fill, int flags)
{
	if(line->width == width &&
           line->flags == flags && 
           SWFFillStyle_equals(line->fill, fill))
	{
		return TRUE;
	}
	return FALSE;
}                               


unsigned short SWFLineStyle_getWidth(SWFLineStyle line)
{
	return line->width;
}


static inline void writeLineStyle1(SWFOutput out, SWFLineStyle line, int shapeType)
{
	SWFOutput_writeUInt16(out, line->width);
	SWFOutput_writeUInt8(out, line->r);
	SWFOutput_writeUInt8(out, line->g);
	SWFOutput_writeUInt8(out, line->b);

	if(shapeType >= SWF_DEFINESHAPE3)
		SWFOutput_writeUInt8(out, line->a);
}

static inline void writeLineStyle2(SWFOutput out, SWFLineStyle line, SWFBlocktype shapeType, SWFRect bounds)
{
	SWFOutput_writeUInt16(out, line->width);
	SWFOutput_writeUInt8(out, (line->flags >> 8));
	SWFOutput_writeUInt8(out, line->flags);
	if(line->flags & SWF_LINESTYLE_JOIN_MITER)
		SWFOutput_writeFixed8(out, line->miterLimit);
	if(line->flags & SWF_LINESTYLE_FLAG_FILL)
		SWFOutput_writeFillStyle(out, line->fill, shapeType, bounds);
	else
	{
		SWFOutput_writeUInt8(out, line->r);
		SWFOutput_writeUInt8(out, line->g);
		SWFOutput_writeUInt8(out, line->b);
		SWFOutput_writeUInt8(out, line->a);
	}
}

void SWFOutput_writeLineStyles(SWFOutput out,
                               SWFLineStyle *lines, int nLines,
                               SWFBlocktype shapeType,
                               SWFRect bounds)
{
	SWFLineStyle line;
	int i;
	
	if(nLines<255)
		SWFOutput_writeUInt8(out, nLines);
	else
	{
		SWFOutput_writeUInt8(out, 255);
		SWFOutput_writeUInt16(out, nLines);
	}
	
	for(i=0; i<nLines; ++i)
	{
		line = lines[i];
		if(shapeType == SWF_DEFINESHAPE4)
			writeLineStyle2(out, line, shapeType, bounds);
		else
			writeLineStyle1(out, line, shapeType);
	}
}


void SWFOutput_writeMorphLineStyles2(SWFOutput out,
		SWFLineStyle *lines1, int nLines1,
		SWFLineStyle *lines2, int nLines2)
{
	SWFLineStyle line1, line2;
	int i;

	SWF_assert(nLines1 == nLines2);

	if(nLines1<255)
		SWFOutput_writeUInt8(out, nLines1);
	else
	{
		SWFOutput_writeUInt8(out, 255);
		SWFOutput_writeUInt16(out, nLines1);
	}

	for(i=0; i<nLines1; ++i)
	{
		line1 = lines1[i];
		line2 = lines2[i];

		SWFOutput_writeUInt16(out, line1->width);
		SWFOutput_writeUInt16(out, line2->width);
		
		if(line1->flags != line2->flags)
			SWF_warnOnce("Morph: shapes _must_ us equal line flags\n");
		SWFOutput_writeUInt8(out, (line1->flags >> 8));
		SWFOutput_writeUInt8(out, line1->flags);

		if(line1->flags & SWF_LINESTYLE_JOIN_MITER)
			SWFOutput_writeFixed8(out, line1->miterLimit);
		if(line1->flags & SWF_LINESTYLE_FLAG_FILL)
			SWFOutput_writeMorphFillStyle(out, line1->fill, NULL, line2->fill, NULL);
		else
		{	

			SWFOutput_writeUInt8(out, line1->r);
			SWFOutput_writeUInt8(out, line1->g);
			SWFOutput_writeUInt8(out, line1->b);
			SWFOutput_writeUInt8(out, line1->a);
			SWFOutput_writeUInt8(out, line2->r);
			SWFOutput_writeUInt8(out, line2->g);
			SWFOutput_writeUInt8(out, line2->b);
			SWFOutput_writeUInt8(out, line2->a);
		}
	}
}


void SWFOutput_writeMorphLineStyles(SWFOutput out,
						SWFLineStyle *lines1, int nLines1,
						SWFLineStyle *lines2, int nLines2)
{
	SWFLineStyle line1, line2;
	int i;

	SWF_assert(nLines1 == nLines2);

	if(nLines1<255)
		SWFOutput_writeUInt8(out, nLines1);
	else
	{
		SWFOutput_writeUInt8(out, 255);
		SWFOutput_writeUInt16(out, nLines1);
	}

	for(i=0; i<nLines1; ++i)
	{
		line1 = lines1[i];
		line2 = lines2[i];

		SWFOutput_writeUInt16(out, line1->width);
		SWFOutput_writeUInt16(out, line2->width);
		SWFOutput_writeUInt8(out, line1->r);
		SWFOutput_writeUInt8(out, line1->g);
		SWFOutput_writeUInt8(out, line1->b);
		SWFOutput_writeUInt8(out, line1->a);
		SWFOutput_writeUInt8(out, line2->r);
		SWFOutput_writeUInt8(out, line2->g);
		SWFOutput_writeUInt8(out, line2->b);
		SWFOutput_writeUInt8(out, line2->a);
	}
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
