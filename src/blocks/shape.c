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

#include "shape.h"

#define STYLE_INCREMENT 4

void writeSWFShapeBlockToMethod(SWFBlock block,
				SWFByteOutputMethod method, void *data)
{
  SWFOutput out = ((SWFShape)block)->out;
  SWFOutput_writeToMethod(out, method, data);
}
int completeSWFShapeBlock(SWFBlock block)
{
  SWFShape shape = (SWFShape)block;

  if(!shape->isEnded)
    SWFShape_end(shape);

  return SWFOutput_length(shape->out);
}
void destroySWFShape(SWFBlock block)
{
  SWFShape shape = (SWFShape)block;
  int i;

  SWFCharacter_clearDependencies(CHARACTER(block));

  for(i=0; i<shape->nFills; ++i)
  {
    if(shape->fills[i]->matrix != NULL)
      destroySWFMatrix(shape->fills[i]->matrix);

    /* gradients and bitmaps are destroyed separately */

    free(shape->fills[i]);
  }

  free(shape->fills);

  for(i=0; i<shape->nLines; ++i)
    free(shape->lines[i]);

  free(shape->lines);

  destroySWFRect(CHARACTER(shape)->bounds);
  destroySWFOutput(shape->out);
  free(shape);
}
SWFShape newSWFShape()
{
  SWFShape shape = calloc(1, SHAPE_SIZE);

  BLOCK(shape)->writeBlock = writeSWFShapeBlockToMethod;
  BLOCK(shape)->complete = completeSWFShapeBlock;
  BLOCK(shape)->dtor = destroySWFShape;
  BLOCK(shape)->type = SWF_DEFINESHAPE3;
  CHARACTERID(shape) = ++SWF_gNumCharacters;
  shape->out = newSWFOutput();
  CHARACTER(shape)->bounds = newSWFRect(0,0,0,0);

  shape->records = NULL;
  shape->lines = NULL;
  shape->fills = NULL;

  SWFOutput_writeUInt8(shape->out, 0); /* space for nFillBits, nLineBits */

  return shape;
}

void SWFShape_writeShapeRecord(SWFShape shape, ShapeRecord record);

void SWFShape_end(SWFShape shape)
{
  int i;

  shape->isEnded = TRUE;

  shape->out->buffer[0] = (SWFOutput_numBits(shape->nFills)<<4) + SWFOutput_numBits(shape->nLines);

  for(i=0; i<shape->nRecords; ++i)
  {
    if(!(i == shape->nRecords-1 &&
	 shape->records[i].type == SHAPERECORD_STATECHANGE))
      SWFShape_writeShapeRecord(shape, shape->records[i]);

    free(shape->records[i].record.stateChange); /* all in union are pointers */
  }

  SWFOutput_writeBits(shape->out, 0, 6); /* end tag */
  SWFOutput_byteAlign(shape->out);

  /* addStyleHeader creates a new output and adds the existing one after
     itself- so even though it's called afterwards it's written before,
     as it should be */

  if(BLOCK(shape)->type > 0) /* i.e., shape with style */
    SWFShape_addStyleHeader(shape);

  free(shape->records);
  shape->nRecords = 0;
}

void SWFShape_addStyleHeader(SWFShape shape)
{
  SWFOutput out = newSWFOutput();

  SWFOutput_writeUInt16(out, CHARACTERID(shape));
  SWFOutput_writeRect(out, CHARACTER(shape)->bounds);
  SWFOutput_writeFillStyles(out, shape->fills, shape->nFills, BLOCK(shape)->type);
  SWFOutput_writeLineStyles(out, shape->lines, shape->nLines, BLOCK(shape)->type);

  /* prepend shape->out w/ shape header */
  out->next = shape->out;
  shape->out = out;
}


/*
  ShapeRecords are an intermediate storage so that we don't have to specify
  fill/line types in advance.
*/

#define SHAPERECORD_INCREMENT 32

inline ShapeRecord newShapeRecord(SWFShape shape, shapeRecordType type)
{
  if(shape->nRecords % SHAPERECORD_INCREMENT == 0)
    shape->records = realloc(shape->records,
			     sizeof(ShapeRecord) *
			     (shape->nRecords + SHAPERECORD_INCREMENT));

  switch(type)
  {
    case SHAPERECORD_STATECHANGE:
    {
      StateChangeRecord change = calloc(1, sizeof(struct _stateChangeRecord));
      shape->records[shape->nRecords].record.stateChange = change;
      break;
    }
    case SHAPERECORD_LINETO:
    {
      LineToRecord lineTo = calloc(1, sizeof(struct _lineToRecord));
      shape->records[shape->nRecords].record.lineTo = lineTo;
      break;
    }
    case SHAPERECORD_CURVETO:
    {
      CurveToRecord curveTo = calloc(1, sizeof(struct _curveToRecord));
      shape->records[shape->nRecords].record.curveTo = curveTo;
      break;
    }
  }

  shape->records[shape->nRecords].type = type;
  return shape->records[shape->nRecords++];
}

void SWFShape_writeShapeRecord(SWFShape shape, ShapeRecord record)
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
        SWFOutput_writeBits(out, record.record.stateChange->leftFill,
			    SWFOutput_numBits(shape->nFills));

      if(flags & SWF_SHAPE_FILLSTYLE1FLAG)
        SWFOutput_writeBits(out, record.record.stateChange->rightFill,
			    SWFOutput_numBits(shape->nFills));

      if(flags & SWF_SHAPE_LINESTYLEFLAG)
        SWFOutput_writeBits(out, record.record.stateChange->line,
			    SWFOutput_numBits(shape->nLines));

      /* newstyle's never used.  But this is what it looks like:

      if(flags & SWF_SHAPE_NEWSTYLEFLAG)
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

      if(nBits<2)
	nBits = 2;

      SWF_assert(nBits<18);

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
void SWFShape_drawScaledLineTo(SWFShape shape, int x, int y)
{
  SWFShape_drawScaledLine(shape, x-shape->xpos, y-shape->ypos);
}
void SWFShape_drawScaledLine(SWFShape shape, int dx, int dy)
{
  ShapeRecord record;

  if(shape->isEnded)
    return;

  if(dx == 0 && dy == 0)
    return;

  record = newShapeRecord(shape, SHAPERECORD_LINETO);

  record.record.lineTo->dx = dx;
  record.record.lineTo->dy = dy;

  shape->xpos += dx;
  shape->ypos += dy;

  SWFRect_includePoint(CHARACTER(shape)->bounds,
		       shape->xpos, shape->ypos, shape->lineWidth);
}

void SWFShape_drawScaledCurveTo(SWFShape shape, int controlx, int controly,
				int anchorx, int anchory)
{
  SWFShape_drawScaledCurve(shape, controlx-shape->xpos, controly-shape->ypos,
			   anchorx-controlx, anchory-controly);
}

void SWFShape_drawScaledCurve(SWFShape shape, int controldx, int controldy,
			int anchordx, int anchordy)
{
  ShapeRecord record;

  if(shape->isEnded)
    return;

  if(controldx == 0 && controldy == 0 &&
     anchordx == 0 && anchordy == 0)
    return;

  record = newShapeRecord(shape, SHAPERECORD_CURVETO);

  record.record.curveTo->controlx = controldx;
  record.record.curveTo->controly = controldy;
  record.record.curveTo->anchorx = anchordx;
  record.record.curveTo->anchory = anchordy;

  /* including the control point is sloppy, but safe -
     figuring the real bounds requires a couple of sqrt ops.. */

  shape->xpos += controldx;
  shape->ypos += controldy;

  SWFRect_includePoint(CHARACTER(shape)->bounds,
		       shape->xpos, shape->ypos, shape->lineWidth);

  shape->xpos += anchordx;
  shape->ypos += anchordy;

  SWFRect_includePoint(CHARACTER(shape)->bounds,
		       shape->xpos, shape->ypos, shape->lineWidth);
}

static int SWFShape_addLineStyle(SWFShape shape, unsigned short width,
				 byte r, byte g, byte b, byte a)
{
  if(shape->nLines%STYLE_INCREMENT == 0)
    shape->lines = realloc(shape->lines,
			   (shape->nLines+STYLE_INCREMENT) *
			   sizeof(SWFLineStyle));

  shape->lines[shape->nLines] = newSWFLineStyle(width, r, g, b, a);

  return ++shape->nLines;
}

/* if the current shape record isn't a style change record, add one */
static inline ShapeRecord addStyleRecord(SWFShape shape)
{
  if(shape->nRecords > 0 &&
     shape->records[shape->nRecords-1].type == SHAPERECORD_STATECHANGE)
    return shape->records[shape->nRecords-1];
  else
    return newShapeRecord(shape, SHAPERECORD_STATECHANGE);
}

void SWFShape_setLineStyle(SWFShape shape, unsigned short width,
			   byte r, byte g, byte b, byte a)
{
  ShapeRecord record;
  int line;

  if(shape->isEnded)
    return;

  for(line=0; line<shape->nLines; ++line)
  {
    if(SWFLineStyle_equals(shape->lines[line], width, r, g, b, a))
      break;
  }

  if(line == shape->nLines)
    line = SWFShape_addLineStyle(shape, width, r, g, b, a);
  else
    ++line;

  if(width == 0)
    shape->lineWidth = 0;
  else
    shape->lineWidth = (shape->lines[line-1]->width+1)/2;

  if(shape->isMorph)
    return;

  record = addStyleRecord(shape);

  record.record.stateChange->line = line;
  record.record.stateChange->flags |= SWF_SHAPE_LINESTYLEFLAG;
}

/* fill 0 is no fill, so set fill->idx to one more than the shape's fill index */
static SWFFillStyle addFillStyle(SWFShape shape, SWFFillStyle fill)
{
  int i;

  for(i=0; i<shape->nFills; ++i)
  {
    if(SWFFillStyle_equals(fill, shape->fills[i]))
    {
      free(fill);
      return shape->fills[i];
    }
  }

  if(shape->isEnded)
  {
    fill->idx = 0;
    return fill;
  }

  if(shape->nFills%STYLE_INCREMENT == 0)
    shape->fills = realloc(shape->fills,
			   (shape->nFills+STYLE_INCREMENT) *
			   sizeof(SWFFillStyle));

  fill->idx = shape->nFills+1;
  shape->fills[shape->nFills] = fill;
  ++shape->nFills;

  return fill;
}

SWFFillStyle SWFShape_addSolidFillStyle(SWFShape shape,
					byte r, byte g, byte b, byte a)
{
  SWFFillStyle fill = calloc(1, FILLSTYLE_SIZE);

  fill->type = SWF_FILL_SOLID;
  fill->data.solid.r = r;
  fill->data.solid.g = g;
  fill->data.solid.b = b;
  fill->data.solid.a = a;
  fill->matrix = NULL; /* make purify shut up */

  return addFillStyle(shape, fill);
}

SWFFillStyle SWFShape_addGradientFillStyle(SWFShape shape,
					   SWFGradient gradient, byte flags)
{
  SWFFillStyle fill = calloc(1, FILLSTYLE_SIZE);

  if(flags == SWF_FILL_RADIAL_GRADIENT)
    fill->type = SWF_FILL_RADIAL_GRADIENT;
  else
    fill->type = SWF_FILL_LINEAR_GRADIENT;

  fill->data.gradient = gradient;
  fill->matrix = newSWFMatrix(1.0, 0, 0, 1.0, 0, 0);

  return addFillStyle(shape, fill);
}

SWFFillStyle SWFShape_addBitmapFillStyle(SWFShape shape,
					 SWFBitmap bitmap, byte flags)
{
  SWFFillStyle fill = calloc(1, FILLSTYLE_SIZE);

  if(flags == SWF_FILL_CLIPPED_BITMAP)
    fill->type = SWF_FILL_CLIPPED_BITMAP;
  else
    fill->type = SWF_FILL_TILED_BITMAP;

  fill->data.bitmap = bitmap;
  fill->matrix = newSWFMatrix(20.0, 0, 0, 20.0, 0, 0);

  SWFCharacter_addDependency((SWFCharacter)shape, (SWFBlock)bitmap);

  return addFillStyle(shape, fill);
}

void SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill)
{
  ShapeRecord record;

  if(shape->isEnded || shape->isMorph)
    return;

  record = addStyleRecord(shape);

  if(fill != NOFILL)
  {
    SWF_assert(fill->idx <= shape->nFills);
    record.record.stateChange->leftFill = fill->idx;
  }
  else
    record.record.stateChange->leftFill = 0;

  record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE0FLAG;
}

void SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill)
{
  ShapeRecord record;

  if(shape->isEnded || shape->isMorph)
    return;

  record = addStyleRecord(shape);

  if(fill != NOFILL)
  {
    SWF_assert(fill->idx <= shape->nFills);
    record.record.stateChange->rightFill = fill->idx;
  }
  else
    record.record.stateChange->rightFill = 0;

  record.record.stateChange->flags |= SWF_SHAPE_FILLSTYLE1FLAG;
}


/* move pen relative to shape origin */
void SWFShape_moveScaledPenTo(SWFShape shape, int x, int y)
{
  ShapeRecord record;

  if(shape->isEnded)
    return;

  record = addStyleRecord(shape);

  record.record.stateChange->moveToX = shape->xpos = x;
  record.record.stateChange->moveToY = shape->ypos = y;
  record.record.stateChange->flags |= SWF_SHAPE_MOVETOFLAG;

  if(shape->nRecords == 0 ||
     (shape->nRecords == 1 &&
      shape->records[0].type == SHAPERECORD_STATECHANGE))
  {
    CHARACTER(shape)->bounds->minX = CHARACTER(shape)->bounds->maxX = x;
    CHARACTER(shape)->bounds->minY = CHARACTER(shape)->bounds->maxY = y;
  }
}

void SWFShape_moveScaledPen(SWFShape shape, int x, int y)
{
  SWFShape_moveScaledPenTo(shape, shape->xpos+x, shape->ypos+y);
}

int SWFShape_getScaledPenX(SWFShape shape)
{
  return shape->xpos;
}

int SWFShape_getScaledPenY(SWFShape shape)
{
  return shape->ypos;
}


/* yes, this is a total hack. */

#include "read.c"

void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned char c)
{
  byte *p = SWFFont_findCharacterGlyph(font, c);
  byte **f = &p;

  int moveBits, x, y;
  int straight, numBits;

  /* moveTos in the record are absolute, but we want to draw from the current
     location. grr. */

  int startX = shape->xpos;
  int startY = shape->ypos;

  byteAlign();

  if(readBits(f, 4) != 1) /* fill bits */
    SWF_error("SWFShape_drawGlyph: was expecting fill bits = 1");

  if(readBits(f, 4) != 0) /* line bits */
    SWF_error("SWFShape_drawGlyph: was expecting line bits = 0");

  /* now we get to parse the shape commands.  Oh boy. */
  /* the first one will be a non-edge block- grab the moveto loc */

  readBits(f, 6); /* type 0, etc. */

  moveBits = readBits(f, 5);
  x = readSBits(f, moveBits);
  y = readSBits(f, moveBits);

  SWFShape_moveScaledPenTo(shape, x+startX, y+startY);

  if(readBits(f, 1) != 1) /* fill1 = 1 */
    SWF_error("SWFShape_drawCharacter says: oops.  Was expecting fill1 = 1.");

  /* translate the glyph's shape records into drawing commands */

  for(;;)
  {
    if(readBits(f, 1) == 0)
    {
      /* it's a moveTo or a shape end */

      if(readBits(f, 5) == 0)
	break;

      moveBits = readBits(f, 5);
      x = readSBits(f, moveBits);
      y = readSBits(f, moveBits);

      SWFShape_moveScaledPenTo(shape, x+startX, y+startY);
      continue;
    }

    straight = readBits(f, 1);
    numBits = readBits(f, 4)+2;

    if(straight==1)
    {
      if(readBits(f, 1)) /* general line */
      {
	x = readSBits(f, numBits);
	y = readSBits(f, numBits);

	SWFShape_drawScaledLine(shape, x, y);
      }
      else
	if(readBits(f, 1)) /* vert = 1 */
	  SWFShape_drawScaledLine(shape, 0, readSBits(f, numBits));
	else
	  SWFShape_drawScaledLine(shape, readSBits(f, numBits), 0);
    }
    else
    {
      int controlX = readSBits(f, numBits);
      int controlY = readSBits(f, numBits);
      int anchorX = readSBits(f, numBits);
      int anchorY = readSBits(f, numBits);

      SWFShape_drawScaledCurve(shape, controlX, controlY, anchorX, anchorY);
    }
  }

  /* no idea where the pen was left */
  SWFShape_moveScaledPenTo(shape, startX, startY);
}
