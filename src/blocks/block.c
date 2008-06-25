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

#include <math.h>
#include <stdlib.h>

#include "block.h"
#include "method.h"
#include "libming.h"


void
SWFBlock_setDefined(SWFBlock block)
{
	block->isDefined = TRUE;
}


byte
SWFBlock_isDefined(SWFBlock block)
{
	return block->isDefined;
}


int
SWFBlock_getLength(SWFBlock block)
{
	return block->length;
}


void
destroySWFBlock(SWFBlock block)
{
	if ( block->dtor )
		block->dtor(block);
	else
		free(block);
}


int
completeSWFBlock(SWFBlock block)
{
	switch(block->type)
	{
	case SWF_UNUSEDBLOCK:
	case SWF_MINGFONT:
		return 0;
	default:
		break;
	}
		
	if ( !block->completed )
	{
		if ( block->complete )
			block->length = block->complete(block);

		block->completed = TRUE;
	}

	if (block->type == SWF_PREBUILT)
		return block->length;
	if ( block->length > 62 ||
			 block->type == SWF_DEFINELOSSLESS ||
			 block->type == SWF_DEFINELOSSLESS2 )
	{
		return block->length + 6;
	}
	else
		return block->length + 2;
}


int
writeSWFBlockToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFBlocktype type = block->type;
	unsigned int length;

	switch(block->type)
	{
	case SWF_UNUSEDBLOCK:
	case SWF_MINGFONT:
		return 0;
	default:
		break;
	}

	if ( !block->completed )
		completeSWFBlock(block);

	length = block->length;

	/* write header */

	if(type == SWF_PREBUILTCLIP)
		type = SWF_DEFINESPRITE;
	if ( type == SWF_PREBUILT )
		;
	else if ( length > 62 ||
			 type == SWF_DEFINELOSSLESS ||
			 type == SWF_DEFINELOSSLESS2 )
	{
		/* yep, a definebitslossless block has to be long form, even if it's
			 under 63 bytes.. */
		method((unsigned char)(((type&0x03)<<6) + 0x3f), data);
		method((unsigned char)((type>>2) & 0xff), data);
		methodWriteUInt32(length, method, data);
		length += 6;
	}
	else
	{	methodWriteUInt16(length + ((type)<<6), method, data);
		length += 2;
	}

	if ( block->writeBlock )
		block->writeBlock(block, method, data);

	return length;
}


void
SWFBlockInit(SWFBlock block)
{
	block->type = SWF_END; // XXX - ???
	block->writeBlock = NULL;
	block->complete = NULL;
	block->dtor = NULL;

	block->length = 0;
	block->isDefined = FALSE;
	block->completed = FALSE;
	
	block->swfVersion = 0;
}


SWFBlock
newEmptySWFBlock(SWFBlocktype type)
{
	SWFBlock block = (SWFBlock) malloc(sizeof(struct SWFBlock_s));
	SWFBlockInit(block);
	block->type = type;

	return block;
}


SWFBlock
newSWFShowFrameBlock()
{
	return newEmptySWFBlock(SWF_SHOWFRAME);
}


SWFBlock
newSWFEndBlock()
{
	return newEmptySWFBlock(SWF_END);
}


SWFBlocktype
SWFBlock_getType(SWFBlock block)
{
	return block->type;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
