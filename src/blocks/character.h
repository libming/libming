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

#include "libswf.h"
#include "block.h"
#include "rect.h"

extern int SWF_gNumCharacters;


/* fudge superclass for character types (shape, text, sprite) */
struct _character
{
  swfBlock block;
  unsigned short number;
  SWFRect bounds;

  /* more bad hackery - allows objects to declare other characters as
     dependencies; e.g., shape needs a jpeg or text needs a font.. */

  int nDependencies;
  SWFBlock *dependencies;

  /* morph needs to acquire dependencies from its shapes.
     this is weird, but the easiest way I can think of to do this. */

  SWFBlock *(*getDependencies)(struct _character *character);
  int (*getNDependencies)(struct _character *character);
};
typedef struct _character swfCharacter;
typedef struct _character *SWFCharacter;


void SWFCharacter_addDependency(SWFCharacter character, SWFBlock dependency);
SWFBlock *SWFCharacter_getDependencies(SWFCharacter character);
int SWFCharacter_getNDependencies(SWFCharacter character);
void SWFCharacter_clearDependencies(SWFCharacter character);

#define CHARACTER(s) ((SWFCharacter)(s))
#define CHARACTERID(s) (((SWFCharacter)(s))->number)

int SWFCharacter_getScaledWidth(SWFCharacter character);
int SWFCharacter_getScaledHeight(SWFCharacter character);

int SWFBlock_isCharacter(SWFBlock block);

#endif /* SWF_CHARACTER_H_INCLUDED */
