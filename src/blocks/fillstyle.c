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


void destroySWFFillStyle(SWFFillStyle fill)
{
	if(fill->matrix != NULL)
		destroySWFMatrix(fill->matrix);
	free(fill);
}

void SWFFillStyle_addDependency(SWFFillStyle fill, SWFCharacter c)
{
	if( (fill->type & SWFFILL_BITMAP) && fill->data.bitmap)
		SWFCharacter_addDependency(c, (SWFCharacter)fill->data.bitmap);
}

SWFFillStyle
newSWFSolidFillStyle(byte r, byte g, byte b, byte a)
{
	SWFFillStyle fill = (SWFFillStyle)malloc(sizeof(struct SWFFillStyle_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == fill)
		return NULL;

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

	/* If malloc failed, return NULL to signify this */
	if (NULL == fill)
		return NULL;

	if ( flags == SWFFILL_RADIAL_GRADIENT )
		fill->type = SWFFILL_RADIAL_GRADIENT;
	else if(SWFGradient_isFocalGradient(gradient))
		fill->type = SWFFILL_FOCAL_GRADIENT;
	else
		fill->type = SWFFILL_LINEAR_GRADIENT;

	fill->data.gradient = gradient;
	fill->matrix = newSWFMatrix(1.0, 0, 0, 1.0, 0, 0);

	/* If newSWFMatrix() failed, return NULL to signify this */
	if (NULL == fill->matrix)
	{
		free(fill);
		return NULL;
	}

	return fill;
}


SWFFillStyle
newSWFBitmapFillStyle(SWFBitmap bitmap, byte flags)
{
	SWFFillStyle fill = (SWFFillStyle) malloc(sizeof(struct SWFFillStyle_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == fill)
		return NULL;

	switch(flags)
	{
		case SWFFILL_CLIPPED_BITMAP:
		case SWFFILL_TILED_BITMAP:
		case SWFFILL_NONSMOOTHED_TILED_BITMAP:
		case SWFFILL_NONSMOOTHED_CLIPPED_BITMAP:
			fill->type = flags;
			break;
		default:
			free(fill);
			SWF_warn("newSWFBitmapFillStyle: not a valid Bitmap FillStyle: %x\n", flags);
			return NULL;
	}

	fill->data.bitmap = bitmap;
	fill->matrix = newSWFMatrix(Ming_scale, 0, 0, Ming_scale, 0, 0);
	if (fill->matrix == NULL)
	{
		free(fill);
		return NULL;
	}
	return fill;
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
		case SWFFILL_FOCAL_GRADIENT:
			return (fill1->data.gradient == fill2->data.gradient);

		case SWFFILL_TILED_BITMAP:
		case SWFFILL_CLIPPED_BITMAP:
		case SWFFILL_NONSMOOTHED_TILED_BITMAP:
		case SWFFILL_NONSMOOTHED_CLIPPED_BITMAP:
			return (fill1->data.bitmap == fill2->data.bitmap);

		default:
			SWF_error("Unknown fill type");
	}

	return 0;
}

void
SWFOutput_writeFillStyle(SWFOutput out, SWFFillStyle fill, 
                         SWFBlocktype shapeType, SWFRect bounds)
{
	int type = fill->type;
	SWFOutput_writeUInt8(out, type);

	if ( type == SWFFILL_SOLID )
	{
		SWFOutput_writeUInt8(out, fill->data.solid.r);
		SWFOutput_writeUInt8(out, fill->data.solid.g);
		SWFOutput_writeUInt8(out, fill->data.solid.b);

		if ( shapeType >= SWF_DEFINESHAPE3 )
			SWFOutput_writeUInt8(out, fill->data.solid.a);
	}
	else if ( type & SWFFILL_GRADIENT )
	{
		SWFGradientMatrix_update(fill->matrix, bounds);
		SWFOutput_writeMatrix(out, fill->matrix);
		SWFOutput_writeGradient(out, fill->data.gradient, shapeType);
	}
	else if ( type & SWFFILL_BITMAP )
	{
		SWFOutput_writeUInt16(out, fill->data.bitmap ?
					   CHARACTERID(fill->data.bitmap) :
					   65535); /* magic number */
		SWFOutput_writeMatrix(out, fill->matrix);
	}
	else
		SWF_error("Unknown fill type: %i", type);
}

void
SWFOutput_writeFillStyles(SWFOutput out,
                          SWFFillStyle *fills, int nFills,
                          SWFBlocktype shapeType,
                          SWFRect bounds)
{
	int i;

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
		SWFOutput_writeFillStyle(out, fills[i], shapeType, bounds);
}


void 
SWFOutput_writeMorphFillStyle(SWFOutput out, SWFFillStyle fill1, SWFRect bounds1,
                              SWFFillStyle fill2, SWFRect bounds2)
{
	int type;
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
                SWFGradientMatrix_update(fill1->matrix, bounds1);
		SWFOutput_writeMatrix(out, fill1->matrix);
		SWFGradientMatrix_update(fill2->matrix, bounds2);
		SWFOutput_writeMatrix(out, fill2->matrix);
		SWFOutput_writeMorphGradient(out, fill1->data.gradient, fill2->data.gradient);
	}
	else if ( type & SWFFILL_BITMAP )
	{
		SWF_assert(CHARACTERID(fill1->data.bitmap) == CHARACTERID(fill2->data.bitmap));

		SWFOutput_writeUInt16(out, CHARACTERID(fill1->data.bitmap));
		SWFOutput_writeMatrix(out, fill1->matrix);
		SWFOutput_writeMatrix(out, fill2->matrix);
	}
	else
		SWF_assert(0);

}

void
SWFOutput_writeMorphFillStyles(SWFOutput out,
                               SWFFillStyle *fills1, int nFills1, SWFRect bounds1,
                               SWFFillStyle *fills2, int nFills2, SWFRect bounds2)
{
	int i;
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
		SWFOutput_writeMorphFillStyle(out, fills1[i], bounds1, 
			fills2[i], bounds2);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
