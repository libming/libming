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

#include <stdio.h>

#include "outputblock.h"
#include "method.h"

void writeSWFOutputBlockToStream(SWFBlock block,
				 SWFByteOutputMethod method, void *data)
{
  SWFOutput out = ((SWFOutputBlock)block)->output;
  SWFOutput_writeToMethod(out, method, data);
}
int getSWFOutputBlockLength(SWFBlock block)
{
  SWFOutput out = ((SWFOutputBlock)block)->output;

  SWFOutput_byteAlign(out);
  return SWFOutput_length(out);
}
void destroySWFOutputBlock(SWFBlock block)
{
  SWFOutputBlock b = (SWFOutputBlock)block;
  destroySWFOutput(b->output);
  free(block);
}
SWFOutputBlock newSWFOutputBlock(SWFOutput out, SWFBlocktype type)
{
  SWFOutputBlock block = calloc(1, SWFOUTPUTBLOCK_SIZE);

  BLOCK(block)->type = type;
  BLOCK(block)->writeBlock = writeSWFOutputBlockToStream;
  BLOCK(block)->complete = getSWFOutputBlockLength;
  BLOCK(block)->dtor = destroySWFOutputBlock;
  block->output = out;

  return block;
}

/* placeObject2 block is in placeobject.c */

SWFOutputBlock newSWFPlaceObjectBlock(SWFCharacter character, int depth,
				      SWFMatrix matrix, SWFCXform cXform)
{
  SWFOutput out = newSizedSWFOutput(40);

  SWFOutput_writeUInt16(out, character->number);
  SWFOutput_writeUInt16(out, depth);
  SWFOutput_writeMatrix(out, matrix); /* max length: 210 bits=27 bytes */

  if(cXform)
    /* max length: 9 bytes */
    SWFOutput_writeCXform(out, cXform, SWF_PLACEOBJECT);

  return newSWFOutputBlock(out, SWF_PLACEOBJECT);
}

SWFOutputBlock newSWFSetBackgroundBlock(byte r, byte g, byte b)
{
  SWFOutput out = newSizedSWFOutput(4);
  SWFOutput_writeUInt8(out, r);
  SWFOutput_writeUInt8(out, g);
  SWFOutput_writeUInt8(out, b);

  return newSWFOutputBlock(out, SWF_SETBACKGROUNDCOLOR);
}

SWFOutputBlock newSWFRemoveObjectBlock(SWFCharacter character, int depth)
{
  SWFOutput out = newSizedSWFOutput(5);
  SWFOutput_writeUInt16(out, character->number);
  SWFOutput_writeUInt16(out, depth);

  return newSWFOutputBlock(out, SWF_REMOVEOBJECT);
}

SWFOutputBlock newSWFRemoveObject2Block(int depth)
{
  SWFOutput out = newSizedSWFOutput(3);
  SWFOutput_writeUInt16(out, depth);

  return newSWFOutputBlock(out, SWF_REMOVEOBJECT2);
}

SWFOutputBlock newSWFFrameLabelBlock(char *string)
{
  SWFOutput out = newSizedSWFOutput(strlen(string)+2);
  SWFOutput_writeString(out, string);

  return newSWFOutputBlock(out, SWF_FRAMELABEL);
}
