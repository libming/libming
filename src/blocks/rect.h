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

/* rect.h
 *
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_RECT_H_INCLUDED
#define SWF_RECT_H_INCLUDED

#include "ming.h"
#include "libming.h"
#include "output.h"
#include "error.h"


typedef struct SWFRect_s *SWFRect;

struct SWFRect_s
{
  int minX;
  int maxX;
  int minY;
  int maxY;
};

SWFRect newSWFRect(int minX, int maxX, int minY, int maxY);

void destroySWFRect(SWFRect rect);

SWFRect SWFRect_copy(SWFRect rect);

int SWFRect_numBits(SWFRect rect);

void SWFOutput_writeRect(SWFOutput output, SWFRect rect);

int SWFRect_getWidth(SWFRect r);

int SWFRect_getHeight(SWFRect r);

void SWFRect_getBounds(SWFRect rect, int *minX, int *maxX, int *minY, int *maxY);

void SWFRect_setBounds(SWFRect r, int minX, int maxX, int minY, int maxY);

void SWFRect_includeRect(SWFRect a, SWFRect b);

void SWFRect_includePoint(SWFRect a, int x, int y, int width);

#endif /* SWF_RECT_H_INCLUDED */
