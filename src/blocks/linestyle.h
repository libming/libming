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

/* line.h */

#ifndef SWF_LINE_H_INCLUDED
#define SWF_LINE_H_INCLUDED

#include "output.h"
#include "blocktypes.h"

struct _lineStyle
{
  unsigned short width;
  byte r;
  byte g;
  byte b;
  byte a;
};
typedef struct _lineStyle *SWFLineStyle;

#define LINESTYLE_SIZE sizeof(struct _lineStyle)

SWFLineStyle newSWFLineStyle(unsigned short width,
			     byte r, byte g, byte b, byte a);

/* okay, this is kinda lame.. */
byte SWFLineStyle_equals(SWFLineStyle line, unsigned short width,
			 byte r, byte g, byte b, byte a);

void SWFOutput_writeLineStyles(SWFOutput out,
			       SWFLineStyle *lines, int nLines,
			       SWFBlocktype shapeType);

void SWFOutput_writeMorphLineStyles(SWFOutput out,
				    SWFLineStyle *lines1, int nLines1,
				    SWFLineStyle *lines2, int nLines2);

#endif /* SWF_LINE_H_INCLUDED */
