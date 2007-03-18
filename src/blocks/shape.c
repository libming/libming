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

#include "shape.h"
#include "character.h"
#include "matrix.h"
#include "fillstyle.h"
#include "linestyle.h"
#include "font.h"
#include "libming.h"

// SWF_DEFINESHAPE4 flags
#define SWF_SHAPE_USESCALINGSTROKES 	(1<<0)
#define SWF_SHAPE_USENONSCALINGSTREOKES	(1<<1)


struct stateChangeRecord
{
	int flags;
	int moveToX;
	int moveToY;
	int leftFill;
	int rightFill;
	int line;
	/* newstyle not used.. */
};
typedef struct stateChangeRecord *StateChangeRecord;


struct lineToRecord
{
	int dx;
	int dy;
};
typedef struct lineToRecord *LineToRecord;


struct curveToRecord
{
	int controlx;
	int controly;
	int anchorx;
	int anchory;
};
typedef struct curveToRecord *CurveToRecord;


typedef enum
{
	SHAPERECORD_STATECHANGE,
	SHAPERECORD_LINETO,
	SHAPERECORD_CURVETO
} shapeRecordType;


struct shapeRecord
{
	shapeRecordType type;

	union
	{
		StateChangeRecord stateChange;
		LineToRecord lineTo;
		CurveToRecord curveTo;
	} record;
};
typedef struct shapeRecord ShapeRecord;


struct SWFShape_s
{
	struct SWFCharacter_s character;

	ShapeRecord *records;
	int nRecords;
	SWFOutput out;
	int xpos;	/* cursor for using abs. coords in lineTo, curveTo */
	int ypos;
	SWFLineStyle *lines;
	SWFFillStyle *fills;
	byte nLines;
	byte nFills;
	short lineWidth;
	BOOL isMorph;
	BOOL isEnded;
	// SWF_DEFINESHAPE4 extensions
	unsigned char flags;
	SWFRect edgeBounds;
#if TRACK_ALLOCS
	/* memory node for garbage collection */
	mem_node *gcnode;
#endif
};


static void
SWFShape_writeShapeRecord(SWFShape shape, ShapeRecord record);


static void
writeSWFShapeBlockToMethod(SWFBlock block,
													 SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFShape)block)->out;
	SWFOutput_writeToMethod(out, method, data);
}


static int
completeSWFShapeBlock(SWFBlock block)
{
	SWFShape shape = (SWFShape)block;

	SWFShape_end(shape);
	
	return SWFOutput_getLength(shape->out);
}


void
destroySWFShape(SWFShape shape)
{
	int i;

	for ( i=0; i<shape->nFills; ++i )
	{
		SWFMatrix matrix = SWFFillStyle_getMatrix(shape->fills[i]);

		if ( matrix != NULL )
			destroySWFMatrix(matrix);

		/* gradients and bitmaps are destroyed separately */

		free(shape->fills[i]);
	}

	if ( shape->fills != NULL )
		free(shape->fills);

	for ( i=0; i<shape->nLines; ++i )
		free(shape->lines[i]);

	if ( shape->lines != NULL )
		free(shape->lines);

	destroySWFOutput(shape->out);

#if TRACK_ALLOCS
	ming_gc_remove_node(shape->gcnode);
#endif

	destroySWFCharacter((SWFCharacter) shape);
}


SWFShape
newSWFShape()
{
	SWFShape shape = (SWFShape)malloc(sizeof(struct SWFShape_s));

	SWFCharacterInit((SWFCharacter)shape);

	BLOCK(shape)->writeBlock = writeSWFShapeBlockToMethod;
	BLOCK(shape)->complete = completeSWFShapeBlock;
	BLOCK(shape)->dtor = (destroySWFBlockMethod) destroySWFShape;
	BLOCK(shape)->type = SWF_DEFINESHAPE3;
	
	CHARACTERID(shape) = ++SWF_gNumCharacters;

	shape->out = newSWFOutput();
	CHARACTER(shape)->bounds = newSWFRect(0,0,0,0);
	shape->edgeBounds = newSWFRect(0,0,0,0);

	shape->records = NULL;
	shape->lines = NULL;
	shape->fills = NULL;

	shape->nRecords = 0;
	shape->xpos = 0;
	shape->ypos = 0;
	shape->nLines = 0;
	shape->nFills = 0;
	shape->lineWidth = 0;
	shape->isMorph = FALSE;
	shape->isEnded = FALSE;
	shape->flags = 0;

	SWFOutput_writeUInt8(shape->out, 0); /* space for nFillBits, nLineBits */

#if TRACK_ALLOCS
	shape->gcnode = ming_gc_add_node(shape, (dtorfunctype) destroySWFShape);
#endif

	return shape;
}


/*
 * Creates a shape filled with bitmap
 */
SWFShape
newSWFShapeFromBitmap(SWFBitmap bitmap, int flag)
{
	SWFShape shape = newSWFShape();
	SWFFillStyle fill;
	int width, height;

	if ( flag != SWFFILL_TILED_BITMAP && flag != SWFFILL_CLIPPED_BITMAP)
	{
		SWF_error("Invalid bitmap fill flag");
	}

	fill = SWFShape_addBitmapFillStyle(shape, bitmap, flag);

	width = SWFBitmap_getWidth(bitmap);
	height = SWFBitmap_getHeight(bitmap);

	SWFShape_setRightFillStyle(shape, fill);

	// XXX - scale shouldn't be hardcoded! (here, or in newSWFBitmapFillStyle)
	SWFShape_drawScaledLine(shape, width * 20, 0);
	SWFShape_drawScaledLine(shape, 0, height * 20);
	SWFShape_drawScaledLine(shape, -width * 20, 0);
	SWFShape_drawScaledLine(shape, 0, -height * 20);

	return shape;
}


void
SWFShape_end(SWFShape shape)
{
	int i;
	byte* buffer;

	if ( shape->isEnded )
		return;

	shape->isEnded = TRUE;

	buffer = SWFOutput_getBuffer(shape->out);

	buffer[0] =
		(SWFOutput_numBits(shape->nFills) << 4) + SWFOutput_numBits(shape->nLines);

	for ( i=0; i<shape->nRecords; ++i )
	{
		if ( i < shape->nRecords-1 ||
				 shape->records[i].type != SHAPERECORD_STATECHANGE )
		{
			SWFShape_writeShapeRecord(shape, shape->records[i]);
		}

		free(shape->records[i].record.stateChange); /* all in union are pointers */
	}

	SWFOutput_writeBits(shape->out, 0, 6); /* end tag */
	SWFOutput_byteAlign(shape->out);

	/* addStyleHeader creates a new output and adds the existing one after
		 itself- so even though it's called afterwards it's written before,
		 as it should be */

	if ( BLOCK(shape)->type > 0 ) /* i.e., shape with style */
		SWFShape_addStyleHeader(shape);

	free(shape->records);
	shape->records = NULL;
	shape->nRecords = 0;
}


SWFOutput
SWFShape_getOutput(SWFShape shape)
{
	return shape->out;
}


void
SWFShape_getFills(SWFShape shape, SWFFillStyle** fills, int* nFills)
{
	*fills = shape->fills;
	*nFills = shape->nFills;
}


void
SWFShape_getLines(SWFShape shape, SWFLineStyle** lines, int* nLines)
{
	*lines = shape->lines;
	*nLines = shape->nLines;
}


void
SWFShape_setMorphFlag(SWFShape shape)
{
	shape->isMorph = TRUE;
}


void
SWFShape_addStyleHeader(SWFShape shape)
{
	SWFOutput out = newSWFOutput();

	SWFOutput_writeUInt16(out, CHARACTERID(shape));
	SWFOutput_writeRect(out, SWFCharacter_getBounds(CHARACTER(shape)));
	if(BLOCK(shape)->type == SWF_DEFINESHAPE4)
	{
		SWFOutput_writeRect(out, shape->edgeBounds);
		SWFOutput_writeUInt8(out, shape->flags);
	}
	SWFOutput_writeFillStyles(out, shape->fills, shape->nFills, BLOCK(shape)->type);
	SWFOutput_writeLineStyles(out, shape->lines, shape->nLines, BLOCK(shape)->type);
	
	/* prepend shape->out w/ shape header */
	SWFOutput_setNext(out, shape->out);
	shape->out = out;
}


/*
	ShapeRecords are an intermediate storage so that we don't have to specify
	fill/line types in advance.
*/

#define SHAPERECORD_INCREMENT 32

static ShapeRecord
newShapeRecord(SWFShape shape, shapeRecordType type)
{
	if ( shape->nRecords % SHAPERECORD_INCREMENT == 0 )
	{
		shape->records = (ShapeRecord*) realloc(shape->records,
					 sizeof(ShapeRecord) *
					 (shape->nRecords + SHAPERECORD_INCREMENT));
	}

	switch ( type )
	{
		case SHAPERECORD_STATECHANGE:
		{
			StateChangeRecord change = (StateChangeRecord)calloc(1,sizeof(struct stateChangeRecord));
			shape->records[shape->nRecords].record.stateChange = change;
			break;
		}
		case SHAPERECORD_LINETO:
		{
			LineToRecord lineTo = (LineToRecord) calloc(1,sizeof(struct lineToRecord));
			shape->records[shape->nRecords].record.lineTo = lineTo;
			break;
		}
		case SHAPERECORD_CURVETO:
		{
			CurveToRecord curveTo = (CurveToRecord) calloc(1,sizeof(struct curveToRecord));
			shape->records[shape->nRecords].record.curveTo = curveTo;
			break;
		}
	}

	shape->records[shape->nRecords].type = type;

// this is intentional - at least one popular compiler cannot handle [shape->nRecords++]
	shape->nRecords++;
	return shape->records[shape->nRecords-1];
}


void
SWFShape_writeShapeRecord(SWFShape shape, ShapeRecord record)
{
	SWFOutput out = shape->out;

	switch(record.type)
	{
		case SHAPERECORD_STATECHANGE:
		{
			int flags = record.record.stateChange->flags;

			if(flags == 0)
				return;

			SWFOutput_writeBits(out, flags, 6);

			if(flags & SWF_SHAPE_MOVETOFLAG)
			{
				int x = record.record.stateChange->moveToX;
				int y = record.record.stateChange->moveToY;
				int nBits = max(SWFOutput_numSBits(x), SWFOutput_numSBits(y));

				SWF_assert(nBits<32);
				SWFOutput_writeBits(out, nBits, 5);
				SWFOutput_writeSBits(out, x, nBits);
				SWFOutput_writeSBits(out, y, nBits);
			}

			if(flags & SWF_SHAPE_FILLSTYLE0FLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->leftFill,
														SWFOutput_numBits(shape->nFills));
			}

			if(flags & SWF_SHAPE_FILLSTYLE1FLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->rightFill,
														SWFOutput_numBits(shape->nFills));
			}

			if(flags & SWF_SHAPE_LINESTYLEFLAG)
			{
				SWFOutput_writeBits(out, record.record.stateChange->line,
														SWFOutput_numBits(shape->nLines));
			}

			/* newstyle's never used.	 But this is what it looks like:

			if ( flags & SWF_SHAPE_NEWSTYLEFLAG )
			{
				SWFOutput_writeFillStyles(shape->out, shape->fills, shape->nFills,
				BLOCK(shape)->type);

				SWFOutput_writeLineStyles(shape->out, shape->lines, shape->nLines,
					BLOCK(shape)->type);

				SWFOutput_writeBits(shape->out, SWFOutput_numBits(shape->nFills), 4);
				SWFOutput_writeBits(shape->out, SWFOutput_numBits(shape->nLines), 4);
			}

			*/

			break;
		}

		case SHAPERECORD_LINETO:
		{
			int nBits;
			int dx = record.record.lineTo->dx;
			int dy = record.record.lineTo->dy;

			SWFOutput_writeBits(out, 3, 2); /* straight edge */

			if(dx==0)
			{
				nBits = SWFOutput_numSBits(dy);
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 1, 2); /* vertical line */
				SWFOutput_writeSBits(out, dy, nBits);
			}
			else if(dy==0)
			{
				nBits = SWFOutput_numSBits(dx);
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 0, 2); /* horizontal line */
				SWFOutput_writeSBits(out, dx, nBits);
			}
			else
			{
				nBits = max(SWFOutput_numSBits(dx), SWFOutput_numSBits(dy));
				SWF_assert(nBits<18);
				SWFOutput_writeBits(out, nBits-2, 4);
				SWFOutput_writeBits(out, 1, 1); /* general line */
				SWFOutput_writeSBits(out, dx, nBits);
				SWFOutput_writeSBits(out, dy, nBits);
			}

			break;
		}

		case SHAPERECORD_CURVETO:
		{
			int controlx = record.record.curveTo->controlx;
			int controly = record.record.curveTo->controly;
			int anchorx = record.record.curveTo->anchorx;
			int anchory = record.record.curveTo->anchory;

			int nBits = max(max(SWFOutput_numSBits(controlx),
													SWFOutput_numSBits(controly)),
											max(SWFOutput_numSBits(anchorx),
													SWFOutput_numSBits(anchory)));

			if ( nBits < 2 )
				nBits = 2;

			SWF_assert(nBits < 18);

			SWFOutput_writeBits(out, 2, 2); /* curved edge */
			SWFOutput_writeBits(out, nBits-2, 4);
			SWFOutput_writeSBits(out, controlx, nBits);
			SWFOutput_writeSBits(out, controly, nBits);
			SWFOutput_writeSBits(out, anchorx, nBits);
			SWFOutput_writeSBits(out, anchory, nBits);

			break;
		}

		default:
			SWF_error("Unknown shapeRecordType");
	}
}


/* x,y relative to shape origin */
void
SWFShape_drawScaledLineTo(SWFShape shape, int x, int y)
{
	SWFShape_drawScaledLine(shape, x-shape->xpos, y-shape->ypos);
}


void
SWFShape_drawScaledLine(SWFShape shape, int dx, int dy)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( dx == 0 && dy == 0 )
		return;

	record = newShapeRecord(shape, SHAPERECORD_LINETO);

	SWF_assert(SWFOutput_numSBits(dx) < 18);
	SWF_assert(SWFOutput_numSBits(dy) < 18);

	record.record.lineTo->dx = dx;
	record.record.lineTo->dy = dy;

	shape->xpos += dx;
	shape->ypos += dy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
}


void
SWFShape_drawScaledCurveTo(SWFShape shape,
													 int controlx, int controly,
													 int anchorx, int anchory)
{
	SWFShape_drawScaledCurve(shape, controlx-shape->xpos, controly-shape->ypos,
													 anchorx-controlx, anchory-controly);
}


void
SWFShape_drawScaledCurve(SWFShape shape,
												 int controldx, int controldy,
												 int anchordx, int anchordy)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( controldx == 0 && controldy == 0 && anchordx == 0 && anchordy == 0 )
		return;

	record = newShapeRecord(shape, SHAPERECORD_CURVETO);

	record.record.curveTo->controlx = controldx;
	record.record.curveTo->controly = controldy;
	record.record.curveTo->anchorx = anchordx;
	record.record.curveTo->anchory = anchordy;

	if ( SWFOutput_numSBits(controldx) >= 18 ||
			 SWFOutput_numSBits(controldy) >= 18 ||
			 SWFOutput_numSBits(anchordx) >= 18 ||
			 SWFOutput_numSBits(anchordy) >= 18 )
		SWF_error("Curve parameters too large");

	/* including the control point is sloppy, but safe.. */

	shape->xpos += controldx;
	shape->ypos += controldy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
	shape->xpos += anchordx;
	shape->ypos += anchordy;

	SWFRect_includePoint(SWFCharacter_getBounds(CHARACTER(shape)),
											 shape->xpos, shape->ypos, shape->lineWidth);
	SWFRect_includePoint(shape->edgeBounds, shape->xpos, shape->ypos, 0);
}


#define STYLE_INCREMENT 4

static inline void growLineArray(SWFShape shape)
{
	int size;

	if ( shape->nLines % STYLE_INCREMENT != 0 )
		return;

	size = (shape->nLines+STYLE_INCREMENT) * sizeof(SWFLineStyle);
	shape->lines = (SWFLineStyle*)realloc(shape->lines, size);	
}

static int 
SWFShape_addLineStyle2filled(SWFShape shape, unsigned short width,
                             SWFFillStyle fill,
                             int flags, float miterLimit)
{
	growLineArray(shape);
	BLOCK(shape)->type = SWF_DEFINESHAPE4;
	shape->lines[shape->nLines] = newSWFLineStyle2_filled(width, fill, flags, miterLimit);
	return ++shape->nLines;
}

static int
SWFShape_addLineStyle2(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a,
                      int flags, float miterLimit)
{
	growLineArray(shape);
	BLOCK(shape)->type = SWF_DEFINESHAPE4;
	shape->lines[shape->nLines] = newSWFLineStyle2(width, r, g, b, a, flags, miterLimit);
	return ++shape->nLines;
}

static int
SWFShape_addLineStyle(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a)
{
	growLineArray(shape);
	shape->lines[shape->nLines] = newSWFLineStyle(width, r, g, b, a);
	return ++shape->nLines;
}


/* if the current shape record isn't a style change record, add one */
static ShapeRecord
addStyleRecord(SWFShape shape)
{
	if ( shape->nRecords > 0 &&
			 shape->records[shape->nRecords-1].type == SHAPERECORD_STATECHANGE )
	{
		return shape->records[shape->nRecords-1];
	}
	else
		return newShapeRecord(shape, SHAPERECORD_STATECHANGE);
}


void
SWFShape_hideLine(SWFShape shape)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	if ( shape->isMorph )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->line = 0;
	record.record.stateChange->flags |= SWF_SHAPE_LINESTYLEFLAG;
}

static void finishSetLine(SWFShape shape, int line, unsigned short width)
{
	ShapeRecord record;
	
	if ( width == 0 )
		shape->lineWidth = 0;
	else
		shape->lineWidth = (SWFLineStyle_getWidth(shape->lines[line-1]) + 1) / 2;

	if ( shape->isMorph )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->line = line;
	record.record.stateChange->flags |= SWF_SHAPE_LINESTYLEFLAG;
}

/*
 * set filled Linestyle2 introduce with SWF 8.
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
void 
SWFShape_setLineStyle2filled(SWFShape shape, unsigned short width,
                       SWFFillStyle fill,
                       int flags, float miterLimit)
{
	int line;

	if ( shape->isEnded )
		return;

	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals2filled(shape->lines[line], width, fill, flags) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle2filled(shape, width, fill, flags, miterLimit);
	else
		++line;

	finishSetLine(shape, line, width);
}



/*
 * set Linestyle2 introduce with SWF 8.
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
void 
SWFShape_setLineStyle2(SWFShape shape, unsigned short width,
                       byte r, byte g, byte b, byte a,
                       int flags, float miterLimit)
{
	int line;

	if ( shape->isEnded )
		return;

	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals(shape->lines[line], width, r, g, b, a, flags) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle2(shape, width, r, g, b, a, flags, miterLimit);
	else
		++line;

	finishSetLine(shape, line, width);
}

void
SWFShape_setLineStyle(SWFShape shape, unsigned short width,
                      byte r, byte g, byte b, byte a)
{
	int line;
	
	if ( shape->isEnded )
		return;

	for ( line=0; line<shape->nLines; ++line )
	{
		if ( SWFLineStyle_equals(shape->lines[line], width, r, g, b, a, 0) )
			break;
	}

	if ( line == shape->nLines )
		line = SWFShape_addLineStyle(shape, width, r, g, b, a);
	else
		++line;
	
	finishSetLine(shape, line, width);
}


/* fill 0 is no fill, so set fill->idx to one more than the shape's fill index */
static SWFFillStyle
addFillStyle(SWFShape shape, SWFFillStyle fill)
{
	int i;

	for ( i=0; i<shape->nFills; ++i )
	{
		if ( SWFFillStyle_equals(fill, shape->fills[i]) )
		{
			free(fill); // XXX - safe?
			return shape->fills[i];
		}
	}

	if ( shape->isEnded )
	{
		SWFFill_setIdx(fill, 0);
		return fill;
	}

	if ( shape->nFills%STYLE_INCREMENT == 0 )
	{
		shape->fills =
			(SWFFillStyle*)realloc(shape->fills,
							(shape->nFills+STYLE_INCREMENT) * sizeof(SWFFillStyle));
	}

	SWFFill_setIdx(fill, shape->nFills+1);

	shape->fills[shape->nFills] = fill;
	++shape->nFills;

	return fill;
}


SWFFillStyle
SWFShape_addSolidFillStyle(SWFShape shape, byte r, byte g, byte b, byte a)
{
	return addFillStyle(shape, newSWFSolidFillStyle(r, g, b, a));
}


SWFFillStyle
SWFShape_addGradientFillStyle(SWFShape shape, SWFGradient gradient, byte flags)
{
	return addFillStyle(shape, newSWFGradientFillStyle(gradient, flags));
}


SWFFillStyle
SWFShape_addBitmapFillStyle(SWFShape shape, SWFBitmap bitmap, byte flags)
{
	SWFCharacter_addDependency((SWFCharacter)shape, (SWFCharacter)bitmap);

	return addFillStyle(shape, newSWFBitmapFillStyle(bitmap, flags));
}


void
SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill)
{
	ShapeRecord record;

	if ( shape->isEnded || shape->isMorph )
		return;

	record = addStyleRecord(shape);

	if ( fill != NOFILL )
	{
		if ( SWFFill_getIdx(fill) > shape->nFills )
			SWF_error("Invalid fill idx");

		record.record.stateChange->leftFill = SWFFill_getIdx(fill);
	}
	else
		record.record.stateChange->leftFill = 0;

	record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE0FLAG;
}


void
SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill)
{
	ShapeRecord record;

	if ( shape->isEnded || shape->isMorph )
		return;

	record = addStyleRecord(shape);

	if ( fill != NOFILL )
	{
		if ( SWFFill_getIdx(fill) > shape->nFills )
			SWF_error("Invalid fill idx");

		record.record.stateChange->rightFill = SWFFill_getIdx(fill);
	}
	else
		record.record.stateChange->rightFill = 0;

	record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE1FLAG;
}


/* move pen relative to shape origin */
void
SWFShape_moveScaledPenTo(SWFShape shape, int x, int y)
{
	ShapeRecord record;

	if ( shape->isEnded )
		return;

	record = addStyleRecord(shape);

	record.record.stateChange->moveToX = shape->xpos = x;
	record.record.stateChange->moveToY = shape->ypos = y;

	record.record.stateChange->flags |= SWF_SHAPE_MOVETOFLAG;

	if ( shape->nRecords == 0 ||
			 (shape->nRecords == 1 &&
				shape->records[0].type == SHAPERECORD_STATECHANGE) )
	{
		SWFRect_setBounds(SWFCharacter_getBounds(CHARACTER(shape)), x, x, y, y);
		SWFRect_setBounds(shape->edgeBounds, x, x, y, y);
	}
}


void
SWFShape_moveScaledPen(SWFShape shape, int x, int y)
{
	SWFShape_moveScaledPenTo(shape, shape->xpos+x, shape->ypos+y);
}


int
SWFShape_getScaledPenX(SWFShape shape)
{
	return shape->xpos;
}


int
SWFShape_getScaledPenY(SWFShape shape)
{
	return shape->ypos;
}


/* yes, this is a total hack. */

#include "read.c"

void
SWFShape_drawScaledGlyph(SWFShape shape,
												 SWFFont font, unsigned short c, int size)
{
	byte *p = SWFFont_findGlyph(font, c);
	byte **f = &p;

	int moveBits, x=0, y=0;
	int straight, numBits;
	int numFillBits, numLineBits;

	/* moveTos in the record are absolute, but we want to draw from the current
		 location. grr. */

	int startX = shape->xpos;
	int startY = shape->ypos;
	int style;

	byteAlign();

	if ( (numFillBits = readBitsP(f, 4)) != 1 ) /* fill bits */
		SWF_error("SWFShape_drawGlyph: bad file format (was expecting fill bits = 1)");

	if ( (numLineBits = readBitsP(f, 4)) > 1 ) /* line bits */
		SWF_error("SWFShape_drawGlyph: bad file format (was expecting line bits = 0)");

	/* now we get to parse the shape commands.	Oh boy.
		 the first one will be a non-edge block- grab the moveto loc */

	readBitsP(f, 2); /* type 0, newstyles */
	style = readBitsP(f, 3);

	if(readBitsP(f, 1))
	{	moveBits = readBitsP(f, 5);
		x = startX + readSBitsP(f, moveBits);
		y = startY + readSBitsP(f, moveBits);
	}
	else if(style == 0)	/* no style, no move => space character */
		return;

	SWFShape_moveScaledPenTo(shape, x*size/1024, y*size/1024);

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

			SWFShape_moveScaledPenTo(shape, x*size/1024, y*size/1024);

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

			SWFShape_drawScaledLineTo(shape, x*size/1024, y*size/1024);
		}
		else
		{
			int controlX = readSBitsP(f, numBits);
			int controlY = readSBitsP(f, numBits);
			int anchorX = readSBitsP(f, numBits);
			int anchorY = readSBitsP(f, numBits);

			SWFShape_drawScaledCurveTo(shape,
				 (x+controlX)*size/1024,
				 (y+controlY)*size/1024,
				 (x+controlX+anchorX)*size/1024,
				 (y+controlY+anchorY)*size/1024);

			x += controlX + anchorX;
			y += controlY + anchorY;
		}
	}

	/* no idea where the pen was left */
	SWFShape_moveScaledPenTo(shape, startX, startY);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
