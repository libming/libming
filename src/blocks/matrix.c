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
#include <math.h>
#include <float.h>

#include "output.h"
#include "matrix.h"

/* scale is in FB format- i.e., lower 16 bits are on the right of the
   decimal point.. */

#define FIXEDBITS 16

SWFMatrix newSWFMatrix(float a, float b, float c, float d, int x, int y)
{
  SWFMatrix m = malloc(sizeof(struct _matrix));
  m->scaleX = a;
  m->rotate0 = b;
  m->rotate1 = c;
  m->scaleY = d;
  m->translateX = x;
  m->translateY = y;

  return m;
}

void SWFMatrix_set(SWFMatrix m, float a, float b, float c, float d, int x, int y)
{
  m->scaleX = a;
  m->rotate0 = b;
  m->rotate1 = c;
  m->scaleY = d;
  m->translateX = x;
  m->translateY = y;
}

void SWFMatrix_clearTranslate(SWFMatrix m)
{
  m->translateX = 0;
  m->translateY = 0;
}

void SWFMatrix_clearTransform(SWFMatrix m)
{
  m->scaleX = 1.0;
  m->rotate0 = 0;
  m->rotate1 = 0;
  m->scaleY = 1.0;
}

void SWFMatrix_copy(SWFMatrix src, SWFMatrix dest)
{
  memcpy(dest, src, sizeof(struct _matrix));
}

SWFMatrix SWFMatrix_dup(SWFMatrix matrix)
{
  SWFMatrix m = malloc(sizeof(struct _matrix));
  return memcpy(m, matrix, sizeof(struct _matrix));
}

void destroySWFMatrix(SWFMatrix matrix)
{
  free(matrix);
}

int SWFMatrix_numBits(SWFMatrix matrix)
{
  int bits = 7;

  if(!((matrix->scaleX == 0 && matrix->scaleY == 0) ||
       (matrix->scaleX == 1.0 && matrix->scaleY == 1.0)))
    bits += 5 + 2*max(SWFOutput_numSBits(matrix->scaleX),
		      SWFOutput_numSBits(matrix->scaleY));

  if(matrix->rotate0 != 0 || matrix->rotate1 != 0)
    bits += 5 + 2*max(SWFOutput_numSBits(matrix->rotate0),
		      SWFOutput_numSBits(matrix->rotate1));

  if(matrix->translateX != 0 || matrix->translateY != 0)
    bits += 2*max(SWFOutput_numSBits(matrix->translateX),
		  SWFOutput_numSBits(matrix->translateY));

  return bits;
}

void SWFOutput_writeMatrix(SWFOutput out, SWFMatrix matrix)
{
  int nBits;

  SWFOutput_byteAlign(out);

  if((matrix->scaleX == 0 && matrix->scaleY == 0) ||
     (matrix->scaleX == 1.0 && matrix->scaleY == 1.0))
    SWFOutput_writeBits(out, 0, 1);
  else
  {
    int xScale = floor(matrix->scaleX * (1<<FIXEDBITS));
    int yScale = floor(matrix->scaleY * (1<<FIXEDBITS));

    SWFOutput_writeBits(out, 1, 1);
    nBits = max(SWFOutput_numSBits(xScale), SWFOutput_numSBits(yScale));
    SWFOutput_writeBits(out, nBits, 5);
    SWFOutput_writeSBits(out, xScale, nBits);
    SWFOutput_writeSBits(out, yScale, nBits);
  }

  if(matrix->rotate0 == 0 && matrix->rotate1 == 0)
    SWFOutput_writeBits(out, 0, 1);
  else
  {
    int rot0 = floor(matrix->rotate0 * (1<<FIXEDBITS));
    int rot1 = floor(matrix->rotate1 * (1<<FIXEDBITS));

    SWFOutput_writeBits(out, 1, 1);
    nBits = max(SWFOutput_numSBits(rot0), SWFOutput_numSBits(rot1));
    SWFOutput_writeBits(out, nBits, 5);
    SWFOutput_writeSBits(out, rot0, nBits);
    SWFOutput_writeSBits(out, rot1, nBits);
  }

  if(matrix->translateX != 0 || matrix->translateY != 0)
    nBits = max(SWFOutput_numSBits(matrix->translateX), SWFOutput_numSBits(matrix->translateY));
  else
    nBits = 0;

  SWFOutput_writeBits(out, nBits, 5);
  SWFOutput_writeSBits(out, matrix->translateX, nBits);
  SWFOutput_writeSBits(out, matrix->translateY, nBits);
}

/* I really hate putting this stuff here, but how else can we
   hide the matrix implementation? */



/*  | a b || e f |   | ae+bg af+bh |
    | c d || g h | = | ce+dg cf+dh | */

/* ma = ma*mb */
void SWFMatrix_multiply(SWFMatrix ma, SWFMatrix mb)
{
  float a = ma->scaleX, b = ma->rotate0, c = ma->rotate1, d = ma->scaleY;
  float e = mb->scaleX, f = mb->rotate0, g = mb->rotate1, h = mb->scaleY;
  float tmp;

  ma->scaleX = a*e+b*g;
  ma->rotate0 = a*f+b*h;
  ma->rotate1 = c*e+d*g;
  ma->scaleY = c*f+d*h;

  tmp = e*ma->translateX + g*ma->translateY + mb->translateX;
  ma->translateY = f*ma->translateX + h*ma->translateY + mb->translateY;
  ma->translateX = tmp;
}

/* mb = ma*mb */
void SWFMatrix_leftMultiply(SWFMatrix ma, SWFMatrix mb)
{
  float a = ma->scaleX, b = ma->rotate0, c = ma->rotate1, d = ma->scaleY;
  float e = mb->scaleX, f = mb->rotate0, g = mb->rotate1, h = mb->scaleY;

  mb->scaleX = a*e+b*g;
  mb->rotate0 = a*f+b*h;
  mb->rotate1 = c*e+d*g;
  mb->scaleY = c*f+d*h;
}

SWFMatrix newSWFRotateMatrix(float degrees)
{
  return newSWFMatrix( cos(degrees*M_PI/180), sin(degrees*M_PI/180),
		      -sin(degrees*M_PI/180), cos(degrees*M_PI/180),
		      0, 0);
}
void SWFMatrix_rotate(SWFMatrix matrix, float degrees)
{
  SWFMatrix rot = newSWFRotateMatrix(degrees);
  SWFMatrix_leftMultiply(rot,matrix);
  destroySWFMatrix(rot);
}

void SWFMatrix_scaleXY(SWFMatrix matrix, float xScale, float yScale)
{
  matrix->scaleX = matrix->scaleX * xScale;
  matrix->rotate0 = matrix->rotate0 * xScale;
  matrix->rotate1 = matrix->rotate1 * yScale;
  matrix->scaleY = matrix->scaleY * yScale;
}

void SWFMatrix_scale(SWFMatrix matrix, float factor)
{
  matrix->scaleX = matrix->scaleX * factor;
  matrix->scaleY = matrix->scaleY * factor;
  matrix->rotate0 = matrix->rotate0 * factor;
  matrix->rotate1 = matrix->rotate1 * factor;
}

void SWFMatrix_translate(SWFMatrix matrix, int dx, int dy)
{
  matrix->translateX += dx;
  matrix->translateY += dy;
}

void SWFMatrix_moveTo(SWFMatrix matrix, int x, int y)
{
  matrix->translateX = x;
  matrix->translateY = y;
}
