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

#include <math.h>

#include "libming.h"
#include "shape_util.h"

extern float Ming_scale;
extern int Ming_cubicThreshold;

float SWFCharacter_getWidth(SWFCharacter character)
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
  SWFShape_setLineStyle(shape, width, r, g, b, a);
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
  SWFFill fill = newSWFFill(SWFShape_addBitmapFillStyle(shape, bitmap, flags));
  SWFFill_scaleTo(fill, Ming_scale, Ming_scale);
  return fill;
}

void SWFShape_setLeftFill(SWFShape shape, SWFFill fill)
{
  SWFShape_setLeftFillStyle(shape, fill==NULL ? NULL : fill->fillstyle);
}

void SWFShape_setRightFill(SWFShape shape, SWFFill fill)
{
  SWFShape_setRightFillStyle(shape, fill==NULL ? NULL : fill->fillstyle);
}

void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character)
{
  SWFShape_drawLine(shape, SWFCharacter_getWidth(character), 0);
  SWFShape_drawLine(shape, 0, SWFCharacter_getHeight(character));
  SWFShape_drawLine(shape, -SWFCharacter_getWidth(character), 0);
  SWFShape_drawLine(shape, 0, -SWFCharacter_getHeight(character));
}

void SWFShape_drawCircle(SWFShape shape, int r)
{
  SWFShape_drawArc(shape, r, 0, 360);
}


/* draw an arc of radius r, centered at (x,y), from angle startAngle to angle
   endAngle (measured in degrees clockwise from due north) */

void SWFShape_drawArc(SWFShape shape, int r, float startAngle, float endAngle)
{
  int i;
  float controlx, controly, anchorx, anchory, x, y;

  /* first determine number of segments, 8 at most */
  int nSegs = 1 + floor(7*(endAngle-startAngle)/360);

  /* subangle is half the angle of each segment */
  float subangle = M_PI*(endAngle-startAngle)/nSegs/360;

  float angle = M_PI*startAngle/180;

  x = rint(r*sin(angle));
  y = -rint(r*cos(angle));

  SWFShape_movePen(shape, x, y);

  for(i=0; i<nSegs; ++i)
  {
    angle += subangle;
    controlx = r*sin(angle)/cos(subangle);
    controly = -r*cos(angle)/cos(subangle);
    angle += subangle;
    anchorx = r*sin(angle);
    anchory = -r*cos(angle);

    SWFShape_drawCurve(shape,
		       rint(controlx)-x, rint(controly)-y,
		       rint(anchorx-controlx), rint(anchory-controly));
    x = anchorx;
    y = anchory;
  }
}


struct control
{
  float ax;
  float ay;
  float bx;
  float by;
  float cx;
  float cy;
  float dx;
  float dy;
};

typedef struct control *Control;

static void subdivideLeft(Control new, Control old, float t)
{
  SWF_assert(t>0.0 && t<1.0);

  if(new != old)
    memcpy(new, old, sizeof(struct control));

  new->dx = t*new->cx + (1-t)*new->dx;
  new->dy = t*new->cy + (1-t)*new->dy;
  new->cx = t*new->bx + (1-t)*new->cx;
  new->cy = t*new->by + (1-t)*new->cy;
  new->bx = t*new->ax + (1-t)*new->bx;
  new->by = t*new->ay + (1-t)*new->by;

  new->dx = t*new->cx + (1-t)*new->dx;
  new->dy = t*new->cy + (1-t)*new->dy;
  new->cx = t*new->bx + (1-t)*new->cx;
  new->cy = t*new->by + (1-t)*new->cy;

  new->dx = t*new->cx + (1-t)*new->dx;
  new->dy = t*new->cy + (1-t)*new->dy;
}

static void subdivideRight(Control new, Control old, float t)
{
  SWF_assert(t>0.0 && t<1.0);

  if(new != old)
    memcpy(new, old, sizeof(struct control));

  new->ax = t*new->ax + (1-t)*new->bx;
  new->ay = t*new->ay + (1-t)*new->by;
  new->bx = t*new->bx + (1-t)*new->cx;
  new->by = t*new->by + (1-t)*new->cy;
  new->cx = t*new->cx + (1-t)*new->dx;
  new->cy = t*new->cy + (1-t)*new->dy;

  new->ax = t*new->ax + (1-t)*new->bx;
  new->ay = t*new->ay + (1-t)*new->by;
  new->bx = t*new->bx + (1-t)*new->cx;
  new->by = t*new->by + (1-t)*new->cy;

  new->ax = t*new->ax + (1-t)*new->bx;
  new->ay = t*new->ay + (1-t)*new->by;
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


static int SWFShape_approxCubic(SWFShape shape, Control pts)
{
  int nCurves = 0;

  float bcrossa = pts->bx*pts->ay - pts->ax*pts->by;
  float ccrossd = pts->cx*pts->dy - pts->dx*pts->cy;

  float denom = (pts->ay-pts->by)*(pts->dx-pts->cx) -
                (pts->ax-pts->bx)*(pts->dy-pts->cy);

  float ex, ey;

  /* calc. diff area */
  float area = 0;

  /* XXX - not a great idea, this can happen if the outer control edges are
     parallel: */

  if(denom == 0)
    return 0;

  ex = ((pts->dx-pts->cx)*bcrossa + (pts->bx-pts->ax)*ccrossd) / denom;
  ey = ((pts->dy-pts->cy)*bcrossa + (pts->by-pts->ay)*ccrossd) / denom;

  area = (pts->bx-ex)*(pts->cy-ey) - (pts->cx-ex)*(pts->by-ey);

  if(area > Ming_cubicThreshold || area < -Ming_cubicThreshold)
  {
    struct control new;

    subdivideLeft(&new, pts, 0.5);
    nCurves = SWFShape_approxCubic(shape, &new);

    subdivideRight(&new, pts, 0.5);
    nCurves += SWFShape_approxCubic(shape, &new);

    return nCurves;
  }
  else
  {
    /* draw quadratic w/ control point at intersection of outside edges */

    SWFShape_drawCurveTo(shape, (int)rint(ex), (int)rint(ey),
			 (int)rint(pts->dx), (int)rint(pts->dy));
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
  int a2x =  ax - 2*bx + cx;
  int a2y =  ay - 2*by + cy;
  int a3x = -ax +   bx;
  int a3y = -ay +   by;

  double a = a1x*a2y-a2x*a1y;
  double b = a1x*a3y-a3x*a1y;
  double c = a2x*a3y-a3x*a2y;

  /* First, chop at inflection points, where a*t^2 + b*t + c = 0 */

  double d = b*b-4*a*c;

  float t1 = 0.0, t2 = 1.0;
  int nCurves = 0;

  struct control pts = { (float)ax, (float)ay, (float)bx, (float)by,
			 (float)cx, (float)cy, (float)dx, (float)dy };
  struct control new;

  if(d>0)
  {
    /* two roots */

    t1 = (-b-sqrt(d))/(2*a);
    t2 = (-b+sqrt(d))/(2*a);

    if(a<0)
    {
      float tmp = t2;
      t2 = t1;
      t1 = tmp;
    }
  }
  else if(d==0)
  {
    /* singular root */
    t1 = -b/(2*a);
  }

  /* use subdivision method to build t=0..t1, t=t1..t2, t=t2..1 curves */

  if(t1 > 0.0 && t1 < 1.0)
  {
    subdivideLeft(&new, &pts, t1);
    nCurves += SWFShape_approxCubic(shape, &new);

    subdivideRight(&pts, &pts, t1);
    t2 = (t2-t1)/(1-t1);
  }

  if(t2 > 0.0 && t2 < 1.0)
  {
    subdivideLeft(&new, &pts, t2);
    nCurves += SWFShape_approxCubic(shape, &new);
    subdivideRight(&pts, &pts, t2);
  }

  nCurves += SWFShape_approxCubic(shape, &pts);

  return nCurves;
}


/* returns number of splines used */

int SWFShape_drawCubic(SWFShape shape, float bx, float by,
		       float cx, float cy, float dx, float dy)
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

int SWFShape_drawCubicTo(SWFShape shape, float bx, float by,
			 float cx, float cy, float dx, float dy)
{
  return SWFShape_drawScaledCubicTo(shape,
				    (int)rint(bx*Ming_scale),
				    (int)rint(by*Ming_scale),
				    (int)rint(cx*Ming_scale),
				    (int)rint(cy*Ming_scale),
				    (int)rint(dx*Ming_scale),
				    (int)rint(dy*Ming_scale));
}
