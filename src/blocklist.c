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
#include "blocklist.h"

void destroySWFBlockList(SWFBlockList list)
{
  int i;

  for(i=0; i<list->nBlocks; ++i)
  {
    /* characters were instantiated by the user, so user has to
       destroy them. */

    if(!list->blocks[i].isCharacter)
      destroySWFBlock(list->blocks[i].block);
  }

  free(list->blocks);
  free(list);
}

SWFBlockList newSWFBlockList()
{
  SWFBlockList blockList = calloc(1, SWFBLOCKLIST_SIZE);
  return blockList;
}

void SWFBlockList_addBlock(SWFBlockList list, SWFBlock block)
{
  if(SWFBlock_isDefined(block))
    return;

  if(list->nBlocks%SWFBLOCKLIST_INCREMENT == 0)
    list->blocks = realloc(list->blocks,
			   (list->nBlocks+SWFBLOCKLIST_INCREMENT) *
			   sizeof(swfBlockEntry));

  list->blocks[list->nBlocks].block = block;
  list->blocks[list->nBlocks].isCharacter = SWFBlock_isCharacter(block);
  list->nBlocks++;
  SWFBlock_setDefined(block);
}

int SWFBlockList_completeBlocks(SWFBlockList list)
{
  int i, total = 0;

  for(i=0; i<list->nBlocks; ++i)
    total += completeSWFBlock(list->blocks[i].block);

  return total;
}

int SWFBlockList_writeBlocksToMethod(SWFBlockList list,
				     SWFByteOutputMethod method, void *data)
{
  int i, size = 0;

  for(i=0; i<list->nBlocks; ++i)
    size += writeSWFBlockToMethod(list->blocks[i].block, method, data);

  return size;
}
