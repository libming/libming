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

#include <stdlib.h>
#include "rect.h"

int SWFRect_numBits(SWFRect rect)
{
  return 5 + 4*max(max(SWFOutput_numSBits(rect->minX), 
		       SWFOutput_numSBits(rect->maxX)),
		   max(SWFOutput_numSBits(rect->minY),
		       SWFOutput_numSBits(rect->maxY)));
}
void SWFOutput_writeRect(SWFOutput out, SWFRect rect)
{
  int nBits = max(max(SWFOutput_numSBits(rect->minX),
		      SWFOutput_numSBits(rect->maxX)),
		  max(SWFOutput_numSBits(rect->minY),
		      SWFOutput_numSBits(rect->maxY)));

  SWF_assert(nBits<32);

  SWFOutput_writeBits(out, nBits, 5);
  SWFOutput_writeSBits(out, rect->minX, nBits);
  SWFOutput_writeSBits(out, rect->maxX, nBits);
  SWFOutput_writeSBits(out, rect->minY, nBits);
  SWFOutput_writeSBits(out, rect->maxY, nBits);
}

void destroySWFRect(SWFRect rect)
{
  free(rect);
}

SWFRect newSWFRect(int minX, int maxX, int minY, int maxY)
{
  SWFRect rect = malloc(SWFRECT_SIZE);
  rect->minX = min(minX, maxX);
  rect->maxX = max(minX, maxX);
  rect->minY = min(minY, maxY);
  rect->maxY = max(minY, maxY);
  return rect;
}

int SWFRect_getWidth(SWFRect r)
{
  return r->maxX - r->minX;
}
int SWFRect_getHeight(SWFRect r)
{
  return r->maxY - r->minY;
}

SWFRect SWFRect_copy(SWFRect rect)
{
  return newSWFRect(rect->minX, rect->maxX, rect->minY, rect->maxY);
}

void SWFRect_includeRect(SWFRect a, SWFRect b)
{
  if(b->minX < a->minX) a->minX = b->minX;
  if(b->maxX > a->maxX) a->maxX = b->maxX;
  if(b->minX < a->minX) a->minX = b->minX;
  if(b->maxX > a->maxX) a->maxX = b->maxX;
}

/* make rect big enough to include (x,y) + width extra */
void SWFRect_includePoint(SWFRect rect, int x, int y, int width)
{
  if(x-width-1 < rect->minX) rect->minX = x-width;
  if(x+width > rect->maxX) rect->maxX = x+width;
  if(y-width-1 < rect->minY) rect->minY = y-width;
  if(y+width > rect->maxY) rect->maxY = y+width;
}
