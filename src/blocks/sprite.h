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

/* sprite.h */

#ifndef SWF_SPRITE_H_INCLUDED
#define SWF_SPRITE_H_INCLUDED

typedef struct SWFSprite_s *SWFSprite;

#include "character.h"


/* movie clip extends sprite, so we need a public definition */

struct SWFSprite_s
{
  struct SWFCharacter_s character;

  int frames;
  int totalFrames;

  int nBlocks;
  SWFBlock *blocks;
};


SWFSprite
newSWFSprite();

void
destroySWFSprite(SWFBlock block);

void
SWFSprite_setNumberOfFrames(SWFSprite sprite, int totalFrames);

void
SWFSprite_setBlocks(SWFSprite sprite, SWFBlock *blocks, int nBlocks);

void
SWFSprite_addBlock(SWFSprite sprite, SWFBlock block);

void
SWFSprite_getDependencies(SWFSprite sprite, SWFCharacter** deps, int* nDeps);

#endif /* SWF_SPRITE_H_INCLUDED */
