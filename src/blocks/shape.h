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

/* shape.h */

#ifndef SWF_SHAPE_H_INCLUDED
#define SWF_SHAPE_H_INCLUDED

#include "libswf.h"

#include "blocktypes.h"
#include "block.h"
#include "character.h"
#include "output.h"
#include "fillstyle.h"
#include "linestyle.h"
#include "rect.h"
#include "font.h"

#define SWF_SHAPE_NEWSTYLEFLAG	 (1<<4)
#define SWF_SHAPE_LINESTYLEFLAG	 (1<<3)
#define SWF_SHAPE_FILLSTYLE1FLAG (1<<2)
#define SWF_SHAPE_FILLSTYLE0FLAG (1<<1)
#define SWF_SHAPE_MOVETOFLAG	 (1<<0)

#define NOFILL NULL

struct _stateChangeRecord
{
  int flags;
  int moveToX;
  int moveToY;
  int leftFill;
  int rightFill;
  int line;
  /* newstyle not used.. */
};
typedef struct _stateChangeRecord *StateChangeRecord;

struct _lineToRecord
{
  int dx;
  int dy;
};
typedef struct _lineToRecord *LineToRecord;

struct _curveToRecord
{
  int controlx;
  int controly;
  int anchorx;
  int anchory;
};
typedef struct _curveToRecord *CurveToRecord;

typedef enum
{
  SHAPERECORD_STATECHANGE,
  SHAPERECORD_LINETO,
  SHAPERECORD_CURVETO
} shapeRecordType;

struct _shapeRecord
{
  shapeRecordType type;

  union
  {
    StateChangeRecord stateChange;
    LineToRecord lineTo;
    CurveToRecord curveTo;
  } record;
};
typedef struct _shapeRecord ShapeRecord;

struct _shape
{
  swfCharacter character;
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
  byte isMorph;
  byte isEnded;
};
typedef struct _shape *SWFShape;

#define SHAPE_SIZE sizeof(struct _shape)

/* returns new shape object */
SWFShape newSWFShape();
void destroySWFShape(SWFBlock block);
void SWFShape_addStyleHeader(SWFShape shape);

int SWFShape_getScaledPenX(SWFShape shape);
int SWFShape_getScaledPenY(SWFShape shape);

void SWFShape_moveScaledPenTo(SWFShape shape, int x, int y);
void SWFShape_moveScaledPen(SWFShape shape, int x, int y);

void SWFShape_drawScaledLineTo(SWFShape shape, int x, int y);
void SWFShape_drawScaledLine(SWFShape shape, int dx, int dy);
void SWFShape_drawScaledCurveTo(SWFShape shape, int controlx, int controly,
				int anchorx, int anchory);
void SWFShape_drawScaledCurve(SWFShape shape, int controldx, int controldy,
			      int anchordx, int anchordy);


void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned char c);


/* deprecated: */

#define SWFShape_moveTo SWFShape_movePenTo
#define SWFShape_moveToRelative SWFShape_movePen
#define SWFShape_lineTo SWFShape_drawScaledLineTo
#define SWFShape_lineToRelative SWFShape_drawScaledLine
#define SWFShape_curveTo SWFShape_drawScaledCurveTo
#define SWFShape_curveToRelative SWFShape_drawScaledCurve


void SWFShape_setLineStyle(SWFShape shape, unsigned short width,
			   byte r, byte g, byte b, byte a);

SWFFillStyle SWFShape_addSolidFillStyle(SWFShape shape,
					byte r, byte g, byte b, byte a);
SWFFillStyle SWFShape_addGradientFillStyle(SWFShape shape,
					   SWFGradient gradient, byte flags);
SWFFillStyle SWFShape_addBitmapFillStyle(SWFShape shape,
					 SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill);
void SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill);

void SWFShape_end(SWFShape shape);
void SWFShape_flushStateChange(SWFShape shape);

#endif /* SWF_SHAPE_H_INCLUDED */
