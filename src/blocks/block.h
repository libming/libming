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

#ifndef SWF_BLOCK_H_INCLUDED
#define SWF_BLOCK_H_INCLUDED

#include "libswf.h"
#include "method.h"
#include "blocktypes.h"

struct _block;

typedef void (*writeSWFBlockMethod)(struct _block *block,
				    SWFByteOutputMethod method, void *data);
typedef int (*completeSWFBlockMethod)(struct _block *block);
typedef void (*destroySWFBlockMethod)(struct _block *block);

struct _block
{
  SWFBlocktype type;
  writeSWFBlockMethod writeBlock;
  completeSWFBlockMethod complete;
  destroySWFBlockMethod dtor;
  int length;
  byte isDefined;
  byte completed;
};
typedef struct _block swfBlock;
typedef struct _block *SWFBlock;

#define BLOCK_SIZE sizeof(struct _block)

void SWFBlock_setDefined(SWFBlock block);
byte SWFBlock_isDefined(SWFBlock block);

void destroySWFBlock(SWFBlock block);
int completeSWFBlock(SWFBlock block);
int writeSWFBlockToMethod(SWFBlock block,
			  SWFByteOutputMethod method, void *data);

#define BLOCK(s) ((SWFBlock)(s))

SWFBlock newSWFShowFrameBlock();
SWFBlock newSWFEndBlock();
SWFBlock newSWFProtectBlock();
SWFBlock newEmptySWFBlock();

SWFBlocktype SWFBlock_getType(SWFBlock block);

#endif /* SWF_BLOCK_H_INCLUDED */
