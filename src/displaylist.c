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

#include <stdlib.h>
/* not used? #include <string.h> */
#include <math.h>

#include "blocks/placeobject.h"
#include "blocks/character.h"
#include "blocks/matrix.h"
#include "displaylist.h"
#include "position.h"
#include "blocks/soundstream.h"
#include "blocks/outputblock.h"
#include "blocks/button.h"
#include "blocks/videostream.h"
#include "blocks/blocktypes.h"
#include "blocks/filter.h"
#include "blocks/sprite.h"
#include "blocks/scalinggrid.h"
#include "libming.h"

#define ITEM_NEW						(1<<0)
#define ITEM_REMOVED				(1<<1)
#define ITEM_TRANSFORMED		(1<<2)
#define ITEM_CXFORMCHANGED	(1<<3)
#define ITEM_RATIOCHANGED		(1<<4)

#define ITEM_CHANGED	(ITEM_TRANSFORMED|ITEM_CXFORMCHANGED|ITEM_RATIOCHANGED|ITEM_NEW)

struct SWFDisplayList_s
{
	SWFSoundStream soundStream;
	SWFDisplayItem head;
	SWFDisplayItem tail;
	byte isSprite;
	int depth;
};


void
destroySWFDisplayItem(SWFDisplayItem displayItem)
{
	if ( displayItem->position )
		destroySWFPosition(displayItem->position);

	if ( displayItem->matrix )
		destroySWFMatrix(displayItem->matrix);

	/* character is freed in blocklist */

	free(displayItem);
}


void
destroySWFDisplayList(SWFDisplayList displayList)
{
	SWFDisplayItem item = displayList->head, next;

	while ( item != NULL )
	{
		next = item->next;
		destroySWFDisplayItem(item);
		item = next;
	}

	free(displayList);
}


SWFDisplayList
newSWFDisplayList()
{
	SWFDisplayList list = (SWFDisplayList)malloc(sizeof(struct SWFDisplayList_s));

	list->isSprite = FALSE;
	list->head = NULL;
	list->tail = NULL;
	list->soundStream = NULL;
	list->depth = 0;

	return list;
}


SWFDisplayList
newSWFSpriteDisplayList()
{
	SWFDisplayList list = (SWFDisplayList)malloc(sizeof(struct SWFDisplayList_s));

	list->isSprite = TRUE;
	list->head = NULL;
	list->tail = NULL;
	list->soundStream = NULL;
	list->depth = 0;

	return list;
}


SWFDisplayItem
SWFDisplayList_add(SWFDisplayList list, SWFCharacter character)
{
	SWFDisplayItem item = (SWFDisplayItem) malloc(sizeof(struct SWFDisplayItem_s));

	item->flags = ITEM_NEW;
	item->next = NULL;
	item->depth = ++list->depth;

	item->matrix = newSWFMatrix(0, 0, 0, 0, 0, 0);
	item->position = newSWFPosition(item->matrix);

	item->block = newSWFPlaceObject2Block(item->depth);
	item->character = character;
	item->isPlaced = 0;

	SWFPlaceObject2Block_setCharacter(item->block, character);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);

	if ( list->tail )
		list->tail->next = item;
	else
		list->head = item;
	
	item->prev = list->tail;
	list->tail = item;
	// back pointer for endmask
	item->list = list;
	return item;
}


void
SWFDisplayItem_remove(SWFDisplayItem item)
{
	item->flags |= ITEM_REMOVED;
}

void 
SWFDisplayItem_removeFromList(SWFDisplayItem item, SWFBlockList blocklist)
{
		SWFDisplayList list;
		
		if(item == NULL || item->list == NULL || blocklist == NULL)
			return;

		list = item->list;
		if(item->next)
			item->next->prev = item->prev;
		
		if(item->prev)
			item->prev->next = item->next;		

		if(item == list->head)
			list->head = item->next;
		if(item == list->tail)
			list->tail = item->prev;

		if(item->isPlaced)
			SWFBlockList_addBlock(blocklist,
				(SWFBlock)newSWFRemoveObject2Block(item->depth));

		destroySWFDisplayItem(item);
}

static void
checkBlock(SWFDisplayItem item)
{
	if ( item->block == NULL )
		item->block = newSWFPlaceObject2Block(item->depth);

	if ( (item->flags & ITEM_NEW) == 0 )
		SWFPlaceObject2Block_setMove(item->block);
}


SWFCharacter
SWFDisplayItem_getCharacter(SWFDisplayItem item)
{
	return item->character;
}


int
SWFDisplayItem_getDepth(SWFDisplayItem item)
{
	return item->depth;
}


void
SWFDisplayItem_setDepth(SWFDisplayItem item, int depth)
{
	item->depth = depth;

	checkBlock(item);

	if ( (item->flags & ITEM_NEW) == 0 )
	{
		/* warn.. */
		return;
	}

	SWFPlaceObject2Block_setDepth(item->block, depth);
}


SWFMatrix
SWFDisplayItem_getMatrix(SWFDisplayItem item)
{
	checkBlock(item);
	return item->matrix;
}


void
SWFDisplayItem_move(SWFDisplayItem item, float x, float y)
{
	checkBlock(item);
	SWFPosition_move(item->position, x, y);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_moveTo(SWFDisplayItem item, float x, float y)
{
	checkBlock(item);
	SWFPosition_moveTo(item->position, x, y);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_getPosition(SWFDisplayItem item, float *x, float *y)
{
	// returns the current position of this display item into the provided
	// pointers, respects NULL values

	checkBlock(item);
	SWFPosition_getXY(item->position, x, y);
}


void
SWFDisplayItem_rotate(SWFDisplayItem item, float degrees)
{
	checkBlock(item);
	SWFPosition_rotate(item->position, degrees);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees)
{
	checkBlock(item);
	SWFPosition_rotateTo(item->position, degrees);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_getRotation(SWFDisplayItem item, float *degrees)
{
	// returns the current rotation of this display item into the given
	// pointer, respects NULL value

	checkBlock(item);

	if ( degrees != NULL )
		*degrees = SWFPosition_getRotation(item->position);
}


void
SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale)
{
	checkBlock(item);
	SWFPosition_scaleXY(item->position, xScale, yScale);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale)
{
	checkBlock(item);
	SWFPosition_scaleXYTo(item->position, xScale, yScale);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_getScale(SWFDisplayItem item, float *xScale, float *yScale)
{
	// returns the current x- and y-scale of this display item into the given
	// pointers, respects NULL values

	checkBlock(item);
	SWFPosition_getXYScale(item->position, xScale, yScale);
}


void
SWFDisplayItem_skewX(SWFDisplayItem item, float x)
{
	checkBlock(item);
	SWFPosition_skewX(item->position, x);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_skewXTo(SWFDisplayItem item, float x)
{
	checkBlock(item);
	SWFPosition_skewXTo(item->position, x);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_skewY(SWFDisplayItem item, float y)
{
	checkBlock(item);
	SWFPosition_skewY(item->position, y);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_skewYTo(SWFDisplayItem item, float y)
{
	checkBlock(item);
	SWFPosition_skewYTo(item->position, y);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_getSkew(SWFDisplayItem item, float *xSkew, float *ySkew)
{
	// returns the current x- and y-skew of this display item into the given
	// pointers, respects NULL values

	checkBlock(item);
	SWFPosition_getXYSkew(item->position, xSkew, ySkew);
}


void
SWFDisplayItem_setMatrix(SWFDisplayItem item,
												 float a, float b, float c, float d, float x, float y)
{
	checkBlock(item);
	SWFPosition_setMatrix(item->position, a, b, c, d, x, y);
	SWFPlaceObject2Block_setMatrix(item->block, item->matrix);
}


void
SWFDisplayItem_setName(SWFDisplayItem item, const char *name)
{
	checkBlock(item);

	if ( (item->flags & ITEM_NEW) == 0 )
	{
		/* warn.. */
		return;
	}

	/* item->block is never null when ITEM_NEW set */
	SWFPlaceObject2Block_setName(item->block, name);
}


void
SWFDisplayItem_setMaskLevel(SWFDisplayItem item, int masklevel)
{
	checkBlock(item);

	if ( (item->flags & ITEM_NEW) == 0 )
	{
		/* warn.. */
		return;
	}

	/* item->block is never null when ITEM_NEW set */
	SWFPlaceObject2Block_setMaskLevel(item->block, masklevel);
}


void
SWFDisplayItem_endMask(SWFDisplayItem item)
{
	checkBlock(item);
	SWFPlaceObject2Block_setMaskLevel(item->block, item->list->depth);
}


void
SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio)
{
	checkBlock(item);
	SWFPlaceObject2Block_setRatio(item->block, (int)rint(ratio*65535));
}


void
SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform)
{
	checkBlock(item);
	SWFPlaceObject2Block_setCXform(item->block, cXform);
}


void
SWFDisplayItem_setColorAdd(SWFDisplayItem item,	int r, int g, int b, int a)
{
	checkBlock(item);
	SWFPlaceObject2Block_setColorAdd(item->block, r, g, b, a);
}


void
SWFDisplayItem_setColorMult(SWFDisplayItem item,
														float r, float g, float b, float a)
{
	checkBlock(item);
	SWFPlaceObject2Block_setColorMult(item->block, r, g, b, a);
}


void
SWFDisplayItem_addAction(SWFDisplayItem item, SWFAction action, int flags)
{
	checkBlock(item);

	if ( (item->flags & ITEM_NEW) == 0 )
	{
		/* warn.. */
		return;
	}

	/* item->block is never null when ITEM_NEW set */
	SWFPlaceObject2Block_addAction(item->block, action, flags);
}

/*
 * Methods for reading position data
 *  - added by David McNab <david@rebirthing.co.nz>
 */

float SWFDisplayItem_get_x(SWFDisplayItem item)
{
  float x;
  x = SWFPosition_getX(item->position);
  return x;
}

float SWFDisplayItem_get_y(SWFDisplayItem item)
{
  return SWFPosition_getY(item->position);
}

float SWFDisplayItem_get_xScale(SWFDisplayItem item)
{
  return SWFPosition_getXScale(item->position);
}

float SWFDisplayItem_get_yScale(SWFDisplayItem item)
{
  return SWFPosition_getYScale(item->position);
}

float SWFDisplayItem_get_xSkew(SWFDisplayItem item)
{
  return SWFPosition_getXSkew(item->position);
}

float SWFDisplayItem_get_ySkew(SWFDisplayItem item)
{
  return SWFPosition_getYSkew(item->position);
}

float SWFDisplayItem_get_rot(SWFDisplayItem item)
{
  return SWFPosition_getRotation(item->position);
}


/* 
 * adds a surface filter
 * Adds a bitmap filter to the display object. 
 * Also sets cache-as-bitmap flag. 
 * See SWFFilter for possible filter objects. Filter can 
 * only be applied to buttons, sprite/movieclips and text
 */ 
void SWFDisplayItem_addFilter(SWFDisplayItem item,
                              SWFFilter filter /* filter */)
{
	SWFBlock block;
	block = (SWFBlock)item->character;
	if(!SWFFilter_testBlockType(block->type))
	{
		SWF_warn("Filter can only be applied to buttons, sprite/movieclips and text\n");
		return;
	}
	checkBlock(item);
	SWFPlaceObject2Block_setCacheFlag(item->block, 1);
	SWFPlaceObject2Block_addFilter(item->block, filter);
	
}


/*
 * set blend mdoe
 * Sets an alternative blend mode instead of default alpha blend.
 * possible modes are:
 * SWFBLEND_MODE_NORMAL
 * SWFBLEND_MODE_LAYER
 * SWFBLEND_MODE_MULT
 * SWFBLEND_MODE_SCREEN
 * SWFBLEND_MODE_DARKEN
 * SWFBLEND_MODE_ADD
 * SWFBLEND_MODE_SUB
 * SWFBLEND_MODE_DIFF
 * SWFBLEND_MODE_DIFF
 * SWFBLEND_MODE_INV
 * SWFBLDEN_MODE_ALPHA
 * SWFBLEND_MDOE_ERASE
 * SWFBLEND_MDOE_OVERLAY
 * SWFBLEND_MODE_HARDLIGHT
 */
void SWFDisplayItem_setBlendMode(SWFDisplayItem item,
			int mode /* blend mode */)
{
	checkBlock(item);
	SWFPlaceObject2Block_setBlendMode(item->block, mode);
}


/*
 * set cashing flag
 * If this flag ist set, the character can be cached as a bitmap. This might
 * improve rednering speed, if the object dos no change often.
 * This feature is for SWF version >= 8 only.
 */
void SWFDisplayItem_cacheAsBitmap(SWFDisplayItem item, 
	int flag /* flag 0 or 1 */)
{
	checkBlock(item);
	SWFPlaceObject2Block_setCacheFlag(item->block, flag);
}

void
SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSoundStream stream)
{
	list->soundStream = stream;
}


void
SWFDisplayList_rewindSoundStream(SWFDisplayList list)
{
	// XXX - this is a hack, should be replaced..

	if ( list->soundStream )
		SWFSoundStream_rewind(list->soundStream);
}

void
SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist)
{
	SWFDisplayItem item = list->head, next;
	SWFCharacter character;

	if ( list->soundStream )
	{
		SWFBlock stream = SWFSoundStream_getStreamBlock(list->soundStream);

		if ( stream )
			SWFBlockList_addBlock(blocklist, stream);
	}	

	while ( item != NULL )
	{
		character = item->character;

		//		if ( character != NULL )
		//			SWFBlockList_resolveCharacterDependencies(blocklist, character);

		if ( item->flags & ITEM_REMOVED )
		{
			next = item->next;
			SWFDisplayItem_removeFromList(item, blocklist);
			item = next;
			continue;
		}
		
		if ( character != NULL &&
				 !SWFBlock_isDefined((SWFBlock)character) &&
				 !list->isSprite )
		{
			SWFBlockList_addBlock(blocklist, (SWFBlock)character);
		}

		if ( item->block != NULL )
		{
			if(!item->isPlaced && character->onPlace)
				character->onPlace(item, blocklist);
			SWFBlockList_addBlock(blocklist, (SWFBlock)item->block);
			item->isPlaced = 1;
		}
		
		if (character && character->onFrame)
			character->onFrame(item, blocklist);

		item->flags = 0;
		item->block = NULL;

		item = item->next;
	}
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
