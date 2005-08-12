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

/* displaylist.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_DISPLAYLIST_H_INCLUDED
#define SWF_DISPLAYLIST_H_INCLUDED

#include "ming.h"
#include "blocklist.h"
#include "blocks/placeobject.h"


typedef struct SWFDisplayList_s *SWFDisplayList;

struct SWFDisplayItem_s
{
	SWFDisplayItem next;

	int flags;
	int depth;
	SWFPlaceObject2Block block;

	SWFCharacter character;
	SWFPosition position;
	SWFMatrix matrix;
	struct SWFDisplayList_s *list;
};


/* display item */

SWFCharacter SWFDisplayItem_getCharacter(SWFDisplayItem item);

void SWFDisplayItem_endMask(SWFDisplayItem item);

/*
 * Methods for reading position data
 *  - added by David McNab <david@rebirthing.co.nz>
 */

float SWFDisplayItem_get_x(SWFDisplayItem item);
float SWFDisplayItem_get_y(SWFDisplayItem item);
float SWFDisplayItem_get_xScale(SWFDisplayItem item);
float SWFDisplayItem_get_yScale(SWFDisplayItem item);
float SWFDisplayItem_get_xSkew(SWFDisplayItem item);
float SWFDisplayItem_get_ySkew(SWFDisplayItem item);
float SWFDisplayItem_get_rot(SWFDisplayItem item);


/* display list */

void destroySWFDisplayList(SWFDisplayList displayList);

SWFDisplayList newSWFDisplayList();

SWFDisplayList newSWFSpriteDisplayList();

void SWFDisplayList_nextFrame(SWFDisplayList list);

SWFDisplayItem SWFDisplayList_add(SWFDisplayList list, SWFCharacter shape);

void SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist);

void SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSoundStream stream);

void SWFDisplayList_rewindSoundStream(SWFDisplayList list);

#endif /* SWF_DISPLAYLIST_H_INCLUDED */
