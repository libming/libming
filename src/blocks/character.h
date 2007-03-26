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

/* character.h
 *
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_CHARACTER_H_INCLUDED
#define SWF_CHARACTER_H_INCLUDED

#include "ming.h"
#include "block.h"
#include "rect.h"
#include "../displaylist.h"

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
  SWFCharacter *dependencies;

  /* We shouldn't allow users to modify characters after they've been added
     to a container- movie, sprite, or button. */

  BOOL isFinished;
	 
  /* will be called if a character is placed first
   * character can add additional blocks to the movies blocklist
   * all blocks are added _before_ the place tag
   * onPlace ist called only for the first place event
   * returns the number of added blocks
   */
  int (*onPlace)(SWFDisplayItem, SWFBlockList);

  /* will be called for each frame
   * returns the number of blocks added
   */
  int (*onFrame)(SWFDisplayItem, SWFBlockList);
};


/* initialize character values to something sane */

void SWFCharacterInit(SWFCharacter character);


/* destroy this character */

void destroySWFCharacter(SWFCharacter character);


/* marks the given block as a dependency of the character- that is,
   we should define the block before defining the character */

void
SWFCharacter_addDependency(SWFCharacter character, SWFCharacter dependency);


/* returns the character's dependency list */

BOOL
SWFCharacter_getDependencies(SWFCharacter character,
			     SWFCharacter** depsPtr, int* nDepsPtr);


/* returns the scaled (i.e., in twips) width of the character */

int
SWFCharacter_getScaledWidth(SWFCharacter character);


/* returns the scaled (i.e., in twips) height of the character */

int
SWFCharacter_getScaledHeight(SWFCharacter character);


/* returns the character's SWFRect bounds object */

SWFRect
SWFCharacter_getBounds(SWFCharacter character);


/* returns TRUE if the given block is a character type, else FALSE */

BOOL
SWFBlock_isCharacter(SWFBlock block);


/* notify the character that it's been added to a container */

void
SWFCharacter_setFinished(SWFCharacter character);


/* returns TRUE if the character has been added to a container */

BOOL
SWFCharacter_isFinished(SWFCharacter character);

#endif /* SWF_CHARACTER_H_INCLUDED */
