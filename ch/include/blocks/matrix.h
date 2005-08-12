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

/* matrix.h
 *
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_MATRIX_H_INCLUDED
#define SWF_MATRIX_H_INCLUDED

#include "ming.h"
#include "output.h"


SWFMatrix newSWFMatrix(float a, float b, float c, float d, int x, int y);

SWFMatrix SWFMatrix_dup(SWFMatrix matrix);

void destroySWFMatrix(SWFMatrix matrix);

void SWFOutput_writeMatrix(SWFOutput out, SWFMatrix matrix);

SWFMatrix newSWFRotateMatrix(float degrees);

int SWFMatrix_numBits(SWFMatrix matrix);

void SWFMatrix_set(SWFMatrix m, float a, float b, float c, float d, int x, int y);

void SWFMatrix_clearTranslate(SWFMatrix m);

void SWFMatrix_clearTransform(SWFMatrix m);

void SWFMatrix_apply(SWFMatrix m, double *x, double *y, int xlate);

void SWFMatrix_scaleXY(SWFMatrix matrix, float xScale, float yScale);

void SWFMatrix_leftMultiply(SWFMatrix ma, SWFMatrix mb);

void SWFMatrix_multiply(SWFMatrix a, SWFMatrix b);

void SWFMatrix_rotate(SWFMatrix matrix, float radians);

void SWFMatrix_scale(SWFMatrix matrix, float factor);

void SWFMatrix_translate(SWFMatrix matrix, int dx, int dy);

void SWFMatrix_moveTo(SWFMatrix matrix, int x, int y);

#endif /* SWF_MATRIX_H_INCLUDED */
