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

typedef struct SWFShape_s *SWFShape;

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


/* returns new shape object */
SWFShape
newSWFShape();

/* returns a shape containing the bitmap in a filled rect */
SWFShape
newSWFShapeFromBitmap(SWFBitmap bitmap);

void
destroySWFShape(SWFBlock block);

void
SWFShape_addStyleHeader(SWFShape shape);

int
SWFShape_getScaledPenX(SWFShape shape);

int
SWFShape_getScaledPenY(SWFShape shape);

void
SWFShape_getScaledPenXY(SWFShape shape, int* outX, int* outY);

void
SWFShape_moveScaledPenTo(SWFShape shape, int x, int y);

void
SWFShape_moveScaledPen(SWFShape shape, int x, int y);

void
SWFShape_drawScaledLineTo(SWFShape shape, int x, int y);

void
SWFShape_drawScaledLine(SWFShape shape, int dx, int dy);

void
SWFShape_drawScaledCurveTo(SWFShape shape,
			   int controlx, int controly,
			   int anchorx, int anchory);

void
SWFShape_drawScaledCurve(SWFShape shape,
			 int controldx, int controldy,
			 int anchordx, int anchordy);

void
SWFShape_drawScaledGlyph(SWFShape shape,
			 SWFFont font, unsigned short c, int size);

void
SWFShape_setLineStyle(SWFShape shape, unsigned short width,
		      byte r, byte g, byte b, byte a);

SWFFillStyle
SWFShape_addSolidFillStyle(SWFShape shape, byte r, byte g, byte b, byte a);

SWFFillStyle
SWFShape_addGradientFillStyle(SWFShape shape, SWFGradient gradient, byte flags);

SWFFillStyle
SWFShape_addBitmapFillStyle(SWFShape shape, SWFBitmap bitmap, byte flags);

void
SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill);

void
SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill);

void
SWFShape_end(SWFShape shape);

void
SWFShape_flushStateChange(SWFShape shape);

SWFOutput
SWFShape_getOutput(SWFShape shape);

void
SWFShape_getFills(SWFShape shape, SWFFillStyle** outFills, int* outNFills);

void
SWFShape_getLines(SWFShape shape, SWFLineStyle** outLines, int* outNLines);

void
SWFShape_setMorphFlag(SWFShape shape);

#endif /* SWF_SHAPE_H_INCLUDED */
