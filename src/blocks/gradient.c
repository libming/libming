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
#include <math.h>

#include "libming.h"
#include "gradient.h"

#include "libming.h"

struct gradientEntry
{
	byte ratio;
	byte r;
	byte g;
	byte b;
	byte a;
};

struct SWFGradient_s
{
	int spreadMode;
	int interpolationMode;
	struct gradientEntry entries[15];
	int nGrads;
};


SWFGradient
newSWFGradient()
{
	SWFGradient gradient = (SWFGradient) malloc(sizeof(struct SWFGradient_s));
	gradient->spreadMode = 0;
	gradient->interpolationMode = 0;
	gradient->nGrads = 0;

	return gradient;
}

/* 
 * set gradient spread mode 
 * SWF8 allows to set different spread modes:
 * PAD (default)
 * REFLECT
 * REPEAT
 */
void SWFGradient_setSpreadMode(SWFGradient gradient, GradientSpreadMode mode)
{
	gradient->spreadMode = mode;
}

/*
 * set gradient interpolation mode
 * SWF8 allows to set different interpolation modes:
 * NORMAL (default)
 * LINEAR
 */
void SWFGradient_setInterpolationMode(SWFGradient gradient, GradientInterpolationMode mode)
{
	gradient->interpolationMode;
}

void
destroySWFGradient(SWFGradient gradient)
{
	free(gradient);
}


void
SWFGradient_addEntry(SWFGradient gradient,
                     float ratio, byte r, byte g, byte b, byte a)
{
	int nGrads = gradient->nGrads;

	if ( nGrads == 15 )
		return;

	gradient->entries[nGrads].ratio = (byte)floor(255*ratio);
	gradient->entries[nGrads].r = r;
	gradient->entries[nGrads].g = g;
	gradient->entries[nGrads].b = b;
	gradient->entries[nGrads].a = a;

	++gradient->nGrads;
}

void 
SWFOutput_writeGradientAsFilter(SWFOutput out, SWFGradient gradient)
{
	int i;
	int nGrads = gradient->nGrads;
	
	if(nGrads > 8)
	{
		SWF_warn("Can't write more than 8 control points for filter gradients\n");
		nGrads = 8;
	}	
	
	SWFOutput_writeUInt8(out, nGrads); /* only 1-8 allowed */

	for ( i=0; i<nGrads; ++i )
	{
		SWFOutput_writeUInt8(out, gradient->entries[i].r);
		SWFOutput_writeUInt8(out, gradient->entries[i].g);
		SWFOutput_writeUInt8(out, gradient->entries[i].b);
		SWFOutput_writeUInt8(out, gradient->entries[i].a);
	}

	for ( i=0; i<nGrads; ++i )
		SWFOutput_writeUInt8(out, gradient->entries[i].ratio);

}

void
SWFOutput_writeGradient(SWFOutput out, SWFGradient gradient, SWFBlocktype shapeType)
{
	int i;
	int nGrads = gradient->nGrads;

	if(shapeType == SWF_DEFINESHAPE4)
	{
		byte flags ;
		nGrads = min(nGrads, 15);
		flags = nGrads;
		flags |= (0x3 & gradient->spreadMode) << 4;
		flags |= (0x3 & gradient->interpolationMode) << 6;
		SWFOutput_writeUInt8(out, flags); /* only 1-15 allowed */
	}
	else 
	{
		nGrads = min(nGrads, 8);
		SWFOutput_writeUInt8(out, nGrads); /* only 1-8 allowed */
	}

	for ( i=0; i<nGrads; ++i )
	{
		SWFOutput_writeUInt8(out, gradient->entries[i].ratio);
		SWFOutput_writeUInt8(out, gradient->entries[i].r);
		SWFOutput_writeUInt8(out, gradient->entries[i].g);
		SWFOutput_writeUInt8(out, gradient->entries[i].b);

		if ( shapeType == SWF_DEFINESHAPE3 )
			SWFOutput_writeUInt8(out, gradient->entries[i].a);
	}
}


void
SWFOutput_writeMorphGradient(SWFOutput out,
                             SWFGradient gradient1, SWFGradient gradient2)
{
	int i;
	int nGrads = min(min(gradient1->nGrads, gradient2->nGrads), 8);

	SWFOutput_writeUInt8(out, nGrads); /* only 1-8 allowed */

	for ( i=0; i<nGrads; ++i )
	{
		SWFOutput_writeUInt8(out, gradient1->entries[i].ratio);
		SWFOutput_writeUInt8(out, gradient1->entries[i].r);
		SWFOutput_writeUInt8(out, gradient1->entries[i].g);
		SWFOutput_writeUInt8(out, gradient1->entries[i].b);
		SWFOutput_writeUInt8(out, gradient1->entries[i].a);
		SWFOutput_writeUInt8(out, gradient2->entries[i].ratio);
		SWFOutput_writeUInt8(out, gradient2->entries[i].r);
		SWFOutput_writeUInt8(out, gradient2->entries[i].g);
		SWFOutput_writeUInt8(out, gradient2->entries[i].b);
		SWFOutput_writeUInt8(out, gradient2->entries[i].a);
	}
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
