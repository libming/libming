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
#include "movie.h"
#include "shape_util.h"

void destroySWFMovie(SWFMovie movie)
{
  destroySWFBlockList(movie->blockList);
  destroySWFDisplayList(movie->displayList);
  destroySWFRect(movie->bounds);
  free(movie);
}

extern int SWF_gNumCharacters;

SWFMovie newSWFMovieWithVersion(int version)
{
  SWFMovie movie = calloc(1, SWFMOVIE_SIZE);

  movie->version = version;
  movie->blockList = newSWFBlockList();
  movie->displayList = newSWFDisplayList();
  movie->bounds = newSWFRect(0, 320*20, 0, 240*20);
  movie->rate = 12.0;
  movie->totalFrames = 1;

  SWFMovie_addBlock(movie, newSWFSetBackgroundBlock(0xff,0xff,0xff));

  return movie;
}
SWFMovie newSWFMovie()
{
  return newSWFMovieWithVersion(4);
}

void SWFMovie_setRate(SWFMovie movie, float rate)
{
  movie->rate = rate;
}

extern float Ming_scale;

void SWFMovie_setDimension(SWFMovie movie, float width, float height)
{
  if(movie->bounds)
    free(movie->bounds);

  movie->bounds = newSWFRect(0, (int)rint(Ming_scale*width),
			     0, (int)rint(Ming_scale*height));
}

void SWFMovie_setNumberOfFrames(SWFMovie movie, int totalFrames)
{
  movie->totalFrames = totalFrames;
}

void SWFMovie_setBackground(SWFMovie movie, int r, int g, int b)
{
  destroySWFBlock(movie->blockList->blocks[0].block);
  movie->blockList->blocks[0].block = newSWFSetBackgroundBlock(r,g,b);
}

void SWFMovie_addBlock(SWFMovie movie, SWFBlock block)
{
  if(SWFBlock_getType(block) == SWF_SHOWFRAME)
    ++movie->nFrames;

  SWFBlockList_addBlock(movie->blockList, block);
}

SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block)
{
  /* if they're trying to add a raw bitmap, we'll be nice and turn
     it into a shape */

  if(SWFBlock_getType(block) == SWF_DEFINEBITS ||
     SWFBlock_getType(block) == SWF_DEFINEBITSJPEG2 ||
     SWFBlock_getType(block) == SWF_DEFINEBITSJPEG3 ||
     SWFBlock_getType(block) == SWF_DEFINELOSSLESS ||
     SWFBlock_getType(block) == SWF_DEFINELOSSLESS2)
  {
    SWFShape shape = newSWFShape();
    SWFFill fill = SWFShape_addBitmapFill(shape, block, SWFFILL_TILED_BITMAP);

    float width = SWFCharacter_getWidth((SWFCharacter)block);
    float height = SWFCharacter_getHeight((SWFCharacter)block);

    SWFShape_setRightFill(shape, fill);

    /* bitmap's bounds aren't scaled, so drawCharacterBounds won't work */
    SWFShape_drawLine(shape, Ming_scale*width, 0);
    SWFShape_drawLine(shape, 0, Ming_scale*height);
    SWFShape_drawLine(shape, -Ming_scale*width, 0);
    SWFShape_drawLine(shape, 0, -Ming_scale*height);

    block = (SWFBlock)shape;
  }

  if(SWFBlock_isCharacter(block))
  {
    return SWFDisplayList_add(movie->displayList, (SWFCharacter)block);
  }
  else
    SWFMovie_addBlock(movie, block);

  return NULL;
}

void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item)
{
  SWFDisplayItem_remove(item);
}

void SWFMovie_setSoundStream(SWFMovie movie, SWFSound sound)
{
  SWFBlock block = SWFSound_getStreamHead(sound, movie->rate);

  SWF_assert(block != NULL);

  SWFMovie_addBlock(movie, block);
  SWFDisplayList_setSoundStream(movie->displayList, sound);
}


void SWFMovie_nextFrame(SWFMovie movie)
{
  SWFDisplayList_writeBlocks(movie->displayList, movie->blockList);
  SWFMovie_addBlock(movie, newSWFShowFrameBlock());
}

void SWFMovie_labelFrame(SWFMovie movie, char *label)
{
  SWFMovie_addBlock(movie, newSWFFrameLabelBlock(label));
}

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data)
{
  int length;
  SWFOutput header;

  while(movie->nFrames < movie->totalFrames)
    SWFMovie_nextFrame(movie);

  SWFMovie_addBlock(movie, newSWFEndBlock());

  length = SWFBlockList_completeBlocks(movie->blockList);

  /* XXX - hack */
  if(movie->displayList->soundStream)
    SWFSound_rewind(movie->displayList->soundStream);

  header = newSizedSWFOutput(20);

  /* figure size, write header */

  SWFOutput_writeRect(header, movie->bounds);
  SWFOutput_writeUInt16(header, (int)floor(movie->rate*256));
  SWFOutput_writeUInt16(header, movie->nFrames);

  SWFOutput_byteAlign(header);
  length += 8 + SWFOutput_length(header);

  method('F', data);
  method('W', data);
  method('S', data);
  method(movie->version, data);
  methodWriteUInt32(length, method, data);
  SWFOutput_writeToMethod(header, method, data);

  destroySWFOutput(header);

  SWFBlockList_writeBlocksToMethod(movie->blockList, method, data);

  return length;
}
