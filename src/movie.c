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
#include <math.h>

#include "ming.h"

#include "movie.h"
#include "shape_util.h"

#include "blocks/exports.h"
#include "blocks/rect.h"


struct SWFMovie_s
{
	SWFBlockList blockList;
	SWFDisplayList displayList;

	/* frame rate */
	float rate;

	/* movie bounds */
	SWFRect bounds;

	/* number of frames currently assigned */
	unsigned short nFrames;

	/* total number of frames, if requested by user */
	unsigned short totalFrames;

	/* version number of this movie */
	byte version;

	/* export items */
	int nExports;
	struct SWFExport_s *exports;

	/* list of fonts used in this movie */
	int nFonts;
	SWFFontCharacter* fonts;

	/* background color */
	byte r;
	byte g;
	byte b;
};


static void
destroySWFExports(SWFMovie movie)
{
	int n;

	for ( n=0; n<movie->nExports; ++n )
		free(movie->exports[n].name);

	free(movie->exports);

	movie->nExports = 0;
	movie->exports = NULL;
}


void
destroySWFMovie(SWFMovie movie)
{
	destroySWFBlockList(movie->blockList);
	destroySWFDisplayList(movie->displayList);
	destroySWFRect(movie->bounds);

	if ( movie->nExports > 0 )
		destroySWFExports(movie);

	if ( movie->fonts != NULL )
		free(movie->fonts);

	free(movie);
}


SWFMovie
newSWFMovieWithVersion(int version)
{
	SWFMovie movie = malloc(sizeof(struct SWFMovie_s));

	movie->version = version;
	movie->blockList = newSWFBlockList();
	movie->displayList = newSWFDisplayList();
	movie->bounds = newSWFRect(0, 320*20, 0, 240*20);
	movie->rate = 12.0;
	movie->totalFrames = 1;
	movie->nFrames = 0;

	movie->nExports = 0;
	movie->exports = NULL;

	movie->nFonts = 0;
	movie->fonts = NULL;

	movie->r = 0xff;
	movie->g = 0xff;
	movie->b = 0xff;

	return movie;
}


SWFMovie
newSWFMovie()
{
	return newSWFMovieWithVersion(SWF_versionNum);
}


void
SWFMovie_setRate(SWFMovie movie, float rate)
{
	movie->rate = rate;
}


void
SWFMovie_setDimension(SWFMovie movie, float width, float height)
{
	if ( movie->bounds != NULL )
		free(movie->bounds);
	
	movie->bounds = newSWFRect(0, (int)rint(Ming_scale*width),
														 0, (int)rint(Ming_scale*height));
}


void
SWFMovie_setNumberOfFrames(SWFMovie movie, int totalFrames)
{
	movie->totalFrames = totalFrames;
}


void
SWFMovie_setBackground(SWFMovie movie, byte r, byte g, byte b)
{
	movie->r = r;
	movie->g = g;
	movie->b = b;
}


void
SWFMovie_protect(SWFMovie movie)
{
	SWFMovie_addBlock(movie, newSWFProtectBlock());
}


static void
SWFMovie_resolveTextFonts(SWFMovie movie, SWFText text)
{
	// translate text object font references to movie-specific fontchars

	SWFTextRecord record = SWFText_getInitialRecord(text);
	SWFFontCharacter fontchar;

	while ( record != NULL )
	{
		SWFFont font = SWFTextRecord_getFont(record);
		int i;

		if ( font != NULL )
		{
			for ( i = 0; i < movie->nFonts; ++i )
			{
				fontchar = movie->fonts[i];

				if ( SWFFontCharacter_getFont(fontchar) == font )
				{
					SWFTextRecord_setFontCharacter(record, fontchar);
					break;
				}
			}

			if ( i == movie->nFonts )
			{
				movie->fonts = realloc(movie->fonts,
															 sizeof(SWFFontCharacter) * (movie->nFonts + 1));

				fontchar = newSWFFontCharacter(font);
				movie->fonts[movie->nFonts++] = fontchar;
				SWFTextRecord_setFontCharacter(record, fontchar);
				SWFMovie_addBlock(movie, (SWFBlock)fontchar);
			}
		}

		record = SWFTextRecord_getNextRecord(record);
	}
}


void
SWFMovie_addBlock(SWFMovie movie, SWFBlock block)
{
	if ( SWFBlock_getType(block) == SWF_SHOWFRAME )
		++movie->nFrames;

	SWFBlockList_addBlock(movie->blockList, block);
}


void
SWFMovie_addExport(SWFMovie movie, SWFBlock block, char *name)
{
	if ( SWFBlock_getType(block) == SWF_DEFINESPRITE )
	{
		movie->exports = realloc(movie->exports,
														 (movie->nExports+1) * sizeof(struct SWFExport_s));

		movie->exports[movie->nExports].block = block;
		movie->exports[movie->nExports].name = strdup(name);

		++movie->nExports;
	}
}


static void
SWFMovie_addDependency(SWFMovie movie, SWFCharacter character)
{
	if ( SWFBlock_getType((SWFBlock)character) == SWF_DEFINETEXT ||
			 SWFBlock_getType((SWFBlock)character) == SWF_DEFINETEXT2 )
	{
		SWFMovie_resolveTextFonts(movie, (SWFText)character);
	}

	SWFMovie_addBlock(movie, (SWFBlock)character);
}


static void
SWFMovie_addCharacterDependencies(SWFMovie movie, SWFCharacter character)
{
	SWFCharacter* deps = NULL;
	int nDeps = 0;

	if ( SWFCharacter_getDependencies(character, &deps, &nDeps) )
	{
		int i;

		for ( i = 0; i < nDeps; ++i )
			SWFMovie_addDependency(movie, deps[i]);

		free(deps);
	}
}


void
SWFMovie_writeExports(SWFMovie movie)
{
	int n;
	SWFBlock exports;

	if ( movie->nExports == 0 )
		return;

	for ( n=0; n<movie->nExports; ++n )
	{
		SWFBlock b = movie->exports[n].block;

		if ( SWFBlock_isCharacter(b) && !SWFBlock_isDefined(b) )
		{
			SWFMovie_addCharacterDependencies(movie, (SWFCharacter)b);
			completeSWFBlock(b);
			SWFMovie_addBlock(movie, b);
		}
	}

	exports = (SWFBlock)newSWFExportBlock(movie->exports, movie->nExports);

	SWFMovie_addBlock(movie, exports);

	destroySWFExports(movie);
}


SWFDisplayItem
SWFMovie_add(SWFMovie movie, SWFBlock block)
{
	if ( block == NULL )
		return NULL;

	/* if they're trying to add a raw bitmap, we'll be nice and turn
		 it into a shape */

	if ( SWFBlock_getType(block) == SWF_DEFINEBITS ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG2 ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG3 ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS2 )
	{
		block = (SWFBlock)newSWFShapeFromBitmap((SWFBitmap)block);
	}

	/* if it's a text object, we need to translate fonts into font characters */

	if ( SWFBlock_getType(block) == SWF_DEFINETEXT ||
			 SWFBlock_getType(block) == SWF_DEFINETEXT2 )
	{
		SWFMovie_resolveTextFonts(movie, (SWFText)block);
	}

	if ( SWFBlock_isCharacter(block) )
	{
		SWFCharacter_setFinished((SWFCharacter)block);
		SWFMovie_addCharacterDependencies(movie, (SWFCharacter)block);

		return SWFDisplayList_add(movie->displayList, (SWFCharacter)block);
	}
	else
		SWFMovie_addBlock(movie, block);

	return NULL;
}


void
SWFMovie_remove(SWFMovie movie, SWFDisplayItem item)
{
	SWFDisplayItem_remove(item);
}


void
SWFMovie_setSoundStream(SWFMovie movie, SWFSoundStream stream)
{
	SWFBlock block = SWFSoundStream_getStreamHead(stream, movie->rate);

	if ( block != NULL )
	{
		SWFMovie_addBlock(movie, block);
		SWFDisplayList_setSoundStream(movie->displayList, stream);
	}
}


SWFSoundInstance
SWFMovie_startSound(SWFMovie movie, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance(sound);

	if ( !SWFBlock_isDefined((SWFBlock)sound) )
		SWFMovie_addBlock(movie, (SWFBlock)sound);

	SWFMovie_addBlock(movie, (SWFBlock)inst);

	return inst;
}


void
SWFMovie_stopSound(SWFMovie movie, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance_stop(sound);

	// XXX - ???
	if ( !SWFBlock_isDefined((SWFBlock)sound) )
		SWFMovie_addBlock(movie, (SWFBlock)sound);

	SWFMovie_addBlock(movie, (SWFBlock)inst);
}


void
SWFMovie_nextFrame(SWFMovie movie)
{
	SWFDisplayList_writeBlocks(movie->displayList, movie->blockList);
	SWFMovie_addBlock(movie, newSWFShowFrameBlock());
}


void
SWFMovie_labelFrame(SWFMovie movie, char *label)
{
	SWFMovie_addBlock(movie, (SWFBlock)newSWFFrameLabelBlock(label));
}


int
SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data)
{
	int length;
	SWFOutput header;
	SWFBlock backgroundBlock;

	if ( movie->nExports > 0 )
		SWFMovie_writeExports(movie);

	while ( movie->nFrames < movie->totalFrames )
		SWFMovie_nextFrame(movie);

	SWFMovie_addBlock(movie, newSWFEndBlock());

	// add five for the setbackground block..
	length = SWFBlockList_completeBlocks(movie->blockList) + 5;

	/* XXX - hack */
	SWFDisplayList_rewindSoundStream(movie->displayList);

	header = newSizedSWFOutput(20);

	/* figure size, write header */

	SWFOutput_writeRect(header, movie->bounds);
	SWFOutput_writeUInt16(header, (int)floor(movie->rate*256));
	SWFOutput_writeUInt16(header, movie->nFrames);

	SWFOutput_byteAlign(header);
	length += 8 + SWFOutput_getLength(header);

	method('F', data);
	method('W', data);
	method('S', data);
	method(movie->version, data);
	methodWriteUInt32(length, method, data);
	SWFOutput_writeToMethod(header, method, data);

	destroySWFOutput(header);

	backgroundBlock =
		(SWFBlock)newSWFSetBackgroundBlock(movie->r, movie->g, movie->b);

	writeSWFBlockToMethod(backgroundBlock, method, data);
	SWFBlockList_writeBlocksToMethod(movie->blockList, method, data);

	destroySWFBlock(backgroundBlock);

	return length;
}


int
SWFMovie_save(SWFMovie movie, char *filename)
{
	FILE *f = fopen(filename, "wb");
	int count;

	if ( f == NULL )
		return -1;

	count = SWFMovie_output(movie, fileOutputMethod, f);

	fclose(f);

	return count;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
