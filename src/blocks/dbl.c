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
#include "dbl.h"
#include "block.h"

struct SWFDBLBitmap_s
{
	struct SWFCharacter_s bitmap;

	SWFInput input;
};


static int
completeSWFDBLBitmap(SWFBlock block)
{
	return SWFBlock_getLength(block);
}


static void
writeSWFDBLBitmapToMethod(SWFBlock block,
													SWFByteOutputMethod method, void *data)
{
	SWFDBLBitmap dbl = (SWFDBLBitmap)block;
	int i;

	methodWriteUInt16(CHARACTERID(dbl), method, data);

	/* just dump the rest of the file */
	for ( i=block->length-2; i>0; --i )
		method((unsigned char)SWFInput_getChar(dbl->input), data);
}


SWFDBLBitmap
newSWFDBLBitmap_fromInput(SWFInput input)
{
	SWFDBLBitmap dbl;
	int version;
	int width, height;

	dbl = (SWFDBLBitmap)malloc(sizeof(struct SWFDBLBitmap_s));

	SWFCharacterInit((SWFCharacter)dbl);

	CHARACTERID(dbl) = ++SWF_gNumCharacters;

	BLOCK(dbl)->writeBlock = writeSWFDBLBitmapToMethod;
	BLOCK(dbl)->complete = completeSWFDBLBitmap;
	BLOCK(dbl)->dtor = destroySWFCharacter;

	dbl->input = input;

	if ( SWFInput_getChar(input) != 'D' ||
			 SWFInput_getChar(input) != 'B' )
	{
		SWF_error("File is not a DBL file!");
	}

	version = SWFInput_getChar(input);

	if ( version != 'L' && version != 'l' )
		SWF_error("File is not a DBL file!");

	switch ( SWFInput_getChar(input) )
	{
		case 1:
			BLOCK(dbl)->type = SWF_DEFINELOSSLESS;
			break;
		case 2:
			BLOCK(dbl)->type = SWF_DEFINELOSSLESS2;
			break;
		default:
			SWF_error("Unexpected DBL type byte!");
	}

	if ( version == 'l' )
	{
		BLOCK(dbl)->length = SWFInput_getUInt32_BE(input);
		BLOCK(dbl)->length += 2; /* character id */
	}
	else
	{
		/* first version used a 2-byte file length..	brilliant, eh? */

		BLOCK(dbl)->length = SWFInput_getUInt16_BE(input);
		BLOCK(dbl)->length += 2; /* character id */
	}

	SWFInput_getChar(input); /* format */

	width = SWFInput_getUInt16(input);
	height = SWFInput_getUInt16(input);

	/* roll back to beginning of dbl data */
	SWFInput_seek(input, -5, SEEK_CUR);

	CHARACTER(dbl)->bounds = newSWFRect(0, width, 0, height);

	return dbl;
}


static void
destroySWFDBLBitmap_andInputs(SWFBlock block)
{
	SWFDBLBitmap bitmap = (SWFDBLBitmap)block;

	if ( bitmap->input != NULL )
		destroySWFInput(bitmap->input);

	if ( CHARACTER(bitmap)->bounds != NULL )
		destroySWFRect(CHARACTER(bitmap)->bounds);

	destroySWFCharacter(block);
}


SWFDBLBitmap
newSWFDBLBitmap(FILE* f)
{
	SWFDBLBitmap dbl = newSWFDBLBitmap_fromInput(newSWFInput_file(f));
	BLOCK(dbl)->dtor = destroySWFDBLBitmap_andInputs;
	return dbl;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
