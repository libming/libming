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

/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include "blocktypes.h"
#include "output.h"
#include "matrix.h"
#include "fillstyle.h"
#include "error.h"
#include "gradient.h"
#include "character.h"
#include "libming.h"


struct SWFFillStyle_s
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


SWFFillStyle
newSWFSolidFillStyle(byte r, byte g, byte b, byte a)
{
	SWFFillStyle fill = (SWFFillStyle)malloc(sizeof(struct SWFFillStyle_s));

	fill->type = SWFFILL_SOLID;
	fill->data.solid.r = r;
	fill->data.solid.g = g;
	fill->data.solid.b = b;
	fill->data.solid.a = a;
	fill->matrix = NULL; /* make purify shut up */

	return fill;
}


SWFFillStyle
newSWFGradientFillStyle(SWFGradient gradient, byte flags)
{
	SWFFillStyle fill = (SWFFillStyle) malloc(sizeof(struct SWFFillStyle_s));

	if ( flags == SWFFILL_RADIAL_GRADIENT )
		fill->type = SWFFILL_RADIAL_GRADIENT;
	else
		fill->type = SWFFILL_LINEAR_GRADIENT;

	fill->data.gradient = gradient;
	fill->matrix = newSWFMatrix(1.0, 0, 0, 1.0, 0, 0);

	return fill;
}


SWFFillStyle
newSWFBitmapFillStyle(SWFBitmap bitmap, byte flags)
{
	SWFFillStyle fill = (SWFFillStyle) malloc(sizeof(struct SWFFillStyle_s));

	if ( flags == SWFFILL_CLIPPED_BITMAP )
		fill->type = SWFFILL_CLIPPED_BITMAP;
	else
		fill->type = SWFFILL_TILED_BITMAP;

	fill->data.bitmap = bitmap;
	fill->matrix = newSWFMatrix(20.0, 0, 0, 20.0, 0, 0);

	return fill;
}


void
SWFFill_setIdx(SWFFillStyle fill, int idx)
{
	fill->idx = idx;
}


int
SWFFill_getIdx(SWFFillStyle fill)
{
	return fill->idx;
}


SWFMatrix
SWFFillStyle_getMatrix(SWFFillStyle fill)
{
	return fill->matrix;
}


int
SWFFillStyle_equals(SWFFillStyle fill1, SWFFillStyle fill2)
{
	if ( fill1->type != fill2->type )
		return 0;

	switch ( fill1->type )
	{
		case SWFFILL_SOLID:
			return (fill1->data.solid.r == fill2->data.solid.r &&
							fill1->data.solid.g == fill2->data.solid.g &&
							fill1->data.solid.b == fill2->data.solid.b &&
							fill1->data.solid.a == fill2->data.solid.a);

		case SWFFILL_LINEAR_GRADIENT:
		case SWFFILL_RADIAL_GRADIENT:
			return (fill1->data.gradient == fill2->data.gradient);

		case SWFFILL_TILED_BITMAP:
		case SWFFILL_CLIPPED_BITMAP:
			return (fill1->data.bitmap == fill2->data.bitmap);

		default:
			SWF_error("Unknown fill type");
	}

	return 0;
}

void
SWFOutput_writeFillStyle(SWFOutput out, SWFFillStyle fill, 
                         SWFBlocktype shapeType)
{
	int type = fill->type;
	SWFOutput_writeUInt8(out, type);

	if ( type == SWFFILL_SOLID )
	{
		SWFOutput_writeUInt8(out, fill->data.solid.r);
		SWFOutput_writeUInt8(out, fill->data.solid.g);
		SWFOutput_writeUInt8(out, fill->data.solid.b);

		if ( shapeType == SWF_DEFINESHAPE3 )
			SWFOutput_writeUInt8(out, fill->data.solid.a);
	}
	else if ( type & SWFFILL_GRADIENT )
	{
		SWFOutput_writeMatrix(out, fill->matrix);
		SWFOutput_writeGradient(out, fill->data.gradient, shapeType);
	}
	else if ( type & SWFFILL_BITMAP )
	{
		SWFOutput_writeUInt16(out, CHARACTERID(fill->data.bitmap));
		SWFOutput_writeMatrix(out, fill->matrix);
	}
	else
		SWF_error("Unknown fill type: %i", type);
}

void
SWFOutput_writeFillStyles(SWFOutput out,
													SWFFillStyle *fills, int nFills,
													SWFBlocktype shapeType)
{
	int i;
	SWFFillStyle fill;

	if ( nFills < 255 )
	{
		SWFOutput_writeUInt8(out, nFills);
	}
	else
	{
		SWFOutput_writeUInt8(out, 255);
		SWFOutput_writeUInt16(out, nFills);
	}

	for ( i=0; i<nFills; ++i )
	{
		fill = fills[i];
		SWFOutput_writeFillStyle(out, fill, shapeType);
	}
}


void
SWFOutput_writeMorphFillStyles(SWFOutput out,
															 SWFFillStyle *fills1, int nFills1,
															 SWFFillStyle *fills2, int nFills2)
{
	int i, type;
	SWFFillStyle fill1, fill2;

	SWF_assert(nFills1 == nFills2);

	if ( nFills1 < 255 )
	{
		SWFOutput_writeUInt8(out, nFills1);
	}
	else
	{
		SWFOutput_writeUInt8(out, 255);
		SWFOutput_writeUInt16(out, nFills1);
	}

	for ( i=0; i<nFills1; ++i )
	{
		fill1 = fills1[0];
		fill2 = fills2[0];

		SWF_assert(fill1->type == fill2->type);

		type = fill1->type;

		SWFOutput_writeUInt8(out, type);

		if ( type == SWFFILL_SOLID )
		{
			SWFOutput_writeUInt8(out, fill1->data.solid.r);
			SWFOutput_writeUInt8(out, fill1->data.solid.g);
			SWFOutput_writeUInt8(out, fill1->data.solid.b);
			SWFOutput_writeUInt8(out, fill1->data.solid.a);
			SWFOutput_writeUInt8(out, fill2->data.solid.r);
			SWFOutput_writeUInt8(out, fill2->data.solid.g);
			SWFOutput_writeUInt8(out, fill2->data.solid.b);
			SWFOutput_writeUInt8(out, fill2->data.solid.a);
		}
		else if ( type & SWFFILL_GRADIENT )
		{
			SWFOutput_writeMatrix(out, fill1->matrix);
			SWFOutput_writeMatrix(out, fill2->matrix);
			SWFOutput_writeMorphGradient(out, fill1->data.gradient, fill2->data.gradient);
		}
		else if ( type & SWFFILL_BITMAP )
		{
			SWF_assert(CHARACTERID(fill1->data.bitmap) ==
								 CHARACTERID(fill2->data.bitmap));

			SWFOutput_writeUInt16(out, CHARACTERID(fill1->data.bitmap));
			SWFOutput_writeMatrix(out, fill1->matrix);
			SWFOutput_writeMatrix(out, fill2->matrix);
		}
		else
			SWF_assert(0);

		++fill1;
		++fill2;
	}
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
