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

#include "character.h"
#include "blocktypes.h"


void
SWFCharacterInit (SWFCharacter character)
{
  SWFBlockInit ((SWFBlock) character);

  character->id = 0;
  character->bounds = NULL;

  character->dependencies = NULL;
  character->nDependencies = 0;
  character->getDependencies = NULL;
}


void
destroySWFCharacter (SWFCharacter character)
{
  SWFCharacter_clearDependencies (character);

  if (character->bounds != NULL)
    destroySWFRect (character->bounds);

  destroySWFBlock ((SWFBlock) character);
}


void
SWFCharacter_addDependency (SWFCharacter character, SWFBlock dependency)
{
  character->dependencies = realloc (character->dependencies,
				     sizeof (SWFBlock) *
				     (character->nDependencies + 1));

  character->dependencies[character->nDependencies] = dependency;
  ++character->nDependencies;
}


void
SWFCharacter_setID (SWFCharacter character, int id)
{
  character->id = id;
}


int
SWFCharacter_getID (SWFCharacter character)
{
  return character->id;
}


int
SWFCharacter_getDependencies (SWFCharacter character, SWFBlock ** outBlocks)
{
  if (character->getDependencies == NULL)
  {
    *outBlocks = character->dependencies;
    return character->nDependencies;
  }
  else
    return character->getDependencies (character, outBlocks);
}


void
SWFCharacter_clearDependencies (SWFCharacter character)
{
  character->nDependencies = 0;

  if (character->dependencies != NULL)
    sec_free ((void **) &character->dependencies);

}


int
SWFCharacter_getScaledWidth (SWFCharacter character)
{
  return SWFRect_getWidth (character->bounds);
}


int
SWFCharacter_getScaledHeight (SWFCharacter character)
{
  return SWFRect_getHeight (character->bounds);
}


SWFRect
SWFCharacter_getBounds (SWFCharacter character)
{
  return character->bounds;
}


/* rather, should it go on the display list..
   character is a bit of a sloppy category now */

int
SWFBlock_isCharacter (SWFBlock block)
{
  SWFBlocktype type = block->type;

  if (type == SWF_DEFINETEXT || type == SWF_DEFINETEXT2 ||
      type == SWF_DEFINESHAPE || type == SWF_DEFINESHAPE2 ||
      type == SWF_DEFINESHAPE3 || type == SWF_DEFINEMORPHSHAPE ||
      type == SWF_DEFINESPRITE || type == SWF_DEFINEBUTTON ||
      type == SWF_DEFINEBUTTON2 || type == SWF_DEFINETEXT2 ||
      type == SWF_DEFINEBITS ||
      type == SWF_DEFINEBITSJPEG2 ||
      type == SWF_DEFINEBITSJPEG3 ||
      type == SWF_DEFINELOSSLESS ||
      type == SWF_DEFINELOSSLESS2 ||
      type == SWF_DEFINEFONT || type == SWF_DEFINEFONT2 ||
      type == SWF_DEFINEEDITTEXT)
    return TRUE;
  else
    return FALSE;
}
