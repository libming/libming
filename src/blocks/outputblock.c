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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "outputblock.h"
#include "method.h"
#include "block.h"
#include "character.h"
#include "matrix.h"
#include "cxform.h"

#include "libming.h"

struct SWFOutputBlock_s
{
	struct SWFBlock_s block;
	SWFOutput output;
};


SWFOutput
SWFOutputBlock_getOutput(SWFOutputBlock block)
{
	return block->output;
}


int
SWFOutputBlock_getLength(SWFOutputBlock block)
{
	return SWFOutput_getLength(block->output);
}


void
writeSWFOutputBlockToStream(SWFBlock block,
														SWFByteOutputMethod method, void *data)
{
	SWFOutput out = ((SWFOutputBlock)block)->output;
	SWFOutput_writeToMethod(out, method, data);
}


int
getSWFOutputBlockLength(SWFBlock block)
{
	SWFOutput out = ((SWFOutputBlock)block)->output;

	SWFOutput_byteAlign(out);
	return SWFOutput_getLength(out);
}


void
destroySWFOutputBlock(SWFOutputBlock outputBlock)
{
	if ( outputBlock->output != NULL )
		destroySWFOutput(outputBlock->output);

	free(outputBlock);
}


SWFOutputBlock
newSWFOutputBlock(SWFOutput out, SWFBlocktype type)
{
	SWFOutputBlock block = (SWFOutputBlock)malloc(sizeof(struct SWFOutputBlock_s));

	SWFBlockInit((SWFBlock)block);

	BLOCK(block)->type = type;
	BLOCK(block)->writeBlock = writeSWFOutputBlockToStream;
	BLOCK(block)->complete = getSWFOutputBlockLength;
	BLOCK(block)->dtor = (destroySWFBlockMethod) destroySWFOutputBlock;

	block->output = out;

	return block;
}


/* placeObject2 block is in placeobject.c */

SWFOutputBlock
newSWFPlaceObjectBlock(SWFCharacter character, int depth,
											 SWFMatrix matrix, SWFCXform cXform)
{
	SWFOutput out = newSizedSWFOutput(40);

	SWFOutput_writeUInt16(out, CHARACTERID(character));
	SWFOutput_writeUInt16(out, depth);
	SWFOutput_writeMatrix(out, matrix); /* max length: 210 bits=27 bytes */

	if ( cXform )
		/* max length: 9 bytes */
		SWFOutput_writeCXform(out, cXform, SWF_PLACEOBJECT);

	return newSWFOutputBlock(out, SWF_PLACEOBJECT);
}


SWFOutputBlock
newSWFSetBackgroundBlock(byte r, byte g, byte b)
{
	SWFOutput out = newSizedSWFOutput(4);
	SWFOutput_writeUInt8(out, r);
	SWFOutput_writeUInt8(out, g);
	SWFOutput_writeUInt8(out, b);

	return newSWFOutputBlock(out, SWF_SETBACKGROUNDCOLOR);
}


SWFOutputBlock
newSWFRemoveObjectBlock(SWFCharacter character, int depth)
{
	SWFOutput out = newSizedSWFOutput(5);
	SWFOutput_writeUInt16(out, CHARACTERID(character));
	SWFOutput_writeUInt16(out, depth);

	return newSWFOutputBlock(out, SWF_REMOVEOBJECT);
}


SWFOutputBlock
newSWFRemoveObject2Block(int depth)
{
	SWFOutput out = newSizedSWFOutput(3);
	SWFOutput_writeUInt16(out, depth);

	return newSWFOutputBlock(out, SWF_REMOVEOBJECT2);
}


SWFOutputBlock
newSWFFrameLabelBlock(const char *string)
{
	SWFOutput out = newSizedSWFOutput(strlen(string)+1);
	SWFOutput_writeString(out, (byte*)string);

	return newSWFOutputBlock(out, SWF_FRAMELABEL);
}


SWFOutputBlock
newSWFNamedAnchorBlock(const char *string)
{
	SWFOutput out = newSizedSWFOutput(strlen(string)+2);
	SWFOutput_writeString(out, (byte*)string);
	SWFOutput_writeUInt8(out, 1);

	return newSWFOutputBlock(out, SWF_FRAMELABEL);
}


SWFOutputBlock
newSWFExportBlock(SWFExports exports, int nExports)
{
	int n, sum;
	SWFOutput out;

	for ( n = 0, sum = 2; n < nExports; ++n )
		sum += 2 + strlen(exports[n].name) + 1;

	out = newSizedSWFOutput(sum);
	SWFOutput_writeUInt16(out, nExports);

	for ( n = 0; n < nExports; ++n )
	{
		SWFOutput_writeUInt16(out, CHARACTERID(exports[n].block));
		SWFOutput_writeString(out, (byte*)exports[n].name);
	}

	return newSWFOutputBlock(out, SWF_EXPORTASSETS);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
