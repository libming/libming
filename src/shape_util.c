/*
    Ming, an SWF output library
    Copyright (C) 2000  Opaque Industries - http://www.opaque.net/

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

  x = floor(r*sin(angle)+0.5);
  y = -floor(r*cos(angle)+0.5);

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
		       floor(controlx+0.5)-x, floor(controly+0.5)-y,
		       floor(anchorx-controlx+0.5), floor(anchory-controly+0.5));
    x = anchorx;
    y = anchory;
  }
}

#include "read.c"
#include <assert.h>
#define error(x) { printf("ERROR: %s\n",(x)); assert(0); }

/* yes, this is a total hack. */

void SWFShape_drawGlyph(SWFShape shape, SWFFont font, int c)
{
  byte *p = SWFFont_findCharacterGlyph(font, c);
  byte **f = &p;

  int moveBits, x, y;
  int straight, numBits;

  /* moveTos in the record are absolute, but we want to draw from the current
     location. grr. */

  int startX = SWFShape_getPenX(shape);
  int startY = SWFShape_getPenY(shape);

  byteAlign();

  if(readBits(f, 4) != 1) /* fill bits */
    error("SWFShape_drawCharacter: was expecting fill bits = 1");

  if(readBits(f, 4) != 0) /* line bits */
    error("SWFShape_drawCharacter: was expecting line bits = 0");

  /* now we get to parse the shape commands.  Oh boy. */

  /* the first one will be a non-edge block- grab the moveto loc */

  readBits(f, 6); /* type 0, etc. */

  moveBits = readBits(f, 5);
  x = readSBits(f, moveBits);
  y = readSBits(f, moveBits);

  SWFShape_movePenTo(shape, x+startX, y+startY);

  if(readBits(f, 1) != 1) /* fill1 = 1 */
    error("SWFShape_drawCharacter says: oops.  Was expecting fill1 = 1.");

  /* we could just dump the rest if we had access to the shape's output buffer,
     but this isn't that hard.. */

  /* oh, and we aren't storing compiled shape bytes any more, anyway.. */

  for(;;)
  {
    if(readBits(f, 1) == 0)
    {
      /* it's a moveTo or a shape end */

      if(readBits(f, 5) == 0)
	break;

      moveBits = readBits(f, 5);
      x = readSBits(f, moveBits);
      y = readSBits(f, moveBits);

      SWFShape_movePenTo(shape, x+startX, y+startY);
      continue;
    }

    straight = readBits(f, 1);
    numBits = readBits(f, 4)+2;

    if(straight==1)
    {
      if(readBits(f, 1)) /* general line */
      {
	x = readSBits(f, numBits);
	y = readSBits(f, numBits);

	SWFShape_drawLine(shape, x, y);
      }
      else
	if(readBits(f, 1)) /* vert = 1 */
	  SWFShape_drawLine(shape, 0, readSBits(f, numBits));
	else
	  SWFShape_drawLine(shape, readSBits(f, numBits), 0);
    }
    else
    {
      int controlX = readSBits(f, numBits);
      int controlY = readSBits(f, numBits);
      int anchorX = readSBits(f, numBits);
      int anchorY = readSBits(f, numBits);

      SWFShape_drawCurve(shape, controlX, controlY, anchorX, anchorY);
    }
  }

  /* no idea where the pen was left */
  SWFShape_movePenTo(shape, startX, startY);
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
  assert(t>0.0 && t<1.0);

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
  assert(t>0.0 && t<1.0);

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

int Ming_cubicThreshold = 10000;

void Ming_setCubicThreshold(int num)
{
  Ming_cubicThreshold = num;
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

    SWFShape_drawCurveTo(shape, (int)floor(ex), (int)floor(ey),
			 (int)floor(pts->dx), (int)floor(pts->dy));
    return 1;
  }
}

int SWFShape_drawCubic(SWFShape shape, int bx, int by, int cx, int cy, int dx, int dy)
{
  int ax = SWFShape_getPenX(shape);
  int ay = SWFShape_getPenY(shape);

  bx += ax;
  by += ay;
  cx += bx;
  cy += by;
  dx += cx;
  dy += cy;

  return SWFShape_drawCubicTo(shape, bx, by, cx, cy, dx, dy);
}

/* returns number of splines used */
int SWFShape_drawCubicTo(SWFShape shape, int bx, int by, int cx, int cy, int dx, int dy)
{
  int ax = SWFShape_getPenX(shape);
  int ay = SWFShape_getPenY(shape);

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
