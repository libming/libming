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

#ifndef SWF_DBL_H_INCLUDED
#define SWF_DBL_H_INCLUDED

typedef struct SWFDBLBitmap_s *SWFDBLBitmap;

#include <stdio.h>

#include "bitmap.h"
#include "input.h"


/* create a new DBL (define bits lossless) bitmap from the given file */

SWFDBLBitmap newSWFDBLBitmap(FILE *f);


/* create a new DBL bitmap from the given input object */

SWFDBLBitmap newSWFDBLBitmap_fromInput(SWFInput input);


#endif /* SWF_DBL_H_INCLUDED */
