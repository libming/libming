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

/* position.h - wrap matrices in something more usable */

#ifndef MING_POSITION_H_INCLUDED
#define MING_POSITION_H_INCLUDED

#include "blocks/swf.h"

struct _swfPosition
{
  float x;
  float y;
  float xScale;
  float yScale;
  float xSkew;
  float ySkew;
  float rot;
  SWFMatrix matrix;
};

typedef struct _swfPosition *SWFPosition;

#define SWFPOSITION_SIZE sizeof(struct _swfPosition)

void destroySWFPosition(SWFPosition position);
SWFPosition newSWFPosition(SWFMatrix matrix);

void SWFPosition_skewX(SWFPosition position, float x);
void SWFPosition_skewXTo(SWFPosition position, float x);
void SWFPosition_skewY(SWFPosition position, float y);
void SWFPosition_skewYTo(SWFPosition position, float y);
void SWFPosition_scaleX(SWFPosition position, float x);
void SWFPosition_scaleXTo(SWFPosition position, float x);
void SWFPosition_scaleY(SWFPosition position, float y);
void SWFPosition_scaleYTo(SWFPosition position, float y);
void SWFPosition_scaleXY(SWFPosition position, float x, float y);
void SWFPosition_scaleXYTo(SWFPosition position, float x, float y);
void SWFPosition_setMatrix(SWFPosition p, float a, float b, float c, float d,
			   float x, float y);

void SWFPosition_rotate(SWFPosition position, float degrees);
void SWFPosition_rotateTo(SWFPosition position, float degrees);
void SWFPosition_move(SWFPosition position, float x, float y);
void SWFPosition_moveTo(SWFPosition position, float x, float y);

#endif /* MING_POSITION_H_INCLUDED */
