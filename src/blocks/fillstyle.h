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

/* fill.h */

#ifndef SWF_FILL_H_INCLUDED
#define SWF_FILL_H_INCLUDED

#include "output.h"
#include "matrix.h"
#include "bitmap.h"
#include "gradient.h"
#include "blocktypes.h"

#define SWF_FILL_SOLID		 0x00
#define SWF_FILL_GRADIENT	 0x10
#define SWF_FILL_LINEAR_GRADIENT 0x10
#define SWF_FILL_RADIAL_GRADIENT 0x12
#define SWF_FILL_BITMAP		 0x40
#define SWF_FILL_TILED_BITMAP	 0x40
#define SWF_FILL_CLIPPED_BITMAP	 0x41

/* fillstyle is a fill instance on a shape */

struct _fillStyle
{
  byte type;
  SWFMatrix matrix;
  int idx;

  union
  {
    struct
    {
      byte r;
      byte g;
      byte b;
      byte a;
    } solid;

    SWFBitmap bitmap;
    SWFGradient gradient;

  } data;
};
typedef struct _fillStyle *SWFFillStyle;

#define FILLSTYLE_SIZE sizeof(struct _fillStyle)

SWFMatrix SWFFillStyle_getMatrix(SWFFillStyle fill);

int SWFFillStyle_equals(SWFFillStyle fill1, SWFFillStyle fill2);

void SWFOutput_writeFillStyles(SWFOutput out,
			       SWFFillStyle *fills, int nFills,
			       SWFBlocktype shapeType);

void SWFOutput_writeMorphFillStyles(SWFOutput out,
				    SWFFillStyle *fills1, int nFills1,
				    SWFFillStyle *fills2, int nFills2);

#endif /* FILL_H_INCLUDED */
