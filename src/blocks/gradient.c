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
#include "matrix.h"
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
	float focalPoint;
	int isFocalGradient;
};


SWFGradient
newSWFGradient()
{
	SWFGradient gradient = (SWFGradient) malloc(sizeof(struct SWFGradient_s));
	gradient->spreadMode = 0;
	gradient->interpolationMode = 0;
	gradient->nGrads = 0;
	gradient->isFocalGradient = 0;
	return gradient;
}

/* 
 * set gradient spread mode 
 * SWF8 allows to set different spread modes:
 * SWF_GRADIENT_PAD (default)
 * SWF_GRADIENT_REFLECT
 * SWF_GRADIENT_REPEAT
 */
void SWFGradient_setSpreadMode(SWFGradient gradient, GradientSpreadMode mode)
{
	gradient->spreadMode = mode;
}

/*
 * set gradient interpolation mode
 * SWF8 allows to set different interpolation modes:
 * SWF_GRADIENT_NORMAL (default)
 * SWF_GRADIENT_LINEAR
 */
void SWFGradient_setInterpolationMode(SWFGradient gradient, GradientInterpolationMode mode)
{
	gradient->interpolationMode = mode;
}

/*
 * creates a radial gradient with focal point
 * valid focalPoint value range is -1.0 ... 1.0
 * where -1.0 means the focal point is close to the left border of 
 * the radial gradient circle; 0.0 is in the center and 1.0 is close
 * to the right border.
 */
void SWFGradient_setFocalPoint(SWFGradient gradient, float focalPoint)
{
	gradient->isFocalGradient = 1;
	if(focalPoint < -1.0) focalPoint = -1.0;
	if(focalPoint > 1.0) focalPoint = 1.0;
	gradient->focalPoint = focalPoint;
}

int SWFGradient_isFocalGradient(SWFGradient gradient)
{
	return gradient->isFocalGradient;
}

void
destroySWFGradient(SWFGradient gradient)
{
	free(gradient);
}

/*
 * add a gradient control point
 * The ratio defines the position of the control point 
 * and RGBA defines its color. 
 * SWF <= 7 allows up to 8 control points 
 * SWF >= 8 allows up to 15 control points
 *
 * The ration parameter is a float and must be in a range 
 * between [0.0 ... 1.0].
 *
 * In order to use SWF8 gradient features you need to set 
 * SWFShapes version to SWF_SHAPE4. Use SWFShape_useVersion()
 * and friends.
 */
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

#define GRADIENT_SIZE 32768.0

void 
SWFGradientMatrix_update(SWFMatrix matrix, SWFRect bounds)
{
	int w, h;
	float scaleX, scaleY;
	SWFMatrix tmp;
	if(bounds == NULL)
		return;

	w = SWFRect_getWidth(bounds);
	h = SWFRect_getHeight(bounds);
	scaleX = w / GRADIENT_SIZE;
	scaleY = h / GRADIENT_SIZE;

	/* update matrix translation first, to be realtive to the gradient's 
	 * coordinate system. */
	SWFMatrix_moveTo(matrix, SWFMatrix_getTranslateX(matrix) / scaleX, 
		SWFMatrix_getTranslateY(matrix) / scaleY);
	
	tmp = newSWFMatrix(scaleX, 0, 0, scaleY, bounds->minX + w/2,  bounds->minY + h/2);
	/* temporary matrix scales gradient to given bounds and centers it. */
	/* all transformations done by the user are "applied" on the tmp matrix */
	/* matrix = matrix * tmp -> "matrix is followed by tmp" */
	SWFMatrix_multiply(matrix, tmp);
	destroySWFMatrix(tmp);	
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
		flags |= (0x3 & gradient->interpolationMode) << 4;
		flags |= (0x3 & gradient->spreadMode) << 6;
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

		if ( shapeType >= SWF_DEFINESHAPE3 )
			SWFOutput_writeUInt8(out, gradient->entries[i].a);
	}

	if(shapeType == SWF_DEFINESHAPE4 && gradient->isFocalGradient)
		SWFOutput_writeFixed8(out, gradient->focalPoint);
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
