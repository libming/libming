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

/* $Id$ */

#ifndef SWF_CXFORM_H_INCLUDED
#define SWF_CXFORM_H_INCLUDED

typedef struct SWFCXform_s *SWFCXform;

#include "output.h"
#include "blocktypes.h"


/* create a new color transform with the given parameters */

SWFCXform newSWFCXform (int rAdd, int gAdd, int bAdd, int aAdd,
			float rMult, float gMult, float bMult, float aMult);


/* create a new color transform with the given additive parameters and
   default multiplicative */

SWFCXform newSWFAddCXform (int rAdd, int gAdd, int bAdd, int aAdd);


/* create a new color transform with the given multiplicative parameters
   and default additive */

SWFCXform newSWFMultCXform (float rMult, float gMult, float bMult,
			    float aMult);


/* set the additive part of the color transform to the given parameters */

void SWFCXform_setColorAdd (SWFCXform cXform,
			    int rAdd, int gAdd, int bAdd, int aAdd);


/* set the multiplicative part of the color transform to the given
   parameters */

void SWFCXform_setColorMult (SWFCXform cXform,
			     float rMult, float gMult, float bMult,
			     float aMult);


/* destroy this color transform */

void destroySWFCXform (SWFCXform cXform);


/* write this color transform to the given output object-
   shapetype indicates whether we include the alpha components */

void SWFOutput_writeCXform (SWFOutput out, SWFCXform cXform,
			    SWFBlocktype shapetype);


#endif /* SWF_CXFORM_H_INCLUDED */
