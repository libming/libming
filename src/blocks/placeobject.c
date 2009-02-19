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

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#include "placeobject.h"
#include "method.h"
#include "block.h"
#include "output.h"
#include "browserfont.h"
#include "outputblock.h"
#include "character.h"
#include "matrix.h"
#include "cxform.h"
#include "filter.h"
#include "action.h"

#include "libming.h"

struct SWFPlaceObject2Block_s
{
	struct SWFBlock_s block;

	SWFOutput out;

	// PlaceObject - version 
	// default is 2
	// 3 if with SWF_versiom >= 8 and V3 extensions were used
	int version;  

	SWFCharacter character;
	SWFMatrix matrix;
	SWFCXform cXform;
	int ratio;
	int masklevel;
	char *name;
	int depth;
	int move;

	int nActions;
	int actionORFlags;
	SWFAction *actions;
	int *actionFlags;
//	char *actionChars;
	
	// V3 extension SWF_version >= 8
	char hasCacheFlag;
	char hasBlendFlag;
	char hasFilterFlag;

	SWFFilterList filterList; 
	int blendMode;

#if TRACK_ALLOCS
	mem_node *gcnode;
#endif	
}; 


static void writeActions(SWFPlaceObject2Block place)
{
	int i;
	SWFBlock block = BLOCK(place);
	
	if ( place->nActions > 0 )
	{
		SWFOutput_writeUInt16(place->out, 0); 
		
		if(block->swfVersion >= 6)
			SWFOutput_writeUInt32(place->out, place->actionORFlags);
		else
			SWFOutput_writeUInt16(place->out, place->actionORFlags);

		for ( i=0; i<place->nActions; ++i )
		{
			int length;
			SWFAction_compile(place->actions[i], block->swfVersion, &length);
			if(block->swfVersion >= 6)
				SWFOutput_writeUInt32(place->out, place->actionFlags[i]);
			else
				SWFOutput_writeUInt16(place->out, place->actionFlags[i]);
			
			// SWF6: extra char if(place->actionFlags[i] & 0x20000)
			if((block->swfVersion >= 6) && (place->actionFlags[i] & 0x20000)) {
				SWFOutput_writeUInt32(place->out, length + 1);
				SWFOutput_writeUInt8(place->out, 0);
			} 
			else 
				SWFOutput_writeUInt32(place->out, length);
			
			SWFOutput_writeAction(place->out, place->actions[i]);
		}
		
		/* trailing 0 for end of actions */
		if(block->swfVersion >= 6)
			SWFOutput_writeUInt32(place->out, 0); 
		else
			SWFOutput_writeUInt16(place->out, 0); 
	}
}

void
writeSWFPlaceObject2BlockToStream(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFPlaceObject2Block place = (SWFPlaceObject2Block)block;
	SWFOutput_writeToMethod(place->out, method, data);
}


int
completeSWFPlaceObject2Block(SWFBlock block)
{
	SWFPlaceObject2Block place = (SWFPlaceObject2Block)block;
	SWFOutput out = newSizedSWFOutput(42);

	int flags =
		((place->name != NULL)			? SWF_PLACE_HAS_NAME : 0) |
		((place->ratio != -1)				? SWF_PLACE_HAS_RATIO : 0) |
		((place->masklevel != -1)		? SWF_PLACE_HAS_MASK : 0) |
		((place->cXform != NULL)		? SWF_PLACE_HAS_CXFORM : 0) |
		((place->matrix != NULL)		? SWF_PLACE_HAS_MATRIX : 0) |
		((place->character != NULL) ? SWF_PLACE_HAS_CHARACTER : 0) |
		((place->move != 0)					? SWF_PLACE_MOVE : 0) |
		((place->nActions != 0)			? SWF_PLACE_HAS_ACTIONS : 0);

	SWFOutput_writeUInt8(out, flags);
	if(place->version == 3)
	{
		flags = 0;
		if(place->hasCacheFlag) flags |= SWF_PLACE_CACHE;
		if(place->hasBlendFlag) flags |= SWF_PLACE_HAS_BLEND;
		if(place->hasFilterFlag) flags |= SWF_PLACE_HAS_FILTER;
		SWFOutput_writeUInt8(out, flags);
	}
	SWFOutput_writeUInt16(out, place->depth);
	
	if ( place->character != NULL )
		SWFOutput_writeUInt16(out, CHARACTERID(place->character));

	if ( place->matrix != NULL )
		SWFOutput_writeMatrix(out, place->matrix);

	if ( place->cXform != NULL )
		SWFOutput_writeCXform(out, place->cXform, SWF_PLACEOBJECT2);

	if ( place->ratio != -1 )
		SWFOutput_writeUInt16(out, place->ratio);

	if ( place->name != NULL )
		SWFOutput_writeString(out, (byte*)place->name);

	if ( place->masklevel != -1 )
		SWFOutput_writeUInt16(out, place->masklevel);

	if( place->version == 3 && place->hasFilterFlag)
		SWFOutput_writeFilterList(out, place->filterList);		

	if( place->version == 3 && place->hasBlendFlag)
		SWFOutput_writeUInt8(out, place->blendMode);

	place->out = out;
	writeActions(place);

	return SWFOutput_getLength(out);
}


void
destroySWFPlaceObject2Block(SWFPlaceObject2Block place)
{
	if ( place->actions != NULL )
		free(place->actions);

	if ( place->actionFlags != NULL )
//	{	free(place->actionChars);
		free(place->actionFlags);
//	}

	if( place->filterList != NULL )
		destroySWFFilterList(place->filterList);		

	if ( place->name != NULL )
		free(place->name);

	if ( place->out != NULL )
		destroySWFOutput(place->out);

	if ( place->matrix != NULL )
		destroySWFMatrix(place->matrix);

	if ( place->cXform != NULL )
		destroySWFCXform(place->cXform);

#if TRACK_ALLOCS
	ming_gc_remove_node(place->gcnode);
#endif

	free(place);
}


static inline void
setPlaceObjectVersion(SWFPlaceObject2Block block, int version)
{
	switch(version)
	{
		case 2: 
			block->version = version;
			BLOCK(block)->type = SWF_PLACEOBJECT2;
			break;
		case 3:
			block->version = version;
			BLOCK(block)->type = SWF_PLACEOBJECT3;
			break;
		default:
			SWF_error("setPlaceObjectVersion: invalid version %i\n",
                                  version);
	}
}

SWFPlaceObject2Block
newSWFPlaceObject2Block(int depth)
{
	SWFPlaceObject2Block place = (SWFPlaceObject2Block)malloc(sizeof(struct SWFPlaceObject2Block_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == place)
		return NULL;

	SWFBlockInit((SWFBlock)place);

	BLOCK(place)->type = SWF_PLACEOBJECT2;
	BLOCK(place)->writeBlock = writeSWFPlaceObject2BlockToStream;
	BLOCK(place)->complete = completeSWFPlaceObject2Block;
	BLOCK(place)->dtor = (destroySWFBlockMethod) destroySWFPlaceObject2Block;

	place->version = 2;
	place->out = NULL;
	place->name = NULL;

	place->move = 0;
	place->matrix = NULL;
	place->cXform = NULL;
	place->character = NULL;
	place->masklevel = -1;
	place->ratio = -1;
	place->depth = depth;

	place->nActions = 0;
	place->actionORFlags = 0;
	place->actionFlags = NULL;
//	place->actionChars = NULL;
	place->actions = NULL;

	place->hasCacheFlag = 0;
	place->hasBlendFlag = 0;
	place->hasFilterFlag = 0;
	place->filterList = NULL;
#if TRACK_ALLOCS
	place->gcnode = ming_gc_add_node(place, (dtorfunctype) destroySWFPlaceObject2Block);
#endif
	return place;
}


void
SWFPlaceObject2Block_setName(SWFPlaceObject2Block block, const char *name)
{
	if ( block->name != NULL )
		free(block->name);

	block->name = strdup(name);
}


void
SWFPlaceObject2Block_setRatio(SWFPlaceObject2Block block, int ratio)
{
	block->ratio = ratio;
}


void
SWFPlaceObject2Block_setDepth(SWFPlaceObject2Block block, int depth)
{
	block->depth = depth;
}


void
SWFPlaceObject2Block_setMaskLevel(SWFPlaceObject2Block block, int masklevel)
{
	block->masklevel = masklevel;
}


void
SWFPlaceObject2Block_setCXform(SWFPlaceObject2Block block, SWFCXform cXform)
{
	block->cXform = cXform;
}


void
SWFPlaceObject2Block_setColorAdd(SWFPlaceObject2Block block,
																 int r, int g, int b, int a)
{
	if ( block->cXform == NULL )
		block->cXform = newSWFAddCXform(r, g, b, a);
	else
		SWFCXform_setColorAdd(block->cXform, r, g, b, a);
}


void
SWFPlaceObject2Block_setColorMult(SWFPlaceObject2Block block,
																	float r, float g, float b, float a)
{
	if ( block->cXform == NULL )
		block->cXform = newSWFMultCXform(r, g, b, a);
	else
		SWFCXform_setColorMult(block->cXform, r, g, b, a);
}


void
SWFPlaceObject2Block_setMatrix(SWFPlaceObject2Block block, SWFMatrix matrix)
{
	if ( block->matrix != NULL )
		free(block->matrix);

	block->matrix = SWFMatrix_dup(matrix);
}


void
SWFPlaceObject2Block_setCharacter(SWFPlaceObject2Block block,
																	SWFCharacter character)
{
	block->character = character;
}


void
SWFPlaceObject2Block_setMove(SWFPlaceObject2Block block)
{
	block->move = 1;
}


void
SWFPlaceObject2Block_addAction(SWFPlaceObject2Block block,
															 SWFAction action, int flags)
{
	block->actions =
		(SWFAction*)realloc(block->actions, (block->nActions+1) * sizeof(SWFAction));

	block->actionFlags =
		(int*)realloc(block->actionFlags, (block->nActions+1) * sizeof(int));

//	block->actionChars =
//		realloc(block->actionChars, (block->nActions+1));

	block->actions[block->nActions] = action;
	block->actionFlags[block->nActions] = flags;
//	block->actionChars[block->nActions] = actChar;
	block->actionORFlags |= flags;

	++block->nActions;
}


/* 
 * set Cache as BitmapFlag 
 * Only if SWF Version >= 8. Sets PlaceObject version to 3
 */
void 
SWFPlaceObject2Block_setCacheFlag(SWFPlaceObject2Block block, int flag)
{
	setPlaceObjectVersion(block, 3);
	block->hasCacheFlag = 1;
}

/* 
 * set blend mode. 
 * See ming.h for possible blend modes
 * Only if SWF Version >= 8. Sets PlaceObject version to 3
 */
void 
SWFPlaceObject2Block_setBlendMode(SWFPlaceObject2Block block, int mode)
{
	if(mode < 0 || mode > 255)
	{
		SWF_warn("SWFPlaceObject2Block_setBlendMode: mode must be in between [0...255]");
		return;
	}
	
	setPlaceObjectVersion(block, 3);
	block->hasBlendFlag = 1;
	block->blendMode = mode;
}


/*
 * add filter
 * see ming.h for filtertypes
 * Only if SWF Version >= 8. Sets PlaceObject version to 3
 */
void 
SWFPlaceObject2Block_addFilter(SWFPlaceObject2Block block, SWFFilter filter)
{
	if(block->filterList == NULL)
	{
		setPlaceObjectVersion(block, 3);
		block->filterList = newSWFFilterList();
		block->hasFilterFlag = 1;
	}
	SWFFilterList_add(block->filterList, filter);
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
