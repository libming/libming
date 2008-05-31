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


#include <math.h>
#include <string.h>

#include "ming.h"
#include "libming.h"
#include "shape_util.h"
#include "blocks/error.h"
#include "blocks/shape.h"


typedef struct
{
	double x;
	double y;
} point;

typedef struct
{
	point a;
	point b;
	point c;
	point d;
} cubic;

typedef struct
{
	point a;
	point b;
	point c;
} quadratic;

static void halfpointCubic(cubic *c, double *x, double *y)
{
	*x = (c->a.x + 3*c->b.x + 3*c->c.x + c->d.x) / 8;
	*y = (c->a.y + 3*c->b.y + 3*c->c.y + c->d.y) / 8;
}

static void halfpointQuadratic(quadratic *q, double *x, double *y)
{
	*x = (q->a.x + 2*q->b.x + q->c.x) / 4;
	*y = (q->a.y + 2*q->b.y + q->c.y) / 4;
}

#define abs(f) ((f)>0?(f):-(f))

static double errorPoints(double ax, double ay, double bx, double by)
{
	return abs(ax-bx) + abs(ay-by);
}

static void subdivideCubicLeft(cubic *New, cubic *old, double t)
{
	SWF_assert(t>0.0 && t<1.0);

	if(New != old)
		memcpy(New, old, sizeof(cubic));

	New->d.x = t*New->c.x + (1-t)*New->d.x;
	New->d.y = t*New->c.y + (1-t)*New->d.y;
	New->c.x = t*New->b.x + (1-t)*New->c.x;
	New->c.y = t*New->b.y + (1-t)*New->c.y;
	New->b.x = t*New->a.x + (1-t)*New->b.x;
	New->b.y = t*New->a.y + (1-t)*New->b.y;

	New->d.x = t*New->c.x + (1-t)*New->d.x;
	New->d.y = t*New->c.y + (1-t)*New->d.y;
	New->c.x = t*New->b.x + (1-t)*New->c.x;
	New->c.y = t*New->b.y + (1-t)*New->c.y;

	New->d.x = t*New->c.x + (1-t)*New->d.x;
	New->d.y = t*New->c.y + (1-t)*New->d.y;
}

static void subdivideCubicRight(cubic *New, cubic *old, double t)
{
	SWF_assert(t>0.0 && t<1.0);

	if(New != old)
		memcpy(New, old, sizeof(cubic));

	New->a.x = t*New->a.x + (1-t)*New->b.x;
	New->a.y = t*New->a.y + (1-t)*New->b.y;
	New->b.x = t*New->b.x + (1-t)*New->c.x;
	New->b.y = t*New->b.y + (1-t)*New->c.y;
	New->c.x = t*New->c.x + (1-t)*New->d.x;
	New->c.y = t*New->c.y + (1-t)*New->d.y;

	New->a.x = t*New->a.x + (1-t)*New->b.x;
	New->a.y = t*New->a.y + (1-t)*New->b.y;
	New->b.x = t*New->b.x + (1-t)*New->c.x;
	New->b.y = t*New->b.y + (1-t)*New->c.y;

	New->a.x = t*New->a.x + (1-t)*New->b.x;
	New->a.y = t*New->a.y + (1-t)*New->b.y;
}

static int SWFShape_approxCubic(SWFShape shape, cubic *c);

static int subdivideCubic(SWFShape shape, cubic *c)
{
	cubic New;
	int nCurves;

	subdivideCubicLeft(&New, c, 0.5);
	nCurves = SWFShape_approxCubic(shape, &New);

	subdivideCubicRight(&New, c, 0.5);
	nCurves += SWFShape_approxCubic(shape, &New);

	return nCurves;
}

static int SWFShape_approxCubic(SWFShape shape, cubic *c)
{
	quadratic q;

	double cx, cy, qx, qy;

	if(c->b.x == c->a.x && c->b.y == c->a.y)
	{
		q.a = c->a;
		q.b = c->c;
		q.c = c->d;
	}
	else if(c->d.x == c->c.x && c->d.y == c->c.y)
	{
		q.a = c->a;
		q.b = c->b;
		q.c = c->d;
	}
	else
		if((c->a.x-c->b.x)*(c->c.x-c->b.x)+(c->a.y-c->b.y)*(c->c.y-c->b.y) >= 0 ||
			 (c->b.x-c->c.x)*(c->d.x-c->c.x)+(c->b.y-c->c.y)*(c->d.y-c->c.y) >= 0)
	{
		/* make sure that angles B and C are obtuse, so that outside
			 edges meet on the right side */

			return subdivideCubic(shape, c);
	}
	else
	{
		double bcrossa = c->b.x*c->a.y - c->a.x*c->b.y;
		double ccrossd = c->c.x*c->d.y - c->d.x*c->c.y;

		double denom = (c->a.y-c->b.y)*(c->d.x-c->c.x) -
			(c->a.x-c->b.x)*(c->d.y-c->c.y);

		if(denom == 0)
		{
			/* XXX - they're collinear?? */
			SWFShape_drawScaledLineTo(shape, (int)rint(c->d.x), (int)rint(c->d.y));
			return 1;
		}

		q.a = c->a;
		q.b.x = ((c->d.x-c->c.x)*bcrossa + (c->b.x-c->a.x)*ccrossd) / denom;
		q.b.y = ((c->d.y-c->c.y)*bcrossa + (c->b.y-c->a.y)*ccrossd) / denom;
		q.c = c->d;
	}

	halfpointCubic(c, &cx, &cy);
	halfpointQuadratic(&q, &qx, &qy);

	if(errorPoints(cx, cy, qx, qy) > Ming_cubicThreshold)
	{
		return subdivideCubic(shape, c);
	}
	else
	{
		/* draw quadratic w/ control point at intersection of outside edges */

		SWFShape_drawScaledCurveTo(shape, (int)rint(q.b.x), (int)rint(q.b.y),
						 (int)rint(q.c.x), (int)rint(q.c.y));
		return 1;
	}
}


int SWFShape_drawScaledCubicTo(SWFShape shape, int bx, int by,
						 int cx, int cy, int dx, int dy)
{
	int ax = SWFShape_getScaledPenX(shape);
	int ay = SWFShape_getScaledPenY(shape);

	/* compute coefficients */
	int a1x = -ax + 3*bx - 3*cx + dx;
	int a1y = -ay + 3*by - 3*cy + dy;
	int a2x =	 ax - 2*bx + cx;
	int a2y =	 ay - 2*by + cy;
	int a3x = -ax +		bx;
	int a3y = -ay +		by;

	double a = 6*(a2x*a1y-a2y*a1x);
	double b = 6*(a3x*a1y-a3y*a1x);
	double c = 2*(a3x*a2y-a3y*a2x);

	/* First, chop at inflection points, where a*t^2 + b*t + c = 0 */

	double d = b*b - 4*a*c;

	double t1 = 0.0, t2 = 1.0;
	int nCurves = 0;

	cubic pts = { { ax, ay }, { bx, by }, {cx, cy }, { dx, dy } };
	cubic New;

	if ( d > 0 )
	{
		/* two roots */

		t1 = (-b-sqrt(d))/(2*a);
		t2 = (-b+sqrt(d))/(2*a);

		if ( a < 0 )
		{
			double tmp = t2;
			t2 = t1;
			t1 = tmp;
		}
	}
	else if ( d == 0 )
	{
		/* singular root */
		t1 = -b/(2*a);
	}

	/* use subdivision method to build t=0..t1, t=t1..t2, t=t2..1 curves */

	if ( t1 > 0.0 && t1 < 1.0 )
	{
		subdivideCubicLeft(&New, &pts, t1);

		nCurves += SWFShape_approxCubic(shape, &New);

		/*
		nCurves += SWFShape_drawCubicTo(shape,
						new.b.x, new.b.y,
						new.c.x, new.c.y,
						new.d.x, new.d.y);
		*/

		subdivideCubicRight(&pts, &pts, t1);
		t2 = (t2-t1)/(1-t1);
	}

	if ( t2 > 0.0 && t2 < 1.0 )
	{
		subdivideCubicLeft(&New, &pts, t2);

		nCurves += SWFShape_approxCubic(shape, &New);

		/*
		nCurves += SWFShape_drawCubicTo(shape,
						new.b.x, new.b.y,
						new.c.x, new.c.y,
						new.d.x, new.d.y);
		*/

		subdivideCubicRight(&pts, &pts, t2);
	}

	nCurves += SWFShape_approxCubic(shape, &pts);

	return nCurves;
}


/* returns number of splines used */

int
SWFShape_drawCubic(SWFShape shape, double bx, double by,
			 double cx, double cy, double dx, double dy)
{
	int sax = SWFShape_getScaledPenX(shape);
	int say = SWFShape_getScaledPenY(shape);
	int sbx = (int)rint(bx*Ming_scale) + sax;
	int sby = (int)rint(by*Ming_scale) + say;
	int scx = (int)rint(cx*Ming_scale) + sbx;
	int scy = (int)rint(cy*Ming_scale) + sby;
	int sdx = (int)rint(dx*Ming_scale) + scx;
	int sdy = (int)rint(dy*Ming_scale) + scy;

	return SWFShape_drawScaledCubicTo(shape, sbx, sby, scx, scy, sdx, sdy);
}

int SWFShape_drawCubicTo(SWFShape shape, double bx, double by,
			 double cx, double cy, double dx, double dy)
{
	return SWFShape_drawScaledCubicTo(shape,
						(int)rint(bx*Ming_scale),
						(int)rint(by*Ming_scale),
						(int)rint(cx*Ming_scale),
						(int)rint(cy*Ming_scale),
						(int)rint(dx*Ming_scale),
						(int)rint(dy*Ming_scale));
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
