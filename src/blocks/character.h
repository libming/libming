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

#ifndef SWF_CHARACTER_H_INCLUDED
#define SWF_CHARACTER_H_INCLUDED

typedef struct SWFCharacter_s *SWFCharacter;

#include "libswf.h"
#include "block.h"
#include "rect.h"

extern int SWF_gNumCharacters;


#define CHARACTER(c) ((SWFCharacter)(c))
#define CHARACTERID(c) (((SWFCharacter)(c))->id)


/* characters are things that can be placed on the stage-
   shapes, text, sprites.. */

struct SWFCharacter_s
{
  struct SWFBlock_s block;

  int id;
  SWFRect bounds;

  /* some characters depend on others- e.g., a shape may need a jpeg for
     a fill or some text needs a font..  we keep track of these dependencies
     so that we write them in the right order, regardless of when/if the user
     adds them to the movie */

  int nDependencies;
  SWFBlock *dependencies;

  /* a morph needs to acquire dependencies from its shapes.
     this is weird, but the easiest way I can think of to do this. */

  SWFBlock *(*getDependencies)(SWFCharacter character);
  int (*getNDependencies)(SWFCharacter character);
};


/* initialize character values to something sane */

void SWFCharacterInit(SWFCharacter character);


/* destroy this character */

void destroySWFCharacter(SWFBlock character);


/* marks the given block as a dependency of the character- that is,
   we should define the block before defining the character */

void SWFCharacter_addDependency(SWFCharacter character, SWFBlock dependency);


/* returns the character's dependency list */

SWFBlock *SWFCharacter_getDependencies(SWFCharacter character);


/* returns the size of the character's dependency list */

int SWFCharacter_getNDependencies(SWFCharacter character);


/* clears the character's dependecy list */

void SWFCharacter_clearDependencies(SWFCharacter character);


/* returns the scaled (i.e., in twips) width of the character */

int SWFCharacter_getScaledWidth(SWFCharacter character);


/* returns the scaled (i.e., in twips) height of the character */

int SWFCharacter_getScaledHeight(SWFCharacter character);


/* returns the character's SWFRect bounds object */

SWFRect SWFCharacter_getBounds(SWFCharacter character);


/* returns non-zero if the given block is a character type */

int SWFBlock_isCharacter(SWFBlock block);


#endif /* SWF_CHARACTER_H_INCLUDED */
