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

#include <stdlib.h>
#include <math.h>

#include "ming.h"
#include "libming.h"
#include "position.h"
#include "blocks/matrix.h"
#include "movie.h"
#include "blocks/error.h"


void destroySWFPosition(SWFPosition position)
{
	/* something else owns the matrix, so don't free it. */
	free(position);
}


/* position wraps a matrix */
SWFPosition
newSWFPosition(SWFMatrix matrix)
{
	SWFPosition p = (SWFPosition)malloc(sizeof(struct SWFPosition_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == p)
		return NULL;

	p->x = 0;
	p->y = 0;
	p->xScale = 1.0;
	p->yScale = 1.0;
	p->xSkew = 0;
	p->ySkew = 0;
	p->rot = 0;
	p->matrix = matrix;

	return p;
}


/* x-skew, then y-skew, then rot and scale, then xlate */
static void
updateMatrix(SWFPosition p)
{
	double cRot = cos(p->rot*M_PI/180);
	double sRot = sin(p->rot*M_PI/180);
	double xS = p->xSkew, yS = p->ySkew;
	SWFMatrix m = p->matrix;
	if ( ! m )
	{
		SWF_warn("Warning! tried to change the matrix of an object with no matrix");
		return;
	}

	SWFMatrix_set(m, p->xScale * (cRot - xS*sRot),
		p->xScale * (yS*cRot - (xS*yS+1)*sRot),
		p->yScale * (sRot + xS*cRot),
		p->yScale * (yS*sRot + (xS*yS+1)*cRot),
		(int)rint(Ming_scale*p->x),
		(int)rint(Ming_scale*p->y));
}


void
SWFPosition_skewX(SWFPosition p, double x)
{
	p->xSkew += x;
	updateMatrix(p);
}


void
SWFPosition_skewXTo(SWFPosition p, double x)
{
	p->xSkew = x;
	updateMatrix(p);
}


void
SWFPosition_skewY(SWFPosition p, double y)
{
	p->ySkew += y;
	updateMatrix(p);
}


void
SWFPosition_skewYTo(SWFPosition p, double y)
{
	p->ySkew = y;
	updateMatrix(p);
}


void
SWFPosition_scaleX(SWFPosition p, double x)
{
	p->xScale *= x;
	updateMatrix(p);
}


void
SWFPosition_scaleXTo(SWFPosition p, double x)
{
	p->xScale = x;
	updateMatrix(p);
}


void
SWFPosition_scaleY(SWFPosition p, double y)
{
	p->yScale *= y;
	updateMatrix(p);
}


void
SWFPosition_scaleYTo(SWFPosition p, double y)
{
	p->yScale = y;
	updateMatrix(p);
}


void
SWFPosition_rotate(SWFPosition p, double degrees)
{
	p->rot += degrees;
	updateMatrix(p);
}


void
SWFPosition_rotateTo(SWFPosition p, double degrees)
{
	p->rot = degrees;
	updateMatrix(p);
}


void
SWFPosition_move(SWFPosition p, double x, double y)
{
	p->x += x;
	p->y += y;
	updateMatrix(p);
}


void
SWFPosition_moveTo(SWFPosition p, double x, double y)
{
	p->x = x;
	p->y = y;
	updateMatrix(p);
}


void
SWFPosition_scaleXY(SWFPosition p, double x, double y)
{
	p->xScale *= x;
	p->yScale *= y;
	updateMatrix(p);
}


void
SWFPosition_scaleXYTo(SWFPosition p, double x, double y)
{
	p->xScale = x;
	p->yScale = y;
	updateMatrix(p);
}


void
SWFPosition_setMatrix(SWFPosition p, double a, double b, double c, double d, double x, double y)
{
	/* set the transform matrix, but don't touch the position properties-
		 you won't want to use the two in conjunction, anyway */

	SWFMatrix_set(p->matrix, a, b, c, d,
		(int)rint(Ming_scale*x), (int)rint(Ming_scale*y));
}


SWFMatrix
SWFPosition_getMatrix(SWFPosition p)
{
	return p->matrix;
}


double
SWFPosition_getRotation(SWFPosition position)
{
	return position->rot;
}


double
SWFPosition_getX(SWFPosition position)
{
	return position->x;
}


double
SWFPosition_getY(SWFPosition position)
{
	return position->y;
}


void
SWFPosition_getXY(SWFPosition position, double* outX, double* outY)
{
	if ( outX != NULL )
		*outX = position->x;

	if ( outY != NULL )
		*outY = position->y;
}


double
SWFPosition_getXScale(SWFPosition position)
{
	return position->xScale;
}


double
SWFPosition_getYScale(SWFPosition position)
{
	return position->yScale;
}


void
SWFPosition_getXYScale(SWFPosition position, double* outXScale, double* outYScale)
{
	if ( outXScale != NULL )
		*outXScale = position->xScale;

	if ( outYScale != NULL )
		*outYScale = position->yScale;
}


double
SWFPosition_getXSkew(SWFPosition position)
{
	return position->xSkew;
}


double
SWFPosition_getYSkew(SWFPosition position)
{
	return position->ySkew;
}


void
SWFPosition_getXYSkew(SWFPosition position, double* outXSkew, double* outYSkew)
{
	if ( outXSkew )
		*outXSkew = position->xSkew;

	if ( outYSkew )
		*outYSkew = position->ySkew;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
