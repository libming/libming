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

#ifndef __C2MAN__
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#endif

#include "error.h"
#include "libming.h"
#include "matrix.h"


/* scale is in FB format- i.e., lower 16 bits are on the right of the
	 decimal point.. */

#define FIXEDBITS 16

SWFMatrix
newSWFMatrix(double a, double b, double c, double d, int x, int y)
{
	SWFMatrix m = (SWFMatrix)malloc(sizeof(struct SWFMatrix_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == m)
		return NULL;

	m->scaleX = a;
	m->rotate0 = b;
	m->rotate1 = c;
	m->scaleY = d;
	m->translateX = x;
	m->translateY = y;

	return m;
}


void
SWFMatrix_set(SWFMatrix m, double a, double b, double c, double d, int x, int y)
{
	m->scaleX = a;
	m->rotate0 = b;
	m->rotate1 = c;
	m->scaleY = d;
	m->translateX = x;
	m->translateY = y;
}


double
SWFMatrix_getScaleX(SWFMatrix m)
{
	return m->scaleX;
}


double
SWFMatrix_getRotate0(SWFMatrix m)
{
	return m->rotate0;
}


double
SWFMatrix_getRotate1(SWFMatrix m)
{
	return m->rotate1;
}


double
SWFMatrix_getScaleY(SWFMatrix m)
{
	return m->scaleY;
}


int
SWFMatrix_getTranslateX(SWFMatrix m)
{
	return m->translateX;
}


int
SWFMatrix_getTranslateY(SWFMatrix m)
{
	return m->translateY;
}


void
SWFMatrix_clearTranslate(SWFMatrix m)
{
	m->translateX = 0;
	m->translateY = 0;
}


void
SWFMatrix_clearTransform(SWFMatrix m)
{
	m->scaleX = 1.0;
	m->rotate0 = 0;
	m->rotate1 = 0;
	m->scaleY = 1.0;
}


SWFMatrix
SWFMatrix_dup(SWFMatrix matrix)
{
	SWFMatrix m = (SWFMatrix)malloc(sizeof(struct SWFMatrix_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == m)
		return NULL;

	memcpy(m, matrix, sizeof(struct SWFMatrix_s));
	return m;
}


void
destroySWFMatrix(SWFMatrix matrix)
{
	free(matrix);
}


int
SWFMatrix_numBits(SWFMatrix matrix)
{
	int bits = 7;

	if ( !((matrix->scaleX == 0 && matrix->scaleY == 0) ||
				 (matrix->scaleX == 1.0 && matrix->scaleY == 1.0)) )
	{
		bits += 5 + 2*max(SWFOutput_numSBits((int)matrix->scaleX),
					SWFOutput_numSBits((int)matrix->scaleY));
	}

	if ( matrix->rotate0 != 0 || matrix->rotate1 != 0 )
	{
		bits += 5 + 2*max(SWFOutput_numSBits((int)matrix->rotate0),
					SWFOutput_numSBits((int)matrix->rotate1));
	}

	if ( matrix->translateX != 0 || matrix->translateY != 0 )
	{
		bits += 2*max(SWFOutput_numSBits(matrix->translateX),
			SWFOutput_numSBits(matrix->translateY));
	}

	return bits;
}


void
SWFOutput_writeMatrix(SWFOutput out, SWFMatrix matrix)
{
	int nBits;

	SWFOutput_byteAlign(out);

	if (matrix->scaleX == 1.0 && matrix->scaleY == 1.0)
	{
		SWFOutput_writeBits(out, 0, 1);	 
	}
	else
	{
		int xScale = (int)floor(matrix->scaleX * (1<<FIXEDBITS));
		int yScale = (int)floor(matrix->scaleY * (1<<FIXEDBITS));

		SWFOutput_writeBits(out, 1, 1);
		nBits = max(SWFOutput_numSBits(xScale), SWFOutput_numSBits(yScale));
		if(nBits >= 32)
			SWF_error("SWFMatrix_scale: number is to big. "
				  " Requested %i bits\n", nBits);
		
		SWFOutput_writeBits(out, nBits, 5);
		SWFOutput_writeSBits(out, xScale, nBits);
		SWFOutput_writeSBits(out, yScale, nBits);
	}

	if ( matrix->rotate0 == 0 && matrix->rotate1 == 0 )
	{
		SWFOutput_writeBits(out, 0, 1);
	}
	else
	{
		int rot0 = (int)floor(matrix->rotate0 * (1<<FIXEDBITS));
		int rot1 = (int)floor(matrix->rotate1 * (1<<FIXEDBITS));

		SWFOutput_writeBits(out, 1, 1);
		nBits = max(SWFOutput_numSBits(rot0), SWFOutput_numSBits(rot1));
		if(nBits >= 32)
			SWF_error("SWFMatrix_rotate: number is to big. "
				  " Requested %i bits\n", nBits);

		SWFOutput_writeBits(out, nBits, 5);
		SWFOutput_writeSBits(out, rot0, nBits);
		SWFOutput_writeSBits(out, rot1, nBits);
	}

	if ( matrix->translateX != 0 || matrix->translateY != 0 )
	{
		nBits = max(SWFOutput_numSBits(matrix->translateX),
		SWFOutput_numSBits(matrix->translateY));

		if(nBits >= 32)
			SWF_error("SWFMatrix_translate: number is to big. "
				  " Requested %i bits\n", nBits);
	}
	else
		nBits = 0;

	SWFOutput_writeBits(out, nBits, 5);
	SWFOutput_writeSBits(out, matrix->translateX, nBits);
	SWFOutput_writeSBits(out, matrix->translateY, nBits);
}


void
SWFMatrix_apply(SWFMatrix m, double *x, double *y, int xlate)
{
	int newx, newy;

	if ( m == NULL )
		return;

	newx = (int)(m->scaleX * (*x) + m->rotate0 * (*y));
	newy = (int)(m->scaleY * (*y) + m->rotate1 * (*x));

	*x = newx + (xlate ? m->translateX : 0);
	*y = newy + (xlate ? m->translateY : 0); 
}


/*	| a b || e f |	 | ae+bg af+bh |
		| c d || g h | = | ce+dg cf+dh | */

/* ma = ma*mb */
void SWFMatrix_multiply(SWFMatrix ma, SWFMatrix mb)
{
	double a = ma->scaleX, b = ma->rotate0, c = ma->rotate1, d = ma->scaleY;
	double e = mb->scaleX, f = mb->rotate0, g = mb->rotate1, h = mb->scaleY;
	double tmp;

	ma->scaleX = a*e+b*g;
	ma->rotate0 = a*f+b*h;
	ma->rotate1 = c*e+d*g;
	ma->scaleY = c*f+d*h;

	tmp = e*ma->translateX + g*ma->translateY + mb->translateX;
	ma->translateY = (int)(f*ma->translateX + h*ma->translateY + mb->translateY);
	ma->translateX = (int)tmp;
}


/* mb = ma*mb */
void
SWFMatrix_leftMultiply(SWFMatrix ma, SWFMatrix mb)
{
	double a = ma->scaleX, b = ma->rotate0, c = ma->rotate1, d = ma->scaleY;
	double e = mb->scaleX, f = mb->rotate0, g = mb->rotate1, h = mb->scaleY;

	mb->scaleX = a*e+b*g;
	mb->rotate0 = a*f+b*h;
	mb->rotate1 = c*e+d*g;
	mb->scaleY = c*f+d*h;
}


SWFMatrix
newSWFRotateMatrix(double degrees)
{
	double r = degrees * M_PI/180;

	return newSWFMatrix(cos(r), sin(r), -sin(r), cos(r), 0, 0);
}


void
SWFMatrix_rotate(SWFMatrix matrix, double degrees)
{
	SWFMatrix rot = newSWFRotateMatrix(degrees);
	SWFMatrix_leftMultiply(rot,matrix);
	destroySWFMatrix(rot);
}


void
SWFMatrix_scaleXY(SWFMatrix matrix, double xScale, double yScale)
{
	matrix->scaleX = matrix->scaleX * xScale;
	matrix->rotate0 = matrix->rotate0 * xScale;
	matrix->rotate1 = matrix->rotate1 * yScale;
	matrix->scaleY = matrix->scaleY * yScale;
}


void
SWFMatrix_scale(SWFMatrix matrix, double factor)
{
	matrix->scaleX = matrix->scaleX * factor;
	matrix->scaleY = matrix->scaleY * factor;
	matrix->rotate0 = matrix->rotate0 * factor;
	matrix->rotate1 = matrix->rotate1 * factor;
}


void
SWFMatrix_translate(SWFMatrix matrix, int dx, int dy)
{
	matrix->translateX += dx;
	matrix->translateY += dy;
}


void
SWFMatrix_moveTo(SWFMatrix matrix, int x, int y)
{
	matrix->translateX = x;
	matrix->translateY = y;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
