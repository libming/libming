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

/*
 * destroy a SWFMovieClip instance
 */
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

/*
 * create a new SWFMovieClip (Sprite) instance
 * A SWFMovieClip is a movie inside a movie. It supports 
 * adding and removing displayable objects, actions and sound
 *
 * returns a SWFMovieClip instance.
 */
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


/*
 * set the frame count of a movie
 * This function sets the frame count for the movieclip. If the number of
 * frames specified here exceeds the number of frame created when adding
 * content to the movie, then blank frames will be added to the movieclip to
 * pad it to the specified frame count.
 */
void
SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int totalFrames)
{
	SWFSprite_setNumberOfFrames((SWFSprite)clip, totalFrames);
}

/* 
 * Includes streaming sound to a movie. 
 * Streaming (embedded) sound is played in sync with movie frames. 
 * The skip parameter describes the time in seconds (float) to be skiped.
 * The rate parameter describes the parent movies framerate.
 *
 * See also SWFMovie_setSoundStream, SWFMovie_setSoundStreamAt, newSWFSoundStream,
 * SWFMovieClip_setSoundStream
 */
void
SWFMovieClip_setSoundStreamAt(SWFMovieClip clip, 
                              SWFSoundStream sound /* soundstream object */, 
                              float rate /* framerate */, 
                              float skip /* skip in seconds */)
{
	SWFBlock block = SWFSoundStream_getStreamHead(sound, rate, skip);
	
	if ( block != NULL )
	{
		SWFBlockList_addBlock(clip->blockList, block);
		SWFDisplayList_setSoundStream(clip->displayList, sound);
	}
}

/* 
 * Includes streaming sound to a movie. 
 * Streaming (embedded) sound is played in sync with movie frames. 
 *
 * See also SWFMovie_setSoundStream, SWFMovie_setSoundStreamAt, newSWFSoundStream,
 * SWFMovieClip_setSoundStreamAt
 */
void
SWFMovieClip_setSoundStream(SWFMovieClip clip, SWFSoundStream sound, float rate)
{
	SWFMovieClip_setSoundStreamAt(clip, sound, rate, 0);
}


/*
 * Starts playing event-sound (SWFSound)
 *
 * returns a SWFSoundInstance object.
 * see also SWFSoundInstance, SWFSound, SWFMovieClip_stopSound
 */
SWFSoundInstance
SWFMovieClip_startSound(SWFMovieClip clip, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance(sound);

	SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)sound);
	SWFBlockList_addBlock(clip->blockList, (SWFBlock)inst);

	return inst;
}


/*
 * Stops playing event sound (SWFSound)
 *
 * see also SWFSoundInstance, SWFSound, SWFMovieClip_startSound
 */
void
SWFMovieClip_stopSound(SWFMovieClip clip, SWFSound sound)
{
	SWFCharacter_addDependency((SWFCharacter)clip, (SWFCharacter)sound);

	SWFBlockList_addBlock(clip->blockList,
												(SWFBlock)newSWFSoundInstance_stop(sound));
}

/*
 * Adds a block to a movie.
 * This function adds a block or character to a movieclip.
 * returns a SWFDisplayItem 
 */
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

		return SWFDisplayList_add(clip->displayList, clip->blockList, (SWFCharacter)block);
	}
	else
	{
		/* XXX - make sure it's a legit block for a sprite */
		SWFBlockList_addBlock(clip->blockList, block);
	}

	return NULL;
}

/* 
 * Removes a SWFDisplayItem from the movieclips stage.
 * A SWFDisplayItem stays on stage until it gets removed. This function 
 * removes the item. The item is displayed until the current frame.
 */
void
SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item)
{
	SWFDisplayItem_remove(item);
}

/*
 * Add a label Frame
 * This function adds a labelFrame to current frame of the movieclip.
 */
void
SWFMovieClip_labelFrame(SWFMovieClip clip, const char *label)
{
	SWFSprite_addBlock((SWFSprite)clip, (SWFBlock)newSWFFrameLabelBlock(label));
}

/*
 * Add a new frame to the current movie.
 * This function adds a new frame to the current movie. All items added, removed
 * manipulated effect this frame and probably following frames.
 */
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

/* 
 * add an initAction block to a MovieClip
 *
 * These actions are executed *before* the MovieClip is available as
 * a script object.
 */
void
SWFMovieClip_addInitAction(SWFMovieClip clip, SWFAction action)
{
	if(clip->sprite.initAction)
		destroySWFInitAction(clip->sprite.initAction);

	clip->sprite.initAction = newSWFInitAction_MovieClip(clip, action);
}
/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
