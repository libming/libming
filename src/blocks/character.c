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

#include "character.h"
#include "blocktypes.h"

void SWFCharacter_addDependency(SWFCharacter character, SWFBlock dependency)
{
  character->dependencies = realloc(character->dependencies,
				    sizeof(SWFBlock) *
				    (character->nDependencies+1));

  character->dependencies[character->nDependencies] = dependency;
  ++character->nDependencies;
}

SWFBlock *SWFCharacter_getDependencies(SWFCharacter character)
{
  if(character->getDependencies != NULL)
    return character->getDependencies(character);
  else
    return character->dependencies;
}
int SWFCharacter_getNDependencies(SWFCharacter character)
{
  if(character->getNDependencies != NULL)
    return character->getNDependencies(character);
  else
    return character->nDependencies;
}
void SWFCharacter_clearDependencies(SWFCharacter character)
{
  character->nDependencies = 0;

  if(character->dependencies != NULL)
    free(character->dependencies);

  character->dependencies = NULL;
}

int SWFCharacter_getScaledWidth(SWFCharacter character)
{
  return character->bounds->maxX - character->bounds->minX;
}

int SWFCharacter_getScaledHeight(SWFCharacter character)
{
  return character->bounds->maxY - character->bounds->minY;
}


/* rather, should it go on the display list..
   character is a bit of a sloppy category now */

int SWFBlock_isCharacter(SWFBlock block)
{
  SWFBlocktype type = block->type;

  if(type == SWF_DEFINETEXT    || type == SWF_DEFINETEXT2      ||
     type == SWF_DEFINESHAPE   || type == SWF_DEFINESHAPE2     ||
     type == SWF_DEFINESHAPE3  || type == SWF_DEFINEMORPHSHAPE ||
     type == SWF_DEFINESPRITE  || type == SWF_DEFINEBUTTON     ||
     type == SWF_DEFINEBUTTON2 || type == SWF_DEFINETEXT2      ||
     type == SWF_DEFINEBITS ||
     type == SWF_DEFINEBITSJPEG2 ||
     type == SWF_DEFINEBITSJPEG3 ||
     type == SWF_DEFINELOSSLESS ||
     type == SWF_DEFINELOSSLESS2 ||
     type == SWF_DEFINEFONT ||
     type == SWF_TEXTFIELD)
    return TRUE;
  else
    return FALSE;
}
