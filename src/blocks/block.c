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

#include <math.h>

#include "block.h"
#include "method.h"

int SWF_gNumCharacters;

void SWFBlock_setDefined(SWFBlock block)
{
  block->isDefined = TRUE;
}

byte SWFBlock_isDefined(SWFBlock block)
{
  return block->isDefined;
}


void destroySWFBlock(SWFBlock block)
{
  if(block->dtor)
    block->dtor(block);
  else
    free(block);
}

int completeSWFBlock(SWFBlock block)
{
  if(!block->completed)
  {
    if(block->complete)
      block->length = block->complete(block);

    block->completed = TRUE;
  }

  if(block->length>62 ||
     block->type == SWF_DEFINELOSSLESS ||
     block->type == SWF_DEFINELOSSLESS2)
    return block->length + 6;
  else
    return block->length + 2;
}

int writeSWFBlockToMethod(SWFBlock block,
			  SWFByteOutputMethod method, void *data)
{
  if(!block->completed)
    completeSWFBlock(block);

  /* write header */

  if(block->length>62 ||
     block->type == SWF_DEFINELOSSLESS ||
     block->type == SWF_DEFINELOSSLESS2)
  {
    /* yep, a definebitslossless block has to be long form, even if it's
       under 63 bytes.  how stupid is that? */

    method(((block->type&0x03)<<6) + 0x3f, data);
    method((block->type >> 2)&0xff, data);
    methodWriteUInt32(block->length, method, data);
  }
  else
    methodWriteUInt16(block->length + ((block->type)<<6), method, data);

  if(block->writeBlock)
    block->writeBlock(block, method, data);

  return block->length + ((block->length>62)?6:2);
}

SWFBlock newEmptySWFBlock(SWFBlocktype type)
{
  SWFBlock block = calloc(1, BLOCK_SIZE);
  block->type = type;

  block->writeBlock = NULL;
  block->complete = NULL;
  block->dtor = NULL;

  return block;
}

SWFBlock newSWFShowFrameBlock()
{
  return newEmptySWFBlock(SWF_SHOWFRAME);
}
SWFBlock newSWFEndBlock()
{
  return newEmptySWFBlock(SWF_END);
}
SWFBlock newSWFProtectBlock()
{
  return newEmptySWFBlock(SWF_PROTECT);
}

SWFBlocktype SWFBlock_getType(SWFBlock block)
{
  return block->type;
}
