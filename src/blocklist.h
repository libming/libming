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

#ifndef MING_BLOCKLIST_H_INCLUDED
#define MING_BLOCKLIST_H_INCLUDED

#include "libming.h"
#include "blocks/swf.h"

/* we have to note which blocks are characters b/c characters may be destroyed
   before we have a chance to look at them */

struct _swfBlockListEntry
{
  SWFBlock block;
  byte isCharacter;
};
typedef struct _swfBlockListEntry swfBlockEntry;

struct _swfBlockList
{
  swfBlockEntry *blocks;
  int nBlocks;
};
typedef struct _swfBlockList *SWFBlockList;

#define SWFBLOCKLIST_SIZE sizeof(struct _swfBlockList)

#define SWFBLOCKLIST_INCREMENT 16

SWFBlockList newSWFBlockList();
void SWFBlockList_addBlock(SWFBlockList blocklist, SWFBlock block);

int SWFBlockList_completeBlocks(SWFBlockList list);
int SWFBlockList_writeBlocksToMethod(SWFBlockList list,
				     SWFByteOutputMethod method, void *data);

void destroySWFBlockList(SWFBlockList list);

#endif /* MING_BLOCKLIST_H_INCLUDED */
