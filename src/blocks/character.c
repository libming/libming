/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

#include <stdlib.h>

#include "character.h"
#include "blocktypes.h"
#include "morph.h"
#include "libming.h"


int SWF_gNumCharacters;


void
SWFCharacterInit(SWFCharacter character)
{
	SWFBlockInit((SWFBlock)character);

	character->id = 0;
	character->bounds = NULL;

	character->dependencies = NULL;
	character->nDependencies = 0;
	
	character->isFinished = FALSE;

	character->onPlace = NULL;
	character->onFrame = NULL;
}


void
destroySWFCharacter(SWFCharacter character)
{
	if ( character->dependencies != NULL )
		free(character->dependencies);

	if ( character->bounds != NULL )
		destroySWFRect(character->bounds);

	// destroySWFBlock((SWFBlock)character);

	free(character);
}


void
SWFCharacter_addDependency(SWFCharacter character, SWFCharacter dependency)
{
#if 1
	int n;
	for(n = 0 ; n < character->nDependencies ; n++)
		if(character->dependencies[n] == dependency)
			return;
#endif
	character->dependencies =
		(SWFCharacter*)realloc(character->dependencies,
						sizeof(SWFCharacter) * (character->nDependencies + 1));

	character->dependencies[character->nDependencies] = dependency;
	++character->nDependencies;
}


void SWFCharacter_setID(SWFCharacter character, int id)
{
	character->id = id;
}


int SWFCharacter_getID(SWFCharacter character)
{
	return character->id;
}


BOOL
SWFCharacter_getDependencies(SWFCharacter character,
														 SWFCharacter** depsPtr, int* nDepsPtr)
{
	int i;
	int nDeps = *nDepsPtr;
	SWFCharacter* deps = *depsPtr;

	if ( BLOCK(character)->type == SWF_DEFINEMORPHSHAPE )
		character = (SWFCharacter)SWFMorph_getShape1((SWFMorph)character);

	for ( i = 0; i < character->nDependencies; ++i )
	{
		SWFCharacter c = character->dependencies[i];

		if ( !SWFBlock_isDefined((SWFBlock)c) )
		{
			deps = (SWFCharacter*) realloc(deps, sizeof(SWFCharacter) * (nDeps + 1));
			deps[nDeps] = c;
			++nDeps;
		}
	}

	if ( nDeps == *nDepsPtr )
		return FALSE;

	*depsPtr = deps;
	*nDepsPtr = nDeps;

	return TRUE;
}


int
SWFCharacter_getScaledWidth(SWFCharacter character)
{
	return SWFRect_getWidth(character->bounds);
}


int
SWFCharacter_getScaledHeight(SWFCharacter character)
{
	return SWFRect_getHeight(character->bounds);
}


SWFRect
SWFCharacter_getBounds(SWFCharacter character)
{
	return character->bounds;
}


/* rather, should it go on the display list..
	 "character" is a bit of a sloppy category now */

BOOL
SWFBlock_isCharacter(SWFBlock block)
{
	SWFBlocktype type = block->type;

	if ( type == SWF_DEFINETEXT		 || type == SWF_DEFINETEXT2			 ||
			 type == SWF_DEFINESHAPE	 || type == SWF_DEFINESHAPE2		 ||
			 type == SWF_DEFINESHAPE3	 || type == SWF_DEFINESHAPE4 ||
			 type == SWF_DEFINEMORPHSHAPE ||
			 type == SWF_DEFINESPRITE	 || type == SWF_DEFINEBUTTON		 ||
			 type == SWF_DEFINEBUTTON2 || type == SWF_DEFINETEXT2			 ||
			 type == SWF_DEFINEBITS ||
			 type == SWF_DEFINEBITSJPEG2 ||
			 type == SWF_DEFINEBITSJPEG3 ||
			 type == SWF_DEFINELOSSLESS ||
			 type == SWF_DEFINELOSSLESS2 ||
			 type == SWF_DEFINEFONT || type == SWF_DEFINEFONT2 ||
			 type == SWF_DEFINEEDITTEXT ||
			 type == SWF_DEFINEVIDEOSTREAM ||
			 type == SWF_PREBUILTCLIP ||
			 type == SWF_DEFINESOUND ||
			 type == SWF_BROWSERFONT)
	{
		return TRUE;
	}
	else
		return FALSE;
}


void
SWFCharacter_setFinished(SWFCharacter character)
{
	character->isFinished = TRUE;
}


BOOL
SWFCharacter_isFinished(SWFCharacter character)
{
	return character->isFinished;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
