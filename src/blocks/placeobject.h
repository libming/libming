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


#ifndef SWF_PLACEOBJECT_H_INCLUDED
#define SWF_PLACEOBJECT_H_INCLUDED

typedef struct SWFPlaceObject2Block_s *SWFPlaceObject2Block;

#include "libswf.h"
#include "character.h"
#include "matrix.h"
#include "cxform.h"
#include "action.h"

#define SWF_PLACE_HAS_ACTIONS     (1<<7)
#define SWF_PLACE_HAS_MASK        (1<<6)
#define SWF_PLACE_HAS_NAME        (1<<5)
#define SWF_PLACE_HAS_RATIO       (1<<4)
#define SWF_PLACE_HAS_CXFORM      (1<<3)
#define SWF_PLACE_HAS_MATRIX      (1<<2)
#define SWF_PLACE_HAS_CHARACTER   (1<<1)
#define SWF_PLACE_MOVE            (1<<0)

SWFPlaceObject2Block newSWFPlaceObject2Block (int depth);

void SWFPlaceObject2Block_setName (SWFPlaceObject2Block block,
				   const char *name);

void SWFPlaceObject2Block_setRatio (SWFPlaceObject2Block block, int ratio);

void SWFPlaceObject2Block_setDepth (SWFPlaceObject2Block block, int depth);

void SWFPlaceObject2Block_setMaskLevel (SWFPlaceObject2Block block,
					int masklevel);

void SWFPlaceObject2Block_setCXform (SWFPlaceObject2Block block,
				     SWFCXform cXform);

void SWFPlaceObject2Block_setColorAdd (SWFPlaceObject2Block block,
				       int r, int g, int b, int a);

void SWFPlaceObject2Block_setColorMult (SWFPlaceObject2Block block,
					float r, float g, float b, float a);

void SWFPlaceObject2Block_setMatrix (SWFPlaceObject2Block block,
				     SWFMatrix matrix);

void SWFPlaceObject2Block_setCharacter (SWFPlaceObject2Block block,
					SWFCharacter character);

void SWFPlaceObject2Block_setMove (SWFPlaceObject2Block block);

#define SWF_PLACEACTION_ONLOAD      (1<<0)
#define SWF_PLACEACTION_ENTERFRAME  (1<<1)
#define SWF_PLACEACTION_UNLOAD      (1<<2)
#define SWF_PLACEACTION_MOUSEMOVE   (1<<3)
#define SWF_PLACEACTION_MOUSEDOWN   (1<<4)
#define SWF_PLACEACTION_MOUSEUP     (1<<5)
#define SWF_PLACEACTION_KEYDOWN     (1<<6)
#define SWF_PLACEACTION_KEYUP       (1<<7)
#define SWF_PLACEACTION_DATA        (1<<8)

void SWFPlaceObject2Block_addAction (SWFPlaceObject2Block block,
				     SWFAction action, int flags);

#endif /* SWF_PLACEOBJECT_H_INCLUDED */
