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

#ifndef MING_DISPLAYLIST_H_INCLUDED
#define MING_DISPLAYLIST_H_INCLUDED

#include "libming.h"
#include "blocks/swf.h"

#include "blocklist.h"
#include "position.h"

#define ITEM_NEW            (1<<0)
#define ITEM_REMOVED        (1<<1)
#define ITEM_TRANSFORMED    (1<<2)
#define ITEM_CXFORMCHANGED  (1<<3)
#define ITEM_RATIOCHANGED   (1<<4)

#define ITEM_CHANGED  (ITEM_TRANSFORMED|ITEM_CXFORMCHANGED|ITEM_RATIOCHANGED)

struct _swfDisplayItem
{
  int flags;
  struct _swfDisplayItem *next;
  int depth;
  SWFPosition position;
  SWFMatrix matrix;
  SWFCharacter character;
  int ratio;
  SWFCXform cXform;
  char *name;
};
typedef struct _swfDisplayItem *SWFDisplayItem;

#define SWFDISPLAYITEM_SIZE sizeof(struct _swfDisplayItem)

void SWFDisplayItem_rotate(SWFDisplayItem item, float degrees);
void SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees);
void SWFDisplayItem_move(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_moveTo(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_skewX(SWFDisplayItem item, float x);
void SWFDisplayItem_skewXTo(SWFDisplayItem item, float x);
void SWFDisplayItem_skewY(SWFDisplayItem item, float y);
void SWFDisplayItem_skewYTo(SWFDisplayItem item, float y);
void SWFDisplayItem_setMatrix(SWFDisplayItem i, float a, float b,
			      float c, float d, float x, float y);

int SWFDisplayItem_getDepth(SWFDisplayItem item);
void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth);
void SWFDisplayItem_remove(SWFDisplayItem item);
void SWFDisplayItem_setName(SWFDisplayItem item, const char *name);
void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio);
void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform);
void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a);
void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a);

#define SWFDisplayItem_addColor SWFDisplayItem_setColorAdd
#define SWFDisplayItem_multColor SWFDisplayItem_setColorMult

struct _swfDisplayList
{
  SWFSound soundStream;
  SWFDisplayItem head;
  SWFDisplayItem tail;
  byte isSprite;
  int depth;
};
typedef struct _swfDisplayList *SWFDisplayList;

#define SWFDISPLAYLIST_SIZE sizeof(struct _swfDisplayList)

void destroySWFDisplayList(SWFDisplayList list);
SWFDisplayList newSWFDisplayList();
SWFDisplayList newSWFSpriteDisplayList();
void SWFDisplayList_nextFrame(SWFDisplayList list);

SWFDisplayItem SWFDisplayList_add(SWFDisplayList list, SWFCharacter shape);
void SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist);

void SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSound sound);

#endif /* MING_DISPLAYLIST_H_INCLUDED */
