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

#include "cxform.h"

SWFCXform newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
		       float rMult, float gMult, float bMult, float aMult)
{
  SWFCXform cXform = malloc(CXFORM_SIZE);

  cXform->rMult = floor(256*rMult);
  cXform->gMult = floor(256*gMult);
  cXform->bMult = floor(256*bMult);
  cXform->aMult = floor(256*aMult);

  cXform->rAdd = rAdd;
  cXform->gAdd = gAdd;
  cXform->bAdd = bAdd;
  cXform->aAdd = aAdd;

  return cXform;
}

SWFCXform newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd)
{
  return newSWFCXform(rAdd, gAdd, bAdd, aAdd, 1.0, 1.0, 1.0, 1.0);
}

SWFCXform newSWFMultCXform(float rMult, float gMult, float bMult, float aMult)
{
  return newSWFCXform(0, 0, 0, 0, rMult, gMult, bMult, aMult);
}

void SWFCXform_setColorAdd(SWFCXform cXform,
			   int rAdd, int gAdd, int bAdd, int aAdd)
{
  cXform->rAdd = rAdd;
  cXform->gAdd = gAdd;
  cXform->bAdd = bAdd;
  cXform->aAdd = aAdd;
}

void SWFCXform_setColorMult(SWFCXform cXform,
			    float rMult, float gMult, float bMult, float aMult)
{
  cXform->rMult = floor(256*rMult);
  cXform->gMult = floor(256*gMult);
  cXform->bMult = floor(256*bMult);
  cXform->aMult = floor(256*aMult);
}

void destroySWFCXform(SWFCXform cXform)
{
  free(cXform);
}

void SWFOutput_writeCXform(SWFOutput out, SWFCXform cXform, SWFBlocktype type)
{
  int nBits = 0, hasAdd = 1, hasMult = 1;

  SWFOutput_byteAlign(out);

  if(cXform->rAdd == 0 && cXform->gAdd == 0 &&
     cXform->bAdd == 0 && cXform->aAdd == 0)
    hasAdd = 0;

  if(cXform->rMult == 256 && cXform->gMult == 256 &&
     cXform->bMult == 256 && cXform->aMult == 256)
    hasMult = 0;

  SWFOutput_writeBits(out, hasAdd, 1);
  SWFOutput_writeBits(out, hasMult, 1);

  if(hasAdd)
  {
    nBits = max(nBits, SWFOutput_numSBits(cXform->rAdd));
    nBits = max(nBits, SWFOutput_numSBits(cXform->gAdd));
    nBits = max(nBits, SWFOutput_numSBits(cXform->bAdd));

    if(type == SWF_PLACEOBJECT2)
      nBits = max(nBits, SWFOutput_numSBits(cXform->aAdd));
  }

  if(hasMult)
  {
    nBits = max(nBits, SWFOutput_numSBits(cXform->rMult));
    nBits = max(nBits, SWFOutput_numSBits(cXform->gMult));
    nBits = max(nBits, SWFOutput_numSBits(cXform->bMult));

    if(type == SWF_PLACEOBJECT2)
      nBits = max(nBits, SWFOutput_numSBits(cXform->aMult));
  }

  SWF_assert(nBits<16);
  SWFOutput_writeBits(out, nBits, 4);

  if(hasMult)
  {
    SWFOutput_writeSBits(out, cXform->rMult, nBits);
    SWFOutput_writeSBits(out, cXform->gMult, nBits);
    SWFOutput_writeSBits(out, cXform->bMult, nBits);

    if(type == SWF_PLACEOBJECT2)
      SWFOutput_writeSBits(out, cXform->aMult, nBits);
  }

  if(hasAdd)
  {
    SWFOutput_writeSBits(out, cXform->rAdd, nBits);
    SWFOutput_writeSBits(out, cXform->gAdd, nBits);
    SWFOutput_writeSBits(out, cXform->bAdd, nBits);

    if(type == SWF_PLACEOBJECT2)
      SWFOutput_writeSBits(out, cXform->aAdd, nBits);
  }
}
