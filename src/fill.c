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

#include "fill.h"
#include "position.h"
#include "libming.h"

struct SWFFill_s
{
	SWFFillStyle fillstyle;
	SWFPosition position;
#if TRACK_ALLOCS
	/* memory node for garbage collection */
	mem_node *gcnode;
#endif
};


SWFFill
newSWFFill(SWFFillStyle fillstyle)
{
	SWFFill fill;
	if(fillstyle == NULL)
		return NULL;

	fill = (SWFFill)malloc(sizeof(struct SWFFill_s));

	fill->fillstyle = fillstyle;
	fill->position = newSWFPosition(SWFFillStyle_getMatrix(fill->fillstyle));

#if TRACK_ALLOCS
	fill->gcnode = ming_gc_add_node(fill, (dtorfunctype) destroySWFFill);
#endif

	return fill;
}


SWFFillStyle
SWFFill_getFillStyle(SWFFill fill)
{
	return fill->fillstyle;
}


void
destroySWFFill(SWFFill fill)
{
	destroySWFPosition(fill->position);
#if TRACK_ALLOCS
	ming_gc_remove_node(fill->gcnode);
#endif
	free(fill);
}


void
SWFFill_skewX(SWFFill fill, float x)
{
	SWFPosition_skewX(fill->position, x);
}


void
SWFFill_skewXTo(SWFFill fill, float x)
{
	SWFPosition_skewXTo(fill->position, x);
}


void
SWFFill_skewY(SWFFill fill, float y)
{
	SWFPosition_skewY(fill->position, y);
}


void
SWFFill_skewYTo(SWFFill fill, float y)
{
	SWFPosition_skewYTo(fill->position, y);
}


void
SWFFill_scaleX(SWFFill fill, float x)
{
	SWFPosition_scaleX(fill->position, x);
}


void
SWFFill_scaleXTo(SWFFill fill, float x)
{
	SWFPosition_scaleXTo(fill->position, x);
}


void
SWFFill_scaleY(SWFFill fill, float y)
{
	SWFPosition_scaleY(fill->position, y);
}


void
SWFFill_scaleYTo(SWFFill fill, float y)
{
	SWFPosition_scaleYTo(fill->position, y);
}


void
SWFFill_scaleXY(SWFFill fill, float x, float y)
{
	SWFPosition_scaleXY(fill->position, x, y);
}


void
SWFFill_scaleXYTo(SWFFill fill, float x, float y)
{
	SWFPosition_scaleXYTo(fill->position, x, y);
}


void
SWFFill_rotate(SWFFill fill, float degrees)
{
	SWFPosition_rotate(fill->position, degrees);
}


void
SWFFill_rotateTo(SWFFill fill, float degrees)
{
	SWFPosition_rotateTo(fill->position, degrees);
}


void
SWFFill_move(SWFFill fill, float x, float y)
{
	SWFPosition_move(fill->position, x, y);
}


void
SWFFill_moveTo(SWFFill fill, float x, float y)
{
	SWFPosition_moveTo(fill->position, x, y);
}


void
SWFFill_setMatrix(SWFFill fill, float a, float b,
									float c, float d, float x, float y)
{
	SWFPosition_setMatrix(fill->position, a, b, c, d, x, y);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
