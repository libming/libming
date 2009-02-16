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
#include "method.h"
#include "error.h"
#include "input.h"
#include "libming.h"

#include "ming_config.h"

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

	/* If malloc failed, return NULL to signify this */
	if (NULL == dbl)
		return NULL;

	SWFCharacterInit((SWFCharacter)dbl);

	CHARACTERID(dbl) = ++SWF_gNumCharacters;

	BLOCK(dbl)->writeBlock = writeSWFDBLBitmapToMethod;
	BLOCK(dbl)->complete = completeSWFDBLBitmap;
	BLOCK(dbl)->dtor = (destroySWFBlockMethod) destroySWFCharacter;

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
writeSWFDBLBitmapDataToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFDBLBitmapData dbl = (SWFDBLBitmapData)block;
	int i;
	unsigned char *ptr;

	methodWriteUInt16(CHARACTERID(dbl), method, data);
	method(dbl->format, data);
	methodWriteUInt16(dbl->width, method, data);
	methodWriteUInt16(dbl->height, method, data);
	i=block->length-8;
	if(dbl->format == 3)	// palette image
		method(dbl->format2, data);
	else
		i++;
	/* just dump the rest of the file */
	for (ptr = dbl->data; i>0; --i )
		method(*ptr++, data);
}

static void
destroySWFDBLBitmapData(SWFDBLBitmapData bitmap)
{
	if ( bitmap->data != NULL )
	{
		free(bitmap->data);
	}

#if TRACK_ALLOCS
	ming_gc_remove_node(bitmap->gcnode);
#endif

	destroySWFCharacter((SWFCharacter) bitmap);
}

SWFDBLBitmapData
newSWFDBLBitmapData_fromData(dblData data)
{
	SWFDBLBitmapData dbl;

	dbl = (SWFDBLBitmapData)malloc(sizeof(struct SWFDBLBitmapData_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == dbl)
		return NULL;

	SWFCharacterInit((SWFCharacter)dbl);

	CHARACTERID(dbl) = ++SWF_gNumCharacters;

	BLOCK(dbl)->writeBlock = writeSWFDBLBitmapDataToMethod;
	BLOCK(dbl)->complete = completeSWFDBLBitmap;
	BLOCK(dbl)->dtor = (destroySWFBlockMethod) destroySWFDBLBitmapData;

	dbl->width = data->width;
	dbl->height = data->height;
	dbl->format = data->format;
	dbl->format2 = data->format2;
	dbl->data = data->data;

	if(data->hasalpha)
		BLOCK(dbl)->type = SWF_DEFINELOSSLESS2;
	else
		BLOCK(dbl)->type = SWF_DEFINELOSSLESS;

	BLOCK(dbl)->length = data->length;
	BLOCK(dbl)->length += 7; /* character id, format, width, height */
	if(dbl->format == 3)
		BLOCK(dbl)->length++;

	CHARACTER(dbl)->bounds = newSWFRect(0, dbl->width, 0, dbl->height);

#if TRACK_ALLOCS
	dbl->gcnode = ming_gc_add_node(dbl, (dtorfunctype)destroySWFDBLBitmapData);
#endif

	return dbl;
}

static void
destroySWFDBLBitmap_andInputs(SWFDBLBitmap bitmap)
{
	if ( bitmap->input != NULL )
		destroySWFInput(bitmap->input);

  // The bounds rectangle will be already freed in destroySWFCharacter
	/*if ( CHARACTER(bitmap)->bounds != NULL )
		destroySWFRect(CHARACTER(bitmap)->bounds);*/

	destroySWFCharacter((SWFCharacter) bitmap);
}

SWFDBLBitmap
newSWFDBLBitmap(FILE* f)
{
	SWFInput input = newSWFInput_file(f);
	SWFDBLBitmap dbl = NULL;

	/* If newSWFInput_file() failed, return NULL to signify this */
	if (NULL == input)
		return NULL;

	dbl = newSWFDBLBitmap_fromInput(input);

	/* If newSWFDBLBitmap_fromInput() failed, return NULL to signify this */
	if (NULL == dbl)
		return NULL;

	BLOCK(dbl)->dtor = (destroySWFBlockMethod) destroySWFDBLBitmap_andInputs;
	return dbl;
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
