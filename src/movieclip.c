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
#include <math.h>
#include "movieclip.h"

void destroySWFMovieClip(SWFMovieClip clip)
{
  destroySWFBlockList(clip->blockList);
  destroySWFDisplayList(clip->displayList);
  destroySWFSprite((SWFBlock)clip);
}
SWFMovieClip newSWFMovieClip()
{
  SWFMovieClip clip = (SWFMovieClip)newSWFSprite();
  clip = realloc(clip, SWFMOVIECLIP_SIZE);

  clip->blockList = newSWFBlockList();
  clip->displayList = newSWFSpriteDisplayList();
  clip->totalFrames = 1;
  return clip;
}
void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int totalFrames)
{
  clip->totalFrames = totalFrames;
}

SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block)
{
  if(SWFBlock_isCharacter(block))
  {
    /* movie clip aquires dependencies from character */
    SWFBlock *deps;
    int i, nDeps = SWFCharacter_getNDependencies((SWFCharacter)block);

    if(nDeps > 0)
    {
      deps = SWFCharacter_getDependencies((SWFCharacter)block);

      for(i=0; i<nDeps; ++i)
	SWFCharacter_addDependency((SWFCharacter)clip, deps[i]);

      SWFCharacter_clearDependencies((SWFCharacter)block);
    }

    SWFCharacter_addDependency((SWFCharacter)clip, block);
    return SWFDisplayList_add(clip->displayList, (SWFCharacter)block);
  }
  else
    /* XXX - make sure it's a legit block for a sprite */
    SWFBlockList_addBlock(clip->blockList, block);

  return NULL;
}

void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item)
{
  SWFDisplayItem_remove(item);
}

void SWFMovieClip_labelFrame(SWFMovieClip clip, char *label)
{
  SWFSprite_addBlock((SWFSprite)clip, (SWFBlock)newSWFFrameLabelBlock(label));
}

void SWFMovieClip_nextFrame(SWFMovieClip clip)
{
  int i;

  SWFDisplayList_writeBlocks(clip->displayList, clip->blockList);

  for(i=0; i<clip->blockList->nBlocks; ++i)
    SWFSprite_addBlock((SWFSprite)clip, clip->blockList->blocks[i].block);

  SWFSprite_addBlock((SWFSprite)clip, newSWFShowFrameBlock());

  clip->blockList->nBlocks = 0;
}
