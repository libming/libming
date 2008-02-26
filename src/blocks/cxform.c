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
#include "cxform.h"
#include "error.h"


struct SWFCXform_s
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

/* 
 * creates a new color transform with the given parameters 
 */
SWFCXform
newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
						 float rMult, float gMult, float bMult, float aMult)
{
	SWFCXform cXform = (SWFCXform) malloc(sizeof(struct SWFCXform_s));

	cXform->rMult = (int)floor(256*rMult);
	cXform->gMult = (int)floor(256*gMult);
	cXform->bMult = (int)floor(256*bMult);
	cXform->aMult = (int)floor(256*aMult);

	cXform->rAdd = rAdd;
	cXform->gAdd = gAdd;
	cXform->bAdd = bAdd;
	cXform->aAdd = aAdd;

	return cXform;
}

/* 
 * creates a new color transform with the given additive parameters and
   default multiplicative 
 */
SWFCXform
newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd)
{
	return newSWFCXform(rAdd, gAdd, bAdd, aAdd, 1.0, 1.0, 1.0, 1.0);
}

/* 
 * creates a new color transform with the given multiplicative parameters
   and default additive 
 */
SWFCXform
newSWFMultCXform(float rMult, float gMult, float bMult, float aMult)
{
	return newSWFCXform(0, 0, 0, 0, rMult, gMult, bMult, aMult);
}

/* 
 * set the additive part of the color transform to the given parameters 
 */
void
SWFCXform_setColorAdd(SWFCXform cXform, int rAdd, int gAdd, int bAdd, int aAdd)
{
	cXform->rAdd = rAdd;
	cXform->gAdd = gAdd;
	cXform->bAdd = bAdd;
	cXform->aAdd = aAdd;
}

/* 
 * set the multiplicative part of the color transform to the given
   parameters 
 */
void
SWFCXform_setColorMult(SWFCXform cXform,
											 float rMult, float gMult, float bMult, float aMult)
{
	cXform->rMult = (int)floor(256*rMult);
	cXform->gMult = (int)floor(256*gMult);
	cXform->bMult = (int)floor(256*bMult);
	cXform->aMult = (int)floor(256*aMult);
}

/*
 * destroys a SWFCXform instance
 */
void
destroySWFCXform(SWFCXform cXform)
{
	free(cXform);
}


void
SWFOutput_writeCXform(SWFOutput out, SWFCXform cXform, SWFBlocktype type)
{
	int nBits = 0;
	int hasAdd, hasMult;

	SWFOutput_byteAlign(out);

	hasAdd = ( cXform->rAdd != 0 || cXform->gAdd != 0 ||
						 cXform->bAdd != 0 || cXform->aAdd != 0 );

	hasMult = ( cXform->rMult != 256 || cXform->gMult != 256 ||
							cXform->bMult != 256 || cXform->aMult != 256 );

	SWFOutput_writeBits(out, hasAdd ? 1 : 0, 1);
	SWFOutput_writeBits(out, hasMult ? 1 : 0, 1);

	if ( hasAdd )
	{
		nBits = max(nBits, SWFOutput_numSBits(cXform->rAdd));
		nBits = max(nBits, SWFOutput_numSBits(cXform->gAdd));
		nBits = max(nBits, SWFOutput_numSBits(cXform->bAdd));

		if ( type == SWF_PLACEOBJECT2 )
			nBits = max(nBits, SWFOutput_numSBits(cXform->aAdd));
	}

	if ( hasMult )
	{
		nBits = max(nBits, SWFOutput_numSBits(cXform->rMult));
		nBits = max(nBits, SWFOutput_numSBits(cXform->gMult));
		nBits = max(nBits, SWFOutput_numSBits(cXform->bMult));

		if ( type == SWF_PLACEOBJECT2 )
			nBits = max(nBits, SWFOutput_numSBits(cXform->aMult));
	}

	if ( nBits>=16 )
		SWF_error("color transform data out of scale");

	SWFOutput_writeBits(out, nBits, 4);

	if ( hasMult )
	{
		SWFOutput_writeSBits(out, cXform->rMult, nBits);
		SWFOutput_writeSBits(out, cXform->gMult, nBits);
		SWFOutput_writeSBits(out, cXform->bMult, nBits);

		if ( type == SWF_PLACEOBJECT2 )
			SWFOutput_writeSBits(out, cXform->aMult, nBits);
	}

	if ( hasAdd )
	{
		SWFOutput_writeSBits(out, cXform->rAdd, nBits);
		SWFOutput_writeSBits(out, cXform->gAdd, nBits);
		SWFOutput_writeSBits(out, cXform->bAdd, nBits);

		if ( type == SWF_PLACEOBJECT2 )
			SWFOutput_writeSBits(out, cXform->aAdd, nBits);
	}
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
