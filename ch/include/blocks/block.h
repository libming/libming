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

/* block.h
 *
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_BLOCK_H_INCLUDED
#define SWF_BLOCK_H_INCLUDED

#include "ming.h"
#include "blocktypes.h"
#include "libming.h"

#define BLOCK(b) ((SWFBlock)(b))

typedef void (*writeSWFBlockMethod)(SWFBlock block,
				    SWFByteOutputMethod method, void *data);
typedef int (*completeSWFBlockMethod)(SWFBlock block);

typedef void (*destroySWFBlockMethod)(SWFBlock block);


struct SWFBlock_s
{
  SWFBlocktype type;

  writeSWFBlockMethod writeBlock;
  completeSWFBlockMethod complete;
  destroySWFBlockMethod dtor;

  int length;
  byte isDefined;
  byte completed;
};


/* sets the defined flag in this block */
void SWFBlock_setDefined(SWFBlock block);

/* returns true if the block has been defined- i.e., written to the file */
BOOL SWFBlock_isDefined(SWFBlock block);

/* returns the byte length of this block */
int SWFBlock_getLength(SWFBlock block);

/* destroy this block by calling its destructor */
void destroySWFBlock(SWFBlock block);

/* calls the block's completion method and returns its byte length */
int completeSWFBlock(SWFBlock block);

/* writes this block to the given output method */
int writeSWFBlockToMethod(SWFBlock block, SWFByteOutputMethod method, 
                          void *data);

/* initialize character values to something sane */
void SWFBlockInit(SWFBlock block);

/* return a new ShowFrame block */
SWFBlock newSWFShowFrameBlock();

/* return a new End block */
SWFBlock newSWFEndBlock();

/* return a new Protect block */
SWFBlock newSWFProtectBlock();

/* return a new empty block */
SWFBlock newEmptySWFBlock();

/* return the block's type */
SWFBlocktype SWFBlock_getType(SWFBlock block);

#endif /* SWF_BLOCK_H_INCLUDED */
