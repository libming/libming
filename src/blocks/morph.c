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

/* $Id$ */

#include "morph.h"

struct SWFMorph_s
{
  struct SWFCharacter_s character;

  SWFOutput out;
  SWFShape shape1;
  SWFShape shape2;
};


/* morph stuff */
static int completeSWFMorphBlock(SWFBlock block)
{
  SWFMorph morph = (SWFMorph)block;

  SWFOutput out = morph->out;
  SWFShape shape1 = morph->shape1;
  SWFShape shape2 = morph->shape2;

  byte *patch;
  int offset;

  SWFShape_end(shape1);
  SWFShape_end(shape2);

  /* build header */
  SWFOutput_writeUInt16(out, CHARACTERID(morph));
  SWFOutput_writeRect(out, CHARACTER(shape1)->bounds);
  SWFOutput_byteAlign(out);
  SWFOutput_writeRect(out, CHARACTER(shape2)->bounds);

  SWFOutput_byteAlign(out);
  patch = SWFOutput_getCurPos(out);
  SWFOutput_writeUInt32(out, 0); /* shape2 offset, patched in below.. */

  SWFOutput_writeMorphFillStyles(out,
				 SWFShape_getFills(shape1),
				 SWFShape_getNFills(shape1),
				 SWFShape_getFills(shape2),
				 SWFShape_getNFills(shape2));

  SWFOutput_writeMorphLineStyles(out,
				 SWFShape_getLines(shape1),
				 SWFShape_getNLines(shape1),
				 SWFShape_getLines(shape2),
				 SWFShape_getNLines(shape2));

  SWFOutput_byteAlign(out);

  offset =
    SWFOutput_getCurPos(out) - patch +
    SWFOutput_getLength(SWFShape_getOutput(shape1)) - 4;

  *(patch++) = offset & 0xff;
  offset >>= 8;
  *(patch++) = offset & 0xff;
  offset >>= 8;
  *(patch++) = offset & 0xff;
  offset >>= 8;
  *patch = offset & 0xff;

  return
    SWFOutput_getLength(out) +
    SWFOutput_getLength(SWFShape_getOutput(shape1)) +
    SWFOutput_getLength(SWFShape_getOutput(shape2));
}


void writeSWFMorphBlockToStream(SWFBlock block,
				SWFByteOutputMethod method, void *data)
{
  SWFMorph morph = (SWFMorph)block;

  SWFOutput_writeToMethod(morph->out, method, data);
  SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape1), method, data);
  SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape2), method, data);
}


void destroySWFMorph(SWFBlock block)
{
  SWFMorph morph = (SWFMorph)block;

  destroySWFOutput(morph->out);

  /* these must be destroyed by hand, since script wrappers can't
     tell the difference between these and newSWFShape()'s..

  destroySWFShape(morph->shape1);
  destroySWFShape(morph->shape2);
  */

  sec_free((void**)&morph);
}


SWFShape SWFMorph_getShape1(SWFMorph morph)
{
  return morph->shape1;
}


SWFShape SWFMorph_getShape2(SWFMorph morph)
{
  return morph->shape2;
}


int SWFMorph_getNDependencies(SWFCharacter character)
{
  SWFMorph morph = (SWFMorph)character;

  return SWFCharacter_getNDependencies(CHARACTER(morph->shape1));
}


SWFBlock *SWFMorph_getDependencies(SWFCharacter character)
{
  SWFMorph morph = (SWFMorph)character;
  return SWFCharacter_getDependencies(CHARACTER(morph->shape1));
}


SWFMorph newSWFMorphShape()
{
  SWFMorph morph = malloc(sizeof(struct SWFMorph_s));

  SWFCharacterInit((SWFCharacter)morph);

  BLOCK(morph)->type = SWF_DEFINEMORPHSHAPE;
  BLOCK(morph)->writeBlock = writeSWFMorphBlockToStream;
  BLOCK(morph)->complete = completeSWFMorphBlock;
  BLOCK(morph)->dtor = destroySWFMorph;

  CHARACTERID(morph) = ++SWF_gNumCharacters;

  morph->out = newSWFOutput();

  morph->shape1 = newSWFShape();
  BLOCK(morph->shape1)->type = 0;

  morph->shape2 = newSWFShape();
  BLOCK(morph->shape2)->type = 0;
  SWFShape_setMorphFlag(morph->shape2);

  morph->character.getDependencies = SWFMorph_getDependencies;
  morph->character.getNDependencies = SWFMorph_getNDependencies;

  return morph;
}
