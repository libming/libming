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

#include "linestyle.h"

SWFLineStyle newSWFLineStyle(unsigned short width,
			     byte r, byte g, byte b, byte a)
{
  SWFLineStyle line = (SWFLineStyle)malloc(LINESTYLE_SIZE);

  line->width = width;
  line->r = r;
  line->g = g;
  line->b = b;
  line->a = a;

  return line;
}

byte SWFLineStyle_equals(SWFLineStyle line, unsigned short width,
			 byte r, byte g, byte b, byte a)
{
  if(line->width == 0 && width == 0)
    return TRUE;

  if(line->width == width &&
     line->r == r &&
     line->g == g &&
     line->b == b &&
     line->a == a)
    return TRUE;

  return FALSE;
}

void SWFOutput_writeLineStyles(SWFOutput out,
			       SWFLineStyle *lines, int nLines,
			       SWFBlocktype shapeType)
{
  SWFLineStyle line;
  int i;

  if(nLines<255)
    SWFOutput_writeUInt8(out, nLines);
  else
  {
    SWFOutput_writeUInt8(out, 255);
    SWFOutput_writeUInt16(out, nLines);
  }

  for(i=0; i<nLines; ++i)
  {
    line = lines[i];

    SWFOutput_writeUInt16(out, line->width);
    SWFOutput_writeUInt8(out, line->r);
    SWFOutput_writeUInt8(out, line->g);
    SWFOutput_writeUInt8(out, line->b);

    if(shapeType==SWF_DEFINESHAPE3)
      SWFOutput_writeUInt8(out, line->a);

    ++line;
  }
}

void SWFOutput_writeMorphLineStyles(SWFOutput out,
				    SWFLineStyle *lines1, int nLines1,
				    SWFLineStyle *lines2, int nLines2)
{
  SWFLineStyle line1, line2;
  int i;

  SWF_assert(nLines1 == nLines2);

  if(nLines1<255)
    SWFOutput_writeUInt8(out, nLines1);
  else
  {
    SWFOutput_writeUInt8(out, 255);
    SWFOutput_writeUInt16(out, nLines1);
  }

  for(i=0; i<nLines1; ++i)
  {
    line1 = lines1[i];
    line2 = lines2[i];

    SWFOutput_writeUInt16(out, line1->width);
    SWFOutput_writeUInt16(out, line2->width);
    SWFOutput_writeUInt8(out, line1->r);
    SWFOutput_writeUInt8(out, line1->g);
    SWFOutput_writeUInt8(out, line1->b);
    SWFOutput_writeUInt8(out, line1->a);
    SWFOutput_writeUInt8(out, line2->r);
    SWFOutput_writeUInt8(out, line2->g);
    SWFOutput_writeUInt8(out, line2->b);
    SWFOutput_writeUInt8(out, line2->a);

    ++line1;
    ++line2;
  }
}
