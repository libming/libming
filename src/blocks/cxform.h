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

#ifndef SWF_CXFORM_H_INCLUDED
#define SWF_CXFORM_H_INCLUDED

#include "output.h"
#include "blocktypes.h"

struct _cXform
{
  int rMult;
  int gMult;
  int bMult;
  int aMult;
  int rAdd;
  int gAdd;
  int bAdd;
  int aAdd;
};
typedef struct _cXform *SWFCXform;

#define CXFORM_SIZE sizeof(struct _cXform)

SWFCXform newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
		       float rMult, float gMult, float bMult, float aMult);

SWFCXform newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd);

SWFCXform newSWFMultCXform(float rMult, float gMult, float bMult, float aMult);

void SWFCXform_setColorAdd(SWFCXform cXform,
			   int rAdd, int gAdd, int bAdd, int aAdd);

void SWFCXform_setColorMult(SWFCXform cXform,
			    float rMult, float gMult, float bMult, float aMult);

void destroySWFCXform(SWFCXform cXform);

void SWFOutput_writeCXform(SWFOutput out, SWFCXform cXform, SWFBlocktype type);

#endif /* SWF_CXFORM_H_INCLUDED */
