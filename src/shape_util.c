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


/* This function returns a characters width.
 * The characters width is return in px. If a character has no bounds
 * -1 is returned.
 */
float SWFCharacter_getWidth(SWFCharacter character)
{
	if(character->bounds == NULL)
		return -1;

	return SWFCharacter_getScaledWidth(character) / Ming_scale;
}

/* This function returns a characters height.
 * The characters height is return in px. If a character has no bounds
 * -1 is returned.
 */
float SWFCharacter_getHeight(SWFCharacter character)
{
	if(character->bounds == NULL)
		return -1;

	return SWFCharacter_getScaledHeight(character) / Ming_scale;
}


/*
 * set line width and line color
 *
 * set line width in px
 * set line color as {r, g, b, a}
 *
 */
void SWFShape_setLine(SWFShape shape, unsigned short width,
					byte r, byte g, byte b, byte a)
{
	SWFShape_setLineStyle_internal(shape, width * Ming_scale, r, g, b, a);
}

/*
 * set Linestyle2 introduce with SWF 8.
 *
 * set line width in pixel
 * set color {r, g, b, a}
 *
 * Linestyle2 extends Linestyle1 with some extra flags:
 *
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
void SWFShape_setLine2(SWFShape shape, unsigned short width, 
                       byte r, byte g, byte b, byte a,
                       int flags, float miterLimit)
{
	SWFShape_setLineStyle2_internal(shape, width * Ming_scale, 
		r, g, b, a, flags, miterLimit);
}

/*
 * set filled Linestyle2 introduce with SWF 8.
 * 
 * set line width in pixel
 *
 * Instead of providing a fill color, a FillStyle can be applied
 * to a line.
 * 
 * Linestyle2 also extends Linestyle1 with some extra flags:
 *
 * Line cap style: select one of the following flags (default is round cap style)
 * SWF_LINESTYLE_CAP_ROUND 
 * SWF_LINESTYLE_CAP_NONE
 * SWF_LINESTYLE_CAP_SQUARE 
 *
 * Line join style: select one of the following flags (default is round join style)
 * SWF_LINESTYLE_JOIN_ROUND
 * SWF_LINESTYLE_JOIN_BEVEL 
 * SWF_LINESTYLE_JOIN_MITER  
 *
 * Scaling flags: disable horizontal / vertical scaling
 * SWF_LINESTYLE_FLAG_NOHSCALE
 * SWF_LINESTYLE_FLAG_NOVSCALE 
 *
 * Enable pixel hinting to correct blurry vertical / horizontal lines
 * -> all anchors will be aligned to full pixels
 * SWF_LINESTYLE_FLAG_HINTING  
 *
 * Disable stroke closure: if no-close flag is set caps will be applied 
 * instead of joins
 * SWF_LINESTYLE_FLAG_NOCLOSE
 *
 * End-cap style: default round
 * SWF_LINESTYLE_FLAG_ENDCAP_ROUND
 * SWF_LINESTYLE_FLAG_ENDCAP_NONE
 * SWF_LINESTYLE_FLAG_ENDCAP_SQUARE
 *
 * If join style is SWF_LINESTYLE_JOIN_MITER a miter limit factor 
 * must be set. Miter max length is then calculated as:
 * max miter len = miter limit * width.
 * If join style is not miter, this value will be ignored.
 */
void SWFShape_setLine2Filled(SWFShape shape, unsigned short width,
                             SWFFillStyle fill,
                             int flags, float miterLimit)
{
	SWFShape_setLineStyle2filled_internal(shape, width * Ming_scale, 
		fill, flags, miterLimit);
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


void SWFShape_drawCircle(SWFShape shape, double r)
{
	SWFShape_drawArc(shape, r, 0, 360);
}


/* draw an arc of radius r, centered at (x,y), from angle startAngle to angle
	 endAngle (measured in degrees clockwise from due north) */

void SWFShape_drawArc(SWFShape shape, double r, double startAngle, double endAngle)
{
	int i;
	double controlx, controly, anchorx, anchory, x, y;

	/* first determine number of segments, 8 at most */
	int nSegs = (int)(1 + floor(7*(endAngle-startAngle)/360));

	/* subangle is half the angle of each segment */
	double subangle = M_PI*(endAngle-startAngle)/nSegs/360;

	double angle = M_PI*startAngle/180;

	x = rint(r*sin(angle));
	y = -rint(r*cos(angle));

	SWFShape_movePen(shape, x, y);

	for ( i=0; i<nSegs; ++i )
	{
		angle += subangle;
		controlx = (r*sin(angle)/cos(subangle));
		controly = (-r*cos(angle)/cos(subangle));
		angle += subangle;
		anchorx = (r*sin(angle));
		anchory = (-r*cos(angle));

		SWFShape_drawCurve(shape, rint(controlx)-x, rint(controly)-y,
		                   rint(anchorx-controlx), rint(anchory-controly));

		x = anchorx;
		y = anchory;
	}
}


/* x,y relative to shape origin */

void SWFShape_movePenTo(SWFShape shape, double x, double y)
{
	SWFShape_moveScaledPenTo(shape, (int)rint(x*Ming_scale),
				 (int)rint(y*Ming_scale));
}


void SWFShape_movePen(SWFShape shape, double dx, double dy)
{
	SWFShape_moveScaledPen(shape, (int)rint(dx*Ming_scale),
			 (int)rint(dy*Ming_scale));
}

double SWFShape_getPenX(SWFShape shape)
{
	return SWFShape_getScaledPenX(shape)/Ming_scale;
}

double SWFShape_getPenY(SWFShape shape)
{
	return SWFShape_getScaledPenY(shape)/Ming_scale;
}



void SWFShape_getPen(SWFShape shape, double* penX, double* penY)
{
        *penX = SWFShape_getScaledPenX(shape)/ Ming_scale;
        *penY = SWFShape_getScaledPenY(shape)/ Ming_scale;
}


void SWFShape_drawLineTo(SWFShape shape, double x, double y)
{
	SWFShape_drawScaledLineTo(shape, (int)rint(x*Ming_scale),
					(int)rint(y*Ming_scale));
}


void SWFShape_drawLine(SWFShape shape, double dx, double dy)
{
	SWFShape_drawScaledLine(shape, (int)rint(dx*Ming_scale),
				(int)rint(dy*Ming_scale));
}


void SWFShape_drawCurveTo(SWFShape shape, double controlx, double controly,
				double anchorx, double anchory)
{
	SWFShape_drawScaledCurveTo(shape,
					 (int)rint(controlx*Ming_scale),
					 (int)rint(controly*Ming_scale),
					 (int)rint(anchorx*Ming_scale),
					 (int)rint(anchory*Ming_scale));
}


void SWFShape_drawCurve(SWFShape shape,	double controldx, double controldy,
			double anchordx, double anchordy)
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
