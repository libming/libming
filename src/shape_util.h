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

#ifndef MING_SHAPE_H_INCLUDED
#define MING_SHAPE_H_INCLUDED

#include "blocks/swf.h"
#include "fill.h"

float SWFCharacter_getWidth(SWFCharacter character);
float SWFCharacter_getHeight(SWFCharacter character);


void SWFShape_setLine(SWFShape shape, unsigned short width,
		      byte r, byte g, byte b, byte a);

SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a);

SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient,
				 byte flags);

SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFill(SWFShape shape, SWFFill fill);
void SWFShape_setRightFill(SWFShape shape, SWFFill fill);

void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character);

void SWFShape_setLine(SWFShape shape,
		      unsigned short width, byte r, byte g, byte b, byte a);

void SWFShape_drawArc(SWFShape shape, int r, float startAngle, float endAngle);

void SWFShape_drawCircle(SWFShape shape, int r);


/* approximate a cubic bezier with quadratic segments,
   returns the number of segments used */

int SWFShape_drawCubic(SWFShape shape, float bx, float by, float cx, float cy, float dx, float dy);
int SWFShape_drawCubicTo(SWFShape shape, float bx, float by, float cx, float cy, float dx, float dy);

/* x,y relative to shape origin */
void SWFShape_drawLineTo(SWFShape shape, float x, float y);
void SWFShape_drawLine(SWFShape shape, float dx, float dy);

/*  */
void SWFShape_drawCurveTo(SWFShape shape, float controlx, float controly,
			  float anchorx, float anchory);

void SWFShape_drawCurve(SWFShape shape,	float controldx, float controldy,
			float anchordx, float anchordy);

void SWFShape_movePenTo(SWFShape shape, float x, float y);
void SWFShape_movePen(SWFShape shape, float dx, float dy);


/* deprecated: */

#define SWFShape_drawFontGlyph SWFShape_drawGlyph


#endif /* MING_SHAPE_H_INCLUDED */
