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

/* morph.h */

#ifndef SWF_MORPH_H_INCLUDED
#define SWF_MORPH_H_INCLUDED

#include "character.h"
#include "output.h"
#include "shape.h"

struct _morph
{
  swfCharacter character;
  SWFOutput out;
  SWFShape shape1;
  SWFShape shape2;
};
typedef struct _morph *SWFMorph;

#define MORPH_SIZE sizeof(struct _morph)

/* morph stuff */
SWFShape SWFMorph_getShape1(SWFMorph morph);
SWFShape SWFMorph_getShape2(SWFMorph morph);
SWFMorph newSWFMorphShape();

#endif /* SWF_MORPH_H_INCLUDED */
