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

typedef struct SWFDisplayItem_s *SWFDisplayItem;
typedef struct SWFDisplayList_s *SWFDisplayList;

#include "libming.h"

#include "blocklist.h"
#include "position.h"

#include "blocks/cxform.h"
#include "blocks/action.h"
#include "blocks/character.h"
#include "blocks/sound.h"
#include "blocks/soundstream.h"


/* display item */

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

SWFCharacter SWFDisplayItem_getCharacter(SWFDisplayItem item);

int SWFDisplayItem_getDepth(SWFDisplayItem item);

void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth);

void SWFDisplayItem_remove(SWFDisplayItem item);

void SWFDisplayItem_setName(SWFDisplayItem item, const char *name);

void SWFDisplayItem_setMaskLevel(SWFDisplayItem item, int masklevel);

void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio);

void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform);

void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a);

void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a);

void SWFDisplayItem_addAction(SWFDisplayItem item,
			      SWFAction action, int flags);

void SWFDisplayItem_getPosition(SWFDisplayItem item, float * x, 
                                float * y);

void SWFDisplayItem_getRotation(SWFDisplayItem item, 
                                float * degrees);

void SWFDisplayItem_getScale(SWFDisplayItem item, float * xScale, 
                             float * yScale);

void SWFDisplayItem_getSkew(SWFDisplayItem item, float * xSkew, 
                            float * ySkew);


/* display list */

void destroySWFDisplayList(SWFDisplayList list);

SWFDisplayList newSWFDisplayList();

SWFDisplayList newSWFSpriteDisplayList();

void SWFDisplayList_nextFrame(SWFDisplayList list);

SWFDisplayItem SWFDisplayList_add(SWFDisplayList list, SWFCharacter shape);

void SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist);

void SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSoundStream stream);

void SWFDisplayList_rewindSoundStream(SWFDisplayList list);

#endif /* MING_DISPLAYLIST_H_INCLUDED */
