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

SWFOutputBlock newSWFPlaceObjectBlock(SWFCharacter character, int depth,
				      SWFMatrix matrix, SWFCXform cXform)
{
  SWFOutput out = newSizedSWFOutput(40);

  SWFOutput_writeUInt16(out, character->number);
  SWFOutput_writeUInt16(out, depth);
  SWFOutput_writeMatrix(out, matrix); /* max length - 210 bits=27 bytes */

  if(cXform)
    SWFOutput_writeCXform(out, cXform, SWF_PLACEOBJECT); /* max length - 9 bytes */

  return newSWFOutputBlock(out, SWF_PLACEOBJECT);
}

SWFOutputBlock newSWFPlaceObject2Block(int depth, char *name, int ratio,
				       SWFCXform cXform, SWFMatrix matrix,
				       SWFCharacter character, int move)
{
  int flags = 0;
  SWFOutput out = newSizedSWFOutput(60);

  if(name)
    flags |= SWF_PLACE_HAS_NAME;
  if(ratio != -1)
    flags |= SWF_PLACE_HAS_RATIO;
  if(cXform)
    flags |= SWF_PLACE_HAS_CXFORM;
  if(matrix)
    flags |= SWF_PLACE_HAS_MATRIX;
  if(character)
    flags |= SWF_PLACE_HAS_CHARACTER;
  if(move)
    flags |= SWF_PLACE_MOVE;

  SWFOutput_writeUInt8(out, flags);

  SWFOutput_writeUInt16(out, depth);

  if(character)
    SWFOutput_writeUInt16(out, character->number);

  if(matrix)
    SWFOutput_writeMatrix(out, matrix);

  if(cXform)
    SWFOutput_writeCXform(out, cXform, SWF_PLACEOBJECT2);

  if(ratio != -1)
    SWFOutput_writeUInt16(out, ratio);

  if(name)
    SWFOutput_writeString(out, name);

  return newSWFOutputBlock(out, SWF_PLACEOBJECT2);
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
