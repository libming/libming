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
#include "blocks/sprite.h"

struct SWFMovieClip_s
{
  struct SWFSprite_s sprite;

  SWFBlockList blockList;
  SWFDisplayList displayList;
  unsigned short nFrames;
};


void destroySWFMovieClip(SWFMovieClip clip)
{
  destroySWFBlockList(clip->blockList);
  destroySWFDisplayList(clip->displayList);
  destroySWFSprite((SWFBlock)clip);
}


SWFMovieClip newSWFMovieClip()
{
  SWFMovieClip clip = (SWFMovieClip)newSWFSprite();
  clip = realloc(clip, sizeof(struct SWFMovieClip_s));

  clip->blockList = newSWFBlockList();
  clip->displayList = newSWFSpriteDisplayList();
  return clip;
}


void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int totalFrames)
{
  SWFSprite_setNumberOfFrames((SWFSprite)clip, totalFrames);
}


void SWFMovieClip_addBlock(SWFMovieClip movie, SWFBlock block)
{
  SWFBlockList_addBlock(movie->blockList, block);
}


void SWFMovieClip_setSoundStream(SWFMovieClip clip, SWFSound sound, float rate)
{
  SWFBlock block = SWFSound_getStreamHead(sound, rate);
  
  if(block != NULL)
  {
    SWFMovieClip_addBlock(clip, block);
    SWFDisplayList_setSoundStream(clip->displayList, sound);
  }
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
  SWFDisplayList_writeBlocks(clip->displayList, clip->blockList);
  SWFBlockList_addToSprite(clip->blockList, (SWFSprite)clip);
  SWFSprite_addBlock((SWFSprite)clip, newSWFShowFrameBlock());
}
