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

#include <stdlib.h>

#include "movieclip.h"
#include "blocklist.h"
#include "displaylist.h"
#include "blocks/soundstream.h"
#include "blocks/soundinstance.h"
#include "blocks/outputblock.h"
#include "blocks/sprite.h"
#include "blocks/shape.h"
#include "blocks/morph.h"
#include "blocks/button.h"
#include "blocks/text.h"
#include "blocks/scalinggrid.h"
#include "libming.h"

struct SWFMovieClip_s
{
	struct SWFSprite_s sprite;

	SWFBlockList blockList;
	SWFDisplayList displayList;
	unsigned short nFrames;
	
#if TRACK_ALLOCS
	/* memory node for garbage collection */
	mem_node *gcnode;
#endif
};


void
destroySWFMovieClip(SWFMovieClip movieClip)
{
#if TRACK_ALLOCS
	ming_gc_remove_node(movieClip->gcnode);
#endif

	destroySWFBlockList(movieClip->blockList);
	destroySWFDisplayList(movieClip->displayList);
	destroySWFSprite((SWFSprite)movieClip);

}


SWFMovieClip
newSWFMovieClip()
{
	SWFMovieClip clip = (SWFMovieClip)newSWFSprite();
	clip = (SWFMovieClip)realloc(clip, sizeof(struct SWFMovieClip_s));

	clip->blockList = newSWFBlockList();
	clip->displayList = newSWFSpriteDisplayList();
#if TRACK_ALLOCS
	clip->gcnode = ming_gc_add_node(clip, (dtorfunctype) destroySWFMovieClip);
#endif

	return clip;
}


void
SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int totalFrames)
{
	SWFSprite_setNumberOfFrames((SWFSprite)clip, totalFrames);
}


void
SWFMovieClip_setSoundStream(SWFMovieClip clip,
														SWFSoundStream sound, float rate)
{
	SWFBlock block = SWFSoundStream_getStreamHead(sound, rate, 0);
	
	if ( block != NULL )
	{
		SWFBlockList_addBlock(clip->blockList, block);
		SWFDisplayList_setSoundStream(clip->displayList, sound);
	}
}


SWFSoundInstance
SWFMovieClip_startSound(SWFMovieClip clip, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance(sound);

	SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)sound);
	SWFBlockList_addBlock(clip->blockList, (SWFBlock)inst);

	return inst;
}


void
SWFMovieClip_stopSound(SWFMovieClip clip, SWFSound sound)
{
	SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)sound);

	SWFBlockList_addBlock(clip->blockList,
												(SWFBlock)newSWFSoundInstance_stop(sound));
}


SWFDisplayItem
SWFMovieClip_add(SWFMovieClip clip, SWFBlock block)
{
	if ( SWFBlock_getType(block) == SWF_DEFINEBITS ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG2 ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG3 ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS2 )
	{
		block = (SWFBlock)newSWFShapeFromBitmap((SWFBitmap)block, SWFFILL_TILED_BITMAP);
	}

	if ( SWFBlock_isCharacter(block) )
	{
		SWFCharacter_getDependencies((SWFCharacter)block,
																 &CHARACTER(clip)->dependencies,
																 &CHARACTER(clip)->nDependencies);

		SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)block);

		SWFCharacter_setFinished((SWFCharacter)block);

		return SWFDisplayList_add(clip->displayList, (SWFCharacter)block);
	}
	else
	{
		/* XXX - make sure it's a legit block for a sprite */
		SWFBlockList_addBlock(clip->blockList, block);
	}

	return NULL;
}


void
SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item)
{
	SWFDisplayItem_remove(item);
}


void
SWFMovieClip_labelFrame(SWFMovieClip clip, const char *label)
{
	SWFSprite_addBlock((SWFSprite)clip, (SWFBlock)newSWFFrameLabelBlock(label));
}


void
SWFMovieClip_nextFrame(SWFMovieClip clip)
{
	SWFDisplayList_writeBlocks(clip->displayList, clip->blockList);
	SWFBlockList_addToSprite(clip->blockList, (SWFSprite)clip);
	SWFSprite_addBlock((SWFSprite)clip, newSWFShowFrameBlock());
}


/*
 * set a 9 slice scaling grid to this sprite / movieclip / button
 * This function sets a 9 slice scaling grid to buttons / sprites / movieclips
 *
 *      1 2 3
 *      4 5 6 
 *      7 8 9
 *
 * x, y, w and h define a rectangle, which is the dimension of the center slice (5).
 * All other slices are determined out of the characters bounds and the defined rect.
 * While slice 5 is scaled vertical and horizontal, slice 2 and 8 are only scaled horizontal.
 * slice 4 and 6 only vertical. the 4 corner slices are not scaled (1, 3, 7, 9).  
 */
void 
SWFMovieClip_setScalingGrid(SWFMovieClip clip, int x, int y, int w, int h)
{
	if(clip->sprite.grid)
		destroySWFScalingGrid(clip->sprite.grid);

	clip->sprite.grid = newSWFScalingGrid((SWFCharacter)clip, x, y, w, h);
}

/*
 * removes the scaling grid
 */
void 
SWFMovieClip_removeScalingGrid(SWFMovieClip clip)
{
	if(clip->sprite.grid)
	{
		destroySWFScalingGrid(clip->sprite.grid);
		clip->sprite.grid = NULL;
	}
}

void
SWFMovieClip_addInitAction(SWFMovieClip clip, SWFAction action)
{
	if(clip->sprite.initAction)
		destroySWFInitAction(clip->sprite.initAction);

	clip->sprite.initAction = newSWFInitAction(clip, action);
}
/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
