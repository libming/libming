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

#ifndef SWF_OUTPUTBLOCK_H_INCLUDED
#define SWF_OUTPUTBLOCK_H_INCLUDED

#include "libswf.h"
#include "block.h"
#include "output.h"
#include "cxform.h"
#include "matrix.h"
#include "character.h"

struct _outputBlock
{
  swfBlock block;
  SWFOutput output;
};
typedef struct _outputBlock *SWFOutputBlock;

#define SWFOUTPUTBLOCK_SIZE sizeof(struct _outputBlock)

void destroySWFOutputBlock(SWFBlock block);

SWFOutputBlock newSWFOutputBlock(SWFOutput out, SWFBlocktype type);

#define SWF_PLACE_HAS_NAME        (1<<5)
#define SWF_PLACE_HAS_RATIO       (1<<4)
#define SWF_PLACE_HAS_CXFORM      (1<<3)
#define SWF_PLACE_HAS_MATRIX      (1<<2)
#define SWF_PLACE_HAS_CHARACTER   (1<<1)
#define SWF_PLACE_MOVE            (1<<0)

SWFOutputBlock newSWFPlaceObjectBlock(SWFCharacter character, int depth,
				      SWFMatrix matrix, SWFCXform cXform);
SWFOutputBlock newSWFPlaceObject2Block(int depth, char *name, int ratio,
				       SWFCXform cXform, SWFMatrix matrix,
				       SWFCharacter character, int move);

SWFOutputBlock newSWFSetBackgroundBlock(byte r, byte g, byte b);
SWFOutputBlock newSWFRemoveObjectBlock(SWFCharacter character, int depth);
SWFOutputBlock newSWFRemoveObject2Block(int depth);
SWFOutputBlock newSWFFrameLabelBlock(char *string);

#endif /* SWF_OUTPUTBLOCK_H_INCLUDED */
