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
#include <string.h>
#include <math.h>

#include "displaylist.h"
#include "blocktypes.h"

void destroySWFDisplayItem(SWFDisplayItem item)
{
  if(item->position)
    destroySWFPosition(item->position);

  if(item->cXform)
    destroySWFCXform(item->cXform);

  if(item->matrix)
    destroySWFMatrix(item->matrix);

  if(item->name)
    free(item->name);

  /* character is freed in blocklist */

  free(item);
}

void destroySWFDisplayList(SWFDisplayList list)
{
  SWFDisplayItem item = list->head, next;

  while(item != NULL)
  {
    next = item->next;
    destroySWFDisplayItem(item);
    item = next;
  }

  if(list->soundStream != NULL)
    destroySWFSound(list->soundStream);

  free(list);
}

SWFDisplayList newSWFDisplayList()
{
  SWFDisplayList list = calloc(1, SWFDISPLAYLIST_SIZE);

  list->isSprite = FALSE;
  list->head = NULL;
  list->tail = NULL;
  list->soundStream = NULL;

  return list;
}

SWFDisplayList newSWFSpriteDisplayList()
{
  SWFDisplayList list = calloc(1, SWFDISPLAYLIST_SIZE);

  list->isSprite = TRUE;
  list->head = NULL;
  list->tail = NULL;
  list->soundStream = NULL;

  return list;
}

SWFDisplayItem SWFDisplayList_add(SWFDisplayList list, SWFCharacter character)
{
  SWFDisplayItem item = calloc(1, SWFDISPLAYITEM_SIZE);

  item->flags = ITEM_NEW | ITEM_TRANSFORMED;
  item->next = NULL;
  item->character = character;
  item->depth = ++list->depth;
  item->matrix = newSWFMatrix(0, 0, 0, 0, 0, 0);
  item->position = newSWFPosition(item->matrix);
  item->cXform = NULL;

  if(list->tail)
    list->tail->next = item;
  else
    list->head = item;

  list->tail = item;
  return item;
}

void SWFDisplayItem_remove(SWFDisplayItem item)
{
  item->flags |= ITEM_REMOVED;
}

int SWFDisplayItem_getDepth(SWFDisplayItem item)
{
  return item->depth;
}

void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth)
{
  item->depth = depth;
}

void SWFDisplayItem_move(SWFDisplayItem item, float x, float y)
{
  SWFPosition_move(item->position, x, y);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_moveTo(SWFDisplayItem item, float x, float y)
{
  SWFPosition_moveTo(item->position, x, y);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_rotate(SWFDisplayItem item, float degrees)
{
  SWFPosition_rotate(item->position, degrees);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees)
{
  SWFPosition_rotateTo(item->position, degrees);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale)
{
  SWFPosition_scaleXY(item->position, xScale, yScale);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale)
{
  SWFPosition_scaleXYTo(item->position, xScale, yScale);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_skewX(SWFDisplayItem item, float x)
{
  SWFPosition_skewX(item->position, x);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_skewXTo(SWFDisplayItem item, float x)
{
  SWFPosition_skewXTo(item->position, x);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_skewY(SWFDisplayItem item, float y)
{
  SWFPosition_skewY(item->position, y);
  item->flags |= ITEM_TRANSFORMED;
}
void SWFDisplayItem_skewYTo(SWFDisplayItem item, float y)
{
  SWFPosition_skewYTo(item->position, y);
  item->flags |= ITEM_TRANSFORMED;
}

void SWFDisplayItem_setMatrix(SWFDisplayItem item, float a, float b,
			      float c, float d, float x, float y)
{
  SWFPosition_setMatrix(item->position, a, b, c, d, x, y);
  item->flags |= ITEM_TRANSFORMED;
}

void SWFDisplayItem_setName(SWFDisplayItem item, const char *name)
{
  if(item->name)
    free(item->name);

  item->name = strdup(name);
}
void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio)
{
  item->ratio = (int)rint(ratio*65535);
  item->flags |= ITEM_RATIOCHANGED;
}
void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform)
{
  if(item->cXform != NULL)
    destroySWFCXform(item->cXform);

  item->cXform = cXform;
  item->flags |= ITEM_CXFORMCHANGED;
}
void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a)
{
  if(item->cXform == NULL)
    item->cXform = newSWFAddCXform(r, g, b, a);
  else
    SWFCXform_setColorAdd(item->cXform, r, g, b, a);

  item->flags |= ITEM_CXFORMCHANGED;
}
void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a)
{
  if(item->cXform == NULL)
    item->cXform = newSWFMultCXform(r, g, b, a);
  else
    SWFCXform_setColorMult(item->cXform, r, g, b, a);

  item->flags |= ITEM_CXFORMCHANGED;
}

void SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSound sound)
{
  list->soundStream = sound;
}

void resolveDependencies(SWFCharacter character, SWFBlockList list)
{
  SWFBlock *deps;
  int i, nDeps = SWFCharacter_getNDependencies(character);

  if(nDeps > 0)
  {
    deps = SWFCharacter_getDependencies(character);

    for(i=0; i<nDeps; ++i)
    {
      if(!SWFBlock_isDefined(deps[i]))
      {
	if(SWFBlock_isCharacter(deps[i]))
	   resolveDependencies((SWFCharacter)deps[i], list);

	SWFBlockList_addBlock(list, (SWFBlock)deps[i]);
	SWFBlock_setDefined(deps[i]);
      }
    }
    SWFCharacter_clearDependencies(character);
  }
}

void SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist)
{
  SWFDisplayItem item = list->head, last = NULL, next;
  SWFBlock block, stream;
  SWFCharacter character;
  SWFCXform cXform;
  SWFMatrix matrix;

  if(list->soundStream)
  {
    stream = SWFSound_getStreamBlock(list->soundStream);

    if(stream)
      SWFBlockList_addBlock(blocklist, stream);
  }

  while(item!=NULL)
  {
    character = item->character;

    if(character != NULL)
      resolveDependencies(character, blocklist);

    if(item->flags & ITEM_REMOVED)
    {
      if(item == list->head)
	list->head = item->next;
      else
	last->next = item->next;

      if(item == list->tail)
	list->tail = last;

      SWFBlockList_addBlock(blocklist, newSWFRemoveObject2Block(item->depth));

      next = item->next;
      destroySWFDisplayItem(item);
      item = next;

      continue;
    }

    if(character != NULL && !list->isSprite)
      SWFBlockList_addBlock(blocklist, character);

    /* blank out info that hasn't changed */

    cXform = (item->flags & ITEM_CXFORMCHANGED) ? item->cXform : NULL;
    matrix = (item->flags & ITEM_TRANSFORMED) ? item->matrix : NULL;

    if(!(item->flags & ITEM_RATIOCHANGED) &&
       (character == NULL ||
	SWFBlock_getType(item->character) != SWF_DEFINEMORPHSHAPE))
      item->ratio = -1;

    if(!(item->flags & ITEM_NEW))
      character = NULL;

    if(item->flags != 0)
    {
      int move = (item->flags & ITEM_CHANGED) && !(item->flags&ITEM_NEW);

      block = newSWFPlaceObject2Block(item->depth, item->name, item->ratio,
				      cXform, matrix, character, move);
    }
    else
      block = NULL;

    /* after first round, displayitems are tracked by depth only */
    free(item->name);
    item->name = NULL;

    item->flags = 0;

    if(block)
      SWFBlockList_addBlock(blocklist, block);

    last = item;
    item = item->next;
  }
}
