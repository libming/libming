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
#include <stdlib.h>

#include "ming.h"
#include "shape_util.h"
#include "blocks/character.h"
#include "blocks/shape.h"
#include "blocks/matrix.h"
#include "movie.h"
#include "fill.h"
#include "libming.h"


float
SWFCharacter_getWidth(SWFCharacter character)
{
	return SWFCharacter_getScaledWidth(character) / Ming_scale;
}


float SWFCharacter_getHeight(SWFCharacter character)
{
	return SWFCharacter_getScaledHeight(character) / Ming_scale;
}


void SWFShape_setLine(SWFShape shape, unsigned short width,
					byte r, byte g, byte b, byte a)
{
	SWFShape_setLineStyle(shape, (unsigned short)(Ming_scale*width), r, g, b, a);
}


SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a)
{
	return newSWFFill(SWFShape_addSolidFillStyle(shape, r, g, b, a));
}


SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient,
				 byte flags)
{
	return newSWFFill(SWFShape_addGradientFillStyle(shape, gradient, flags));
}


SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags)
{
	SWFFill fill =
		newSWFFill(SWFShape_addBitmapFillStyle(shape, bitmap, flags));

	SWFFill_scaleXYTo(fill, Ming_scale, Ming_scale);
	return fill;
}


void SWFShape_setLeftFill(SWFShape shape, SWFFill fill)
{
	SWFShape_setLeftFillStyle(shape, fill==NULL ? NULL : SWFFill_getFillStyle(fill));
}


void SWFShape_setRightFill(SWFShape shape, SWFFill fill)
{
	SWFShape_setRightFillStyle(shape, fill==NULL ? NULL : SWFFill_getFillStyle(fill));
}


void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character)
{
	SWFShape_drawLine(shape, SWFCharacter_getWidth(character), 0);
	SWFShape_drawLine(shape, 0, SWFCharacter_getHeight(character));
	SWFShape_drawLine(shape, -SWFCharacter_getWidth(character), 0);
	SWFShape_drawLine(shape, 0, -SWFCharacter_getHeight(character));
}


void SWFShape_drawCircle(SWFShape shape, float r)
{
	SWFShape_drawArc(shape, r, 0, 360);
}


/* draw an arc of radius r, centered at (x,y), from angle startAngle to angle
	 endAngle (measured in degrees clockwise from due north) */

void SWFShape_drawArc(SWFShape shape, float r, float startAngle, float endAngle)
{
	int i;
	float controlx, controly, anchorx, anchory, x, y;

	/* first determine number of segments, 8 at most */
	int nSegs = (int)(1 + floor(7*(endAngle-startAngle)/360));

	/* subangle is half the angle of each segment */
	float subangle = M_PI*(endAngle-startAngle)/nSegs/360;

	float angle = M_PI*startAngle/180;

	x = (float)rint(r*sin(angle));
	y = (float)-rint(r*cos(angle));

	SWFShape_movePen(shape, x, y);

	for ( i=0; i<nSegs; ++i )
	{
		angle += subangle;
		controlx = (float)(r*sin(angle)/cos(subangle));
		controly = (float)(-r*cos(angle)/cos(subangle));
		angle += subangle;
		anchorx = (float)(r*sin(angle));
		anchory = (float)(-r*cos(angle));

		SWFShape_drawCurve(shape,
					 (float)rint(controlx)-x, (float)rint(controly)-y,
					 (float)rint(anchorx-controlx), (float)rint(anchory-controly));

		x = anchorx;
		y = anchory;
	}
}


/* x,y relative to shape origin */

void SWFShape_movePenTo(SWFShape shape, float x, float y)
{
	SWFShape_moveScaledPenTo(shape, (int)rint(x*Ming_scale),
				 (int)rint(y*Ming_scale));
}


void SWFShape_movePen(SWFShape shape, float dx, float dy)
{
	SWFShape_moveScaledPen(shape, (int)rint(dx*Ming_scale),
			 (int)rint(dy*Ming_scale));
}

float SWFShape_getPenX(SWFShape shape)
{
	return (float)(SWFShape_getScaledPenX(shape)/Ming_scale);
}

float SWFShape_getPenY(SWFShape shape)
{
	return (float)(SWFShape_getScaledPenY(shape)/Ming_scale);
}



void SWFShape_getPen(SWFShape shape, float* penX, float* penY)
{
        *penX = (float)(SWFShape_getScaledPenX(shape)/ Ming_scale);
        *penY = (float)(SWFShape_getScaledPenY(shape)/ Ming_scale);
}


void SWFShape_drawLineTo(SWFShape shape, float x, float y)
{
	SWFShape_drawScaledLineTo(shape, (int)rint(x*Ming_scale),
					(int)rint(y*Ming_scale));
}


void SWFShape_drawLine(SWFShape shape, float dx, float dy)
{
	SWFShape_drawScaledLine(shape, (int)rint(dx*Ming_scale),
				(int)rint(dy*Ming_scale));
}


void SWFShape_drawCurveTo(SWFShape shape, float controlx, float controly,
				float anchorx, float anchory)
{
	SWFShape_drawScaledCurveTo(shape,
					 (int)rint(controlx*Ming_scale),
					 (int)rint(controly*Ming_scale),
					 (int)rint(anchorx*Ming_scale),
					 (int)rint(anchory*Ming_scale));
}


void SWFShape_drawCurve(SWFShape shape,	float controldx, float controldy,
			float anchordx, float anchordy)
{
	SWFShape_drawScaledCurve(shape,
				 (int)rint(controldx*Ming_scale),
				 (int)rint(controldy*Ming_scale),
				 (int)rint(anchordx*Ming_scale),
				 (int)rint(anchordy*Ming_scale));
}


void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned short c)
{
	SWFShape_drawScaledGlyph(shape, font, c, 1024);
}


void SWFShape_drawSizedGlyph(SWFShape shape,
					 SWFFont font, unsigned short c, int size)
{
	if(size == 0)
		SWFShape_drawScaledGlyph(shape, font, c, 1024);
	else
		SWFShape_drawScaledGlyph(shape, font, c, (int)(size*Ming_scale));
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
