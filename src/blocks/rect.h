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

#ifndef SWF_RECT_H_INCLUDED
#define SWF_RECT_H_INCLUDED

#include "libswf.h"
#include "output.h"

struct _swfRect
{
  int minX;
  int maxX;
  int minY;
  int maxY;
};
typedef struct _swfRect *SWFRect;

#define SWFRECT_SIZE sizeof(struct _swfRect)

SWFRect newSWFRect(int minX, int maxX, int minY, int maxY);
void destroySWFRect(SWFRect rect);

SWFRect SWFRect_copy(SWFRect rect);

int SWFRect_numBits(SWFRect rect);
void SWFOutput_writeRect(SWFOutput output, SWFRect rect);

int SWFRect_getWidth(SWFRect r);
int SWFRect_getHeight(SWFRect r);

/* internal functions */
void SWFRect_includeRect(SWFRect a, SWFRect b);
void SWFRect_includePoint(SWFRect a, int x, int y, int width);

#endif /* SWF_RECT_H_INCLUDED */
