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

/* shape_util.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_SHAPE_UTIL_H_INCLUDED
#define SWF_SHAPE_UTIL_H_INCLUDED

#include "ming.h"

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

void SWFShape_drawArc(SWFShape shape, float r, float startAngle, float endAngle);

void SWFShape_drawCircle(SWFShape shape, float r);


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

float SWFShape_getPenX(SWFShape shape);
float SWFShape_getPenY(SWFShape shape);

void SWFShape_getPen(SWFShape shape, float* penX, float* penY);

#endif /* SWF_SHAPE_UTIL_H_INCLUDED */
