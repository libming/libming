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

#include "sprite.h"

extern int SWF_gNumCharacters;

void writeSWFSpriteToMethod(SWFBlock block,
			    SWFByteOutputMethod method, void *data)
{
  int i;
  SWFSprite sprite = (SWFSprite)block;

  methodWriteUInt16(CHARACTERID(sprite), method, data);
  methodWriteUInt16(sprite->frames, method, data);

  for(i=0; i<sprite->nBlocks; ++i)
    writeSWFBlockToMethod(sprite->blocks[i], method, data);
}
int completeSWFSprite(SWFBlock block)
{
  int i, length = 0;
  SWFSprite sprite = (SWFSprite)block;

  SWFSprite_addBlock(sprite, newSWFEndBlock());

  for(i=0; i<sprite->nBlocks; ++i)
    length += completeSWFBlock(sprite->blocks[i]);

  return length+4;
}
void destroySWFSprite(SWFBlock block)
{
  SWFSprite sprite = (SWFSprite)block;
  int i;

  SWFCharacter_clearDependencies(CHARACTER(block));

  for(i=0; i<sprite->nBlocks; ++i)
  {
    /* characters are destroyed separately */

    if(!SWFBlock_isCharacter(sprite->blocks[i]))
      destroySWFBlock(sprite->blocks[i]);
  }

  free(sprite->blocks);
  free(sprite);
}

SWFSprite newSWFSprite()
{
  SWFSprite sprite = calloc(1, SWF_SPRITE_SIZE);

  CHARACTERID(sprite) = ++SWF_gNumCharacters;
  BLOCK(sprite)->type = SWF_DEFINESPRITE;
  BLOCK(sprite)->writeBlock = writeSWFSpriteToMethod;
  BLOCK(sprite)->complete = completeSWFSprite;
  BLOCK(sprite)->dtor = destroySWFSprite;

  sprite->blocks = NULL;

  return sprite;
}

void SWFSprite_addBlock(SWFSprite sprite, SWFBlock block)
{
  if(block->type == SWF_SHOWFRAME)
    ++sprite->frames;

  if(block->type == SWF_SHOWFRAME ||
     block->type == SWF_PLACEOBJECT ||
     block->type == SWF_PLACEOBJECT2 ||
     block->type == SWF_REMOVEOBJECT ||
     block->type == SWF_REMOVEOBJECT2 ||
     block->type == SWF_DOACTION ||
     block->type == SWF_STARTSOUND ||
     block->type == SWF_FRAMELABEL ||
     block->type == SWF_SOUNDSTREAMHEAD ||
     block->type == SWF_SOUNDSTREAMBLOCK ||
     block->type == SWF_END)
  {
    sprite->blocks = realloc(sprite->blocks,
			     (sprite->nBlocks+1)*sizeof(SWFBlock));

    sprite->blocks[sprite->nBlocks++] = block;

    /* XXX - can we get sprite's dependencies? */
  }
  else
    ; /* XXX - warn */
}
