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

#include "libming.h"
#include "sprite.h"
#include "method.h"


static void
writeSWFSpriteToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	int i;
	SWFSprite sprite = (SWFSprite)block;

	methodWriteUInt16(CHARACTERID(sprite), method, data);
	methodWriteUInt16(sprite->frames, method, data);

	for ( i=0; i<sprite->nBlocks; ++i )
		writeSWFBlockToMethod(sprite->blocks[i], method, data);
}


static int
completeSWFSprite(SWFBlock block)
{
	int i, length = 0;
	SWFSprite sprite = (SWFSprite)block;

	if ( sprite->nBlocks < 1 ||
			 sprite->blocks[sprite->nBlocks-1]->type != SWF_SHOWFRAME ||
			 sprite->frames <= sprite->totalFrames )
	{
		SWFSprite_addBlock(sprite, newSWFShowFrameBlock());
	}

	SWFSprite_addBlock(sprite, newSWFEndBlock());

	SWF_assert(block->swfVersion);
	for ( i=0; i<sprite->nBlocks; ++i )
	{
		/* 
 		set version information to sprite blocks 
		so we can assure a valid movie version at completion time
		*/   	
		sprite->blocks[i]->swfVersion = block->swfVersion;
		length += completeSWFBlock(sprite->blocks[i]);
	}

	return length + 4;
}


void
destroySWFSprite(SWFSprite sprite)
{
	int i;

	for ( i=0; i<sprite->nBlocks; ++i )
	{
		/* characters are destroyed separately */

		if ( !SWFBlock_isCharacter(sprite->blocks[i]) )
			destroySWFBlock(sprite->blocks[i]);
	}

	if ( sprite->blocks != NULL )
		free(sprite->blocks);

	destroySWFCharacter((SWFCharacter) sprite);
}

static int onPlace(SWFDisplayItem item, SWFBlockList list)
{
	int ret = 0;
	SWFSprite sprite = (SWFSprite)SWFDisplayItem_getCharacter(item);
	
	if(sprite->grid)
	{
		SWFBlockList_addBlock(list, (SWFBlock)sprite->grid);
		ret++;
	}
	if(sprite->initAction)
	{
		SWFBlockList_addBlock(list, (SWFBlock)sprite->initAction);
		ret++;
	}	
	return ret;
}

SWFSprite
newSWFSprite()
{
	SWFSprite sprite = (SWFSprite)malloc(sizeof(struct SWFSprite_s));

	SWFCharacterInit((SWFCharacter)sprite);

	CHARACTERID(sprite) = ++SWF_gNumCharacters;
	BLOCK(sprite)->type = SWF_DEFINESPRITE;
	BLOCK(sprite)->writeBlock = writeSWFSpriteToMethod;
	BLOCK(sprite)->complete = completeSWFSprite;
	BLOCK(sprite)->dtor = (destroySWFBlockMethod) destroySWFSprite;
	
	((SWFCharacter)sprite)->onPlace = onPlace;

	sprite->nBlocks = 0;
	sprite->blocks = NULL;
	sprite->frames = 0;
	sprite->totalFrames = 0;
	sprite->grid = NULL;
	sprite->initAction = NULL;
	return sprite;
}


void
SWFSprite_setNumberOfFrames(SWFSprite sprite, int totalFrames)
{
	sprite->totalFrames = totalFrames;
}


void
SWFSprite_addBlock(SWFSprite sprite, SWFBlock block)
{
	if ( block->type == SWF_SHOWFRAME )
		++sprite->frames;

	if ( block->type == SWF_SHOWFRAME ||
			 block->type == SWF_PLACEOBJECT ||
			 block->type == SWF_PLACEOBJECT2 ||
			 block->type == SWF_PLACEOBJECT3 || 		
			 block->type == SWF_REMOVEOBJECT ||
			 block->type == SWF_REMOVEOBJECT2 ||
			 block->type == SWF_DOACTION ||
			 block->type == SWF_STARTSOUND ||
			 block->type == SWF_FRAMELABEL ||
			 block->type == SWF_SOUNDSTREAMHEAD ||
			 block->type == SWF_SOUNDSTREAMHEAD2 ||
			 block->type == SWF_SOUNDSTREAMBLOCK ||
			 block->type == SWF_VIDEOFRAME ||
			 block->type == SWF_END )
	{
		sprite->blocks =
			(SWFBlock*) realloc(sprite->blocks, (sprite->nBlocks + 1) * sizeof(SWFBlock));

		sprite->blocks[sprite->nBlocks++] = block;
	}
	else
		SWF_warn("SWFSprite: invalid block %i\n", block->type);
}


void
SWFSprite_getDependencies(SWFSprite sprite, SWFCharacter** deps, int* nDeps)
{
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
