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

#ifndef MING_FILL_H_INCLUDED
#define MING_FILL_H_INCLUDED

#include <stdlib.h>

#include "blocks/swf.h"
#include "position.h"

/* SWFFill adds a position object to manipulate SWFFillStyle's matrix. */

struct _swfFill
{
  SWFFillStyle fillstyle;
  SWFPosition position;
};
typedef struct _swfFill *SWFFill;

#define SWFFILL_SIZE (sizeof(struct _swfFill))

SWFFill newSWFFill(SWFFillStyle fillstyle);
void destroySWFFill(SWFFill fill);

void SWFFill_skewX(SWFFill fill, float x);
void SWFFill_skewXTo(SWFFill fill, float x);
void SWFFill_skewY(SWFFill fill, float y);
void SWFFill_skewYTo(SWFFill fill, float y);
void SWFFill_scaleX(SWFFill fill, float x);
void SWFFill_scaleXTo(SWFFill fill, float x);
void SWFFill_scaleY(SWFFill fill, float y);
void SWFFill_scaleYTo(SWFFill fill, float y);

/* deprecated: */
void SWFFill_scaleXY(SWFFill fill, float x, float y);
void SWFFill_scaleXYTo(SWFFill fill, float x, float y);

void SWFFill_scale(SWFFill fill, float x, float y);
void SWFFill_scaleTo(SWFFill fill, float x, float y);
void SWFFill_rotate(SWFFill fill, float degrees);
void SWFFill_rotateTo(SWFFill fill, float degrees);
void SWFFill_move(SWFFill fill, float x, float y);
void SWFFill_moveTo(SWFFill fill, float x, float y);
void SWFFill_setMatrix(SWFFill fill, float a, float b,
		       float c, float d, float x, float y);

#endif /* MING_FILL_H_INCLUDED */
