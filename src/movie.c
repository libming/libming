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

#include "ming_config.h"

#include "movie.h"
#include "shape_util.h"
#include "blocklist.h"
#include "displaylist.h"

/*
 * If we move math.h include *after* block.h
 * compiler issues errors (?!)
 */
#ifndef __C2MAN__
#include <math.h>
#endif

#include "blocks/block.h"
#include "blocks/method.h"
#include "blocks/browserfont.h"
#include "blocks/character.h"
#include "blocks/outputblock.h"
#include "blocks/soundstream.h"
#include "blocks/exports.h"
#include "blocks/imports.h"
#include "blocks/rect.h"
#include "blocks/font.h"
#include "blocks/textfield.h"
#include "blocks/shape.h"
#include "blocks/soundinstance.h"
#include "blocks/fileattrs.h"
#include "blocks/metadata.h"
#include "blocks/scriptlimits.h"
#include "blocks/tabindex.h"
#include "blocks/sprite.h"
#include "blocks/symbolclass.h"
#include "blocks/scenedata.h"

#include "libming.h"

#if USE_ZLIB
# include <zlib.h>
#endif

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

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

	/* import items */
	int nImports;
	struct SWFImportBlock_s **imports;

	/* list of fonts used in this movie */
	int nFonts;
	SWFFontCharacter* fonts;

	/* background color */
	SWFBlock backgroundBlock;
	
	/* Fileattributes (necessary for version >= 8 */
	SWFFileAttributes fattrs;

	/* Metadata object */
	SWFMetadata metadata;

	/* Script limits */
	SWFScriptLimits limits;

	/* (exported) Symbol table (SWF >= 9) */
	SWFSymbolClass symbolClass;

	/* scene and frame definitions (SWF >= 9) */
	SWFSceneData sceneData;	
#if TRACK_ALLOCS
	/* memory node for garbage collection */
	mem_node *gcnode;
#endif
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

/*
 * destroy a SWFMovie
 * This function destroys a SWFMovie and frees the memmory associated with it
 */
void
destroySWFMovie(SWFMovie movie /* Movie to be destroyed */)
{
	destroySWFBlockList(movie->blockList);
	destroySWFDisplayList(movie->displayList);
	destroySWFRect(movie->bounds);

	if ( movie->nExports > 0 )
		destroySWFExports(movie);

	if ( movie->fonts != NULL )
		free(movie->fonts);

	if (movie->imports)
		free(movie->imports);
	
	if(movie->fattrs)
		destroySWFFileAttributes(movie->fattrs);

	if(movie->limits)
		destroySWFScriptLimits(movie->limits);

	if(movie->backgroundBlock)
		destroySWFBlock(movie->backgroundBlock);

#if TRACK_ALLOCS
	ming_gc_remove_node(movie->gcnode);
#endif

	free(movie);
}

/*
 * create a new SWFMovie
 * This function creates a new SWFMovie with the specified version.
 */
SWFMovie
newSWFMovieWithVersion(int version /* Flash version */)
{
	SWFMovie movie;

	Ming_useSWFVersion(version);

	movie = (SWFMovie) malloc(sizeof(struct SWFMovie_s));

	/* If malloc failed, return NULL to signify this */
	if (movie == NULL)
		return NULL;

	movie->version = version;
	movie->blockList = newSWFBlockList();
	movie->displayList = newSWFDisplayList();
	/* Default movie dimension is hard-coded to 320x240 pixels */
	movie->bounds = newSWFRect(0, 320*20, 0, 240*20);
	movie->rate = 12.0;
	movie->totalFrames = 0;
	movie->nFrames = 0;

	movie->nExports = 0;
	movie->exports = NULL;

	movie->nImports = 0;
	movie->imports = NULL;

	movie->nFonts = 0;
	movie->fonts = NULL;

	movie->backgroundBlock = NULL;

	if(version >= 8)	
		movie->fattrs = newSWFFileAttributes();
	else
		movie->fattrs = NULL;
	movie->metadata = NULL;
	movie->limits = NULL;
	movie->symbolClass = NULL;
	movie->sceneData = NULL;
#if TRACK_ALLOCS
	movie->gcnode = ming_gc_add_node(movie, (dtorfunctype) destroySWFMovie);
#endif

	return movie;
}

/*
 * create a new SWFMovie
 * This function creates a new SWFMovie with the default version.
 */
SWFMovie
newSWFMovie()
{
	return newSWFMovieWithVersion(SWF_versionNum);
}

/*
 * set the frame rate of a movie
 * This function sets the frame rate for the movie.
 */
void
SWFMovie_setRate(SWFMovie movie /* movie to adjust */,
	float rate	/* new frame rate */)
{
	movie->rate = rate;
}

/*
 * get the frame rate of a movie
 * This function gets the frame rate for the movie.
 */
float
SWFMovie_getRate(SWFMovie movie /* movie to adjust */)
{
	return movie->rate;
}

/*
 * set the dimensions of a movie
 * This function sets the dimensions for the movie.
 */
void
SWFMovie_setDimension(SWFMovie movie /* movie to adjust */,
	float width	/* new width of the movie */,
	float height	/* new height of the movie */)
{
	if ( movie->bounds != NULL )
		free(movie->bounds);
	
	/*printf("Ming_scale: %g, with: %g, height: %g\n", Ming_scale, width, height);*/
	movie->bounds = newSWFRect(0, (int)rint(Ming_scale*width),
														 0, (int)rint(Ming_scale*height));
}

/*
 * set the frame count of a movie
 * This function sets the frame count for the movie. If the number of
 * frames specified here exceeds the number of frame created when adding
 * content to the movie, then blank frames will be added to the movie to
 * pad it to the specified frame count.
 */
void
SWFMovie_setNumberOfFrames(SWFMovie movie /* movie to adjust */,
	int totalFrames	/* new frame count */)
{
	movie->totalFrames = totalFrames;
}

/*
 * set the background color for a movie
 * This function sets the background color for the movie.
 */
void
SWFMovie_setBackground(SWFMovie movie /* movie whose background is being set */,
	byte r /* red value of background color */,
	byte g /* green value of background color */,
	byte b /* blue value og background color */)
{
	if(movie->backgroundBlock)
		destroySWFBlock(movie->backgroundBlock);
	movie->backgroundBlock = (SWFBlock)newSWFSetBackgroundBlock(r, g, b);
}

/*
 * enable edit protections for a movie
 * This function adds a block that tells flash editors to require a password
 * to allow editing of this this movie. The block contains an md5 encoded
 * password which will be used by the flash editor to allow access to the movie.
 */
void
SWFMovie_protect(SWFMovie movie /* movie to protect */,
		const char *password /* mds5 encoded password */)
{
	SWFMovie_addBlock(movie, newSWFProtect(password));
}

/* 
 * Adds a font to a movie.
 * retursn a SWFFontCharacter object
 */
SWFFontCharacter
SWFMovie_addFont(SWFMovie movie, SWFFont font)
{
	SWFFontCharacter fontchar;
	int i;
	
	for( i = 0 ; i < movie->nFonts ; i++ )
	{	fontchar = movie->fonts[i];
		if ( SWFFontCharacter_getFont(fontchar) == font )
			return fontchar;
	}
	movie->fonts = (SWFFontCharacter*)realloc(movie->fonts,
					 sizeof(SWFFontCharacter) * (movie->nFonts + 1));
	fontchar = newSWFFontCharacter(font);
	movie->fonts[movie->nFonts++] = fontchar;
	SWFMovie_addBlock(movie, (SWFBlock)fontchar);
	return fontchar;
}


static void
SWFMovie_resolveTextFonts(SWFMovie movie, SWFText text)
{
	// translate text object font references to movie-specific fontchars

	SWFTextRecord record = SWFText_getInitialRecord(text);
	SWFFontCharacter fontchar;

	while ( record != NULL )
	{
		SWFFont font = SWFTextRecord_getUnresolvedFont(record);

		if ( font != NULL )
		{	fontchar = SWFMovie_addFont(movie, font);
			SWFTextRecord_setFontCharacter(record, fontchar);
		}
	
		record = SWFTextRecord_getNextRecord(record);
	}
}

static void
SWFMovie_resolveTextfieldFont(SWFMovie movie, SWFTextField field)
{
	// given a font used for a text field, add it to the movie
	SWFFontCharacter fontchar;
	SWFFont font = SWFTextField_getUnresolvedFont(field);

	if ( font != NULL )
	{	fontchar = SWFMovie_addFont(movie, font);
		SWFTextField_setFontCharacter(field, fontchar);
	}
}

void
SWFMovie_addBlock(SWFMovie movie, SWFBlock block)
{
	if ( SWFBlock_getType(block) == SWF_SHOWFRAME )
		++movie->nFrames;

	SWFBlockList_addBlock(movie->blockList, block);
}


/* 
 * Assigns a name to a SWFBlock object.
 * Creates an exportlibrary with named symbols to be imported by other 
 * SWF movies.
 * Call SWFMovie_writeExports() when you're done with the exports
 * to actually write the tag. If you don't the tag will be added
 * at the END of the SWF.
 * see also SWFMovie_importCharacter, SWFMovie_importFont
 */
void
SWFMovie_addExport(SWFMovie movie, SWFBlock block, const char *name)
{
	switch( SWFBlock_getType(block))
	{
		case SWF_DEFINESHAPE:
		case SWF_DEFINESHAPE2:
		case SWF_DEFINESHAPE3:
		case SWF_DEFINESHAPE4:
		case SWF_DEFINEBUTTON:
		case SWF_DEFINEBUTTON2:
		case SWF_DEFINESPRITE:
		case SWF_DEFINEFONT2:
		case SWF_DEFINESOUND:
		case SWF_DEFINELOSSLESS:
		case SWF_DEFINELOSSLESS2:
		case SWF_DEFINEBITS:
		case SWF_DEFINEBITSJPEG2:
		case SWF_DEFINEBITSJPEG3:
			movie->exports = (struct SWFExport_s*)realloc(movie->exports,
					(movie->nExports+1) * sizeof(struct SWFExport_s));
			movie->exports[movie->nExports].block = block;
			movie->exports[movie->nExports].name = strdup(name);
			++movie->nExports;
			break;
		default:
			SWF_error("Exporting a character of type %d is not supported", SWFBlock_getType(block));
			break;
	}
}

static void
SWFMovie_addCharacterDependencies(SWFMovie movie, SWFCharacter character);
static void
SWFMovie_addDependency(SWFMovie movie, SWFCharacter character)
{
	if ( SWFBlock_getType((SWFBlock)character) == SWF_DEFINETEXT ||
			 SWFBlock_getType((SWFBlock)character) == SWF_DEFINETEXT2 )
	{
		SWFMovie_resolveTextFonts(movie, (SWFText)character);
	}
	else if ( SWFBlock_getType((SWFBlock)character) == SWF_DEFINEEDITTEXT)
	{
		SWFMovie_resolveTextfieldFont(movie, (SWFTextField)character);
	}
	else if ( SWFBlock_getType((SWFBlock)character) == SWF_DEFINEFONT)
	{
		SWFMovie_addCharacterDependencies(movie, character);
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

/*
 * Write the EXPORTASSET tag with informations gathered by calls to
 * SWFMovie_addExport.
 *
 * Call this function to control insertion of the EXPORTASSET tag, which
 * is otherwise written at the END of the SWF.
 */
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
		b->swfVersion = movie->version;

		if ( SWFBlock_isCharacter(b) && !SWFBlock_isDefined(b) )
		{
			SWFMovie_addCharacterDependencies(movie, (SWFCharacter)b);
			completeSWFBlock(b);
			SWFMovie_addBlock(movie, b);
			/* Workaround for movieclip exports:
			 * initAction and scalingGrid are only written when 
			 * placing the movieclip. These extra blocks should
			 * not get lost if a MC is eported only 
			 */
			if(SWFBlock_getType(b) == SWF_DEFINESPRITE)
			{
				SWFSprite sprite = (SWFSprite)b;
				if(sprite->grid)
					SWFMovie_addBlock(movie, (SWFBlock)sprite->grid);
				if(sprite->initAction)
					SWFMovie_addBlock(movie, (SWFBlock)sprite->initAction);
			}
		}
	}

	exports = (SWFBlock)newSWFExportBlock(movie->exports, movie->nExports);

	SWFMovie_addBlock(movie, exports);

	destroySWFExports(movie);
}

/* 
 * This function replaces a displayable character with a new one.
 * Do not use this function. Use SWFMovie_replace instead!
 * returns 0 on success 
 */
int
SWFMovie_replace_internal(SWFMovie movie, SWFDisplayItem item, SWFMovieBlockType ublock)
{
	SWFBlock block = ublock.block;
	if(block == NULL || item == NULL)
		return -1;

	if ( SWFBlock_getType(block) == SWF_DEFINEBITS ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG2 ||
			 SWFBlock_getType(block) == SWF_DEFINEBITSJPEG3 ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS ||
			 SWFBlock_getType(block) == SWF_DEFINELOSSLESS2 )
	{
		block = (SWFBlock)newSWFShapeFromBitmap((SWFBitmap)block, SWFFILL_TILED_BITMAP);
	}

	/* if it's a text object, we need to translate fonts into font characters */

	if ( SWFBlock_getType(block) == SWF_DEFINETEXT ||
			 SWFBlock_getType(block) == SWF_DEFINETEXT2 )
	{
		SWFMovie_resolveTextFonts(movie, (SWFText)block);
	}

	if ( SWFBlock_getType(block) == SWF_DEFINEEDITTEXT)
	{
		SWFMovie_resolveTextfieldFont(movie, (SWFTextField)block);
	}
	
	if ( SWFBlock_isCharacter(block) )
	{
		SWFCharacter_setFinished((SWFCharacter)block);
		SWFMovie_addCharacterDependencies(movie, (SWFCharacter)block);

		SWFDisplayItem_replace(item, (SWFCharacter)block);
		return 0;
	}

	SWF_warn("SWFMovie_replace: only characters can be replaced\n");
	return -1;
}


/*
 * Adds a block to a movie.
 * This function adds a block or character to a movie.
 * Do not use this function. Use SWFMovie_add instead
 * returns a SWFDisplayItem 
 */
SWFDisplayItem
SWFMovie_add_internal(SWFMovie movie /* movie to which the block will be added */,
                      SWFMovieBlockType ublock /* block to add to the movie */)
{
	SWFBlock block = ublock.block;
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
		block = (SWFBlock)newSWFShapeFromBitmap((SWFBitmap)block, SWFFILL_TILED_BITMAP);
	}

	/* if it's a text object, we need to translate fonts into font characters */

	if ( SWFBlock_getType(block) == SWF_DEFINETEXT ||
			 SWFBlock_getType(block) == SWF_DEFINETEXT2 )
	{
		SWFMovie_resolveTextFonts(movie, (SWFText)block);
	}

	if ( SWFBlock_getType(block) == SWF_DEFINEEDITTEXT)
	{
		SWFMovie_resolveTextfieldFont(movie, (SWFTextField)block);
	}

	// not nice but has to be done!
	if ( SWFBlock_getType(block) == SWF_INITACTION)
	{
		SWFInitAction init = (SWFInitAction)block;
		SWFMovieClip mc = SWFInitAction_getMovieClip(init);
		if(mc != NULL)
			SWFMovie_addBlock(movie, (SWFBlock)mc);
	} 
	
	if ( SWFBlock_isCharacter(block) )
	{
		
		SWFCharacter_setFinished((SWFCharacter)block);
		SWFMovie_addCharacterDependencies(movie, (SWFCharacter)block);

		return SWFDisplayList_add(movie->displayList, 
			movie->blockList, (SWFCharacter)block);
	}
	else
		SWFMovie_addBlock(movie, block);

	return NULL;
}

/* 
 * Removes a SWFDisplayItem from the movie stage.
 * A SWFDisplayItem stays on stage until it gets removed. This function 
 * removes the item. The item is displayed until the current frame.
 */
void
SWFMovie_remove(SWFMovie movie , SWFDisplayItem item)
{
	SWFDisplayItem_removeFromList(item, movie->blockList);
}

/* 
 * Includes streaming sound to a movie. Sound is skiped by skip seconds.
 * The skip parameter describes the time in seconds (float) to be skiped.
 * See also SWFMovie_setSoundStream, newSWFSoundStream
 */
void
SWFMovie_setSoundStreamAt(SWFMovie movie, SWFSoundStream stream, float skip)
{
	SWFBlock block = SWFSoundStream_getStreamHead(stream, movie->rate, skip);

	if ( block != NULL )
	{
		SWFMovie_addBlock(movie, block);
		SWFDisplayList_setSoundStream(movie->displayList, stream);
	}
}

/* 
 * Includes streaming sound to a movie. 
 * Streaming (embedded) sound is played in sync with movie frames. 
 * See also SWFMovie_setSoundStreamAt, newSWFSoundStream
 */
void
SWFMovie_setSoundStream(SWFMovie movie, SWFSoundStream stream)
{
	SWFMovie_setSoundStreamAt(movie, stream, 0);
}

/*
 * Starts playing event-sound (SWFSound)
 *
 * returns a SWFSoundInstance object.
 * see also SWFSoundInstance, SWFSound, SWFMovie_stopSound
 */
SWFSoundInstance
SWFMovie_startSound(SWFMovie movie, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance(sound);

	if ( !SWFBlock_isDefined((SWFBlock)sound) )
		SWFMovie_addBlock(movie, (SWFBlock)sound);

	SWFMovie_addBlock(movie, (SWFBlock)inst);

	return inst;
}

/*
 * Stops playing event sound (SWFSound)
 *
 * see also SWFSoundInstance, SWFSound, SWFMovie_startSound
 */
void
SWFMovie_stopSound(SWFMovie movie, SWFSound sound)
{
	SWFSoundInstance inst = newSWFSoundInstance_stop(sound);

	// XXX - ???
	if ( !SWFBlock_isDefined((SWFBlock)sound) )
		SWFMovie_addBlock(movie, (SWFBlock)sound);

	SWFMovie_addBlock(movie, (SWFBlock)inst);
}

/*
 * Add a new frame to the current movie.
 * This function adds a new frame to the current movie. All items added, removed
 * manipulated effect this frame and probably following frames.
 */
void
SWFMovie_nextFrame(SWFMovie movie)
{
	SWFDisplayList_writeBlocks(movie->displayList, movie->blockList);
	SWFMovie_addBlock(movie, newSWFShowFrameBlock());
}

/*
 * Add a label Frame
 * This function adds a labelFrame to the movie's current frame.
 */
void
SWFMovie_labelFrame(SWFMovie movie /* Movie to which the label is added */,
	const char *label /* name to use for the label */)
{
	SWFMovie_addBlock(movie, (SWFBlock)newSWFFrameLabelBlock(label));
}

/*
 * Add a named anchor Frame
 * This function adds a named anchor to the movie.
 */
void
SWFMovie_namedAnchor(SWFMovie movie /* Movie to which the anchor is added */,
	const char *label /* name to use for the anchor */)
{
	SWFMovie_addBlock(movie, (SWFBlock)newSWFNamedAnchorBlock(label));
}

SWFOutput
SWFMovie_toOutput(SWFMovie movie, int level)
{
	int swflength;
#if USE_ZLIB
	int status;
#endif
	SWFOutput header, tempbuffer=0, buffer, swfbuffer;
	SWFBlock lastBlock;
	unsigned long compresslength;

	lastBlock = SWFBlockList_getLastBlock(movie->blockList);
	if ( ! lastBlock || SWFBlock_getType(lastBlock) != SWF_END )
	{

		if ( movie->nExports > 0 )
			SWFMovie_writeExports(movie);

		if ( movie->metadata != NULL)
		{
			SWFMovie_addBlock(movie, (SWFBlock)movie->metadata);
			// do not destroy with movie if added as block
			movie->metadata = NULL;
		}


		/* Add a final SHOWFRAME tag if not already there */
		if ( ! lastBlock
			|| SWFBlock_getType(lastBlock) != SWF_SHOWFRAME )
		{
			SWFMovie_nextFrame(movie);
		}

		while ( movie->nFrames < movie->totalFrames )
			SWFMovie_nextFrame(movie);

		if(movie->symbolClass)
			SWFMovie_addBlock(movie, (SWFBlock)movie->symbolClass);

		if(movie->sceneData)
			SWFMovie_addBlock(movie, (SWFBlock)movie->sceneData);

		SWFMovie_addBlock(movie, newSWFEndBlock());
	}

	// add five for the setbackground block..
	swflength = SWFBlockList_completeBlocks(movie->blockList, movie->version);

	/* XXX - hack */
	SWFDisplayList_rewindSoundStream(movie->displayList);

	header = newSizedSWFOutput(23);

	SWFOutput_writeRect(header, movie->bounds);
	SWFOutput_writeUInt16(header, (int)floor(movie->rate*256));
	SWFOutput_writeUInt16(header, movie->nFrames);

	/* SWF >= 8: first block _must_ be SWF_FILEATTRIBUTES */ 
	if(movie->fattrs)
		writeSWFBlockToMethod((SWFBlock)movie->fattrs, SWFOutputMethod, header);

	if(movie->backgroundBlock)
		writeSWFBlockToMethod(movie->backgroundBlock, SWFOutputMethod, header);
	
	if(movie->limits)
		writeSWFBlockToMethod((SWFBlock)movie->limits, SWFOutputMethod, header);	
	SWFOutput_byteAlign(header);
	swflength += 8 + SWFOutput_getLength(header);

	// compression level check
#if USE_ZLIB
	if (level < -1) level = -1;
	if (level >  9) level = 9;
#else
	if ( level != -1 )
	{
		SWF_warn("No zlib support compiled in, "
			"cannot generate compressed output");
		level = -1; 
	}
#endif

	// reserve output buffer
	if(level >= 0)
	{	// a little bit more than the uncompressed data
		compresslength = swflength + (swflength/1000) + 15 + 1;
		swfbuffer    = newSizedSWFOutput( compresslength + 8 );
	}
	else
		swfbuffer    = newSizedSWFOutput( swflength );

	if (level >= 0) SWFOutput_writeUInt8 (swfbuffer, 'C');
	else SWFOutput_writeUInt8 (swfbuffer, 'F');
	SWFOutput_writeUInt8 (swfbuffer, 'W');
	SWFOutput_writeUInt8 (swfbuffer, 'S');

	SWFOutput_writeUInt8 (swfbuffer, movie->version);

	// Movie length
	SWFOutput_writeUInt32(swfbuffer, swflength);

	if(level >= 0)
		buffer = tempbuffer = newSizedSWFOutput( swflength - 8);
	else
		buffer = swfbuffer;

	SWFOutput_writeToMethod(header, SWFOutputMethod, buffer);

	destroySWFOutput(header);

	// fill swfbuffer with blocklist
	SWFBlockList_writeBlocksToMethod(movie->blockList, SWFOutputMethod, buffer);

#if USE_ZLIB
	if (level >= 0)
	{
		status = compress2 ( (Bytef*) SWFOutput_getBuffer(swfbuffer)+8, &compresslength, SWFOutput_getBuffer(tempbuffer), SWFOutput_getLength(tempbuffer), level);
		if (status == Z_OK) {
			SWFOutput_truncate(swfbuffer, compresslength+8);
			destroySWFOutput(tempbuffer);
		} else SWF_error("compression failed");
	}
#endif // ndef USE_ZLIB
	return swfbuffer;
}

/*
 * Oputput a movie with SWFByteOutputMethod
 * returns the number of bytes written.
 */
int
SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data)
{
	SWFOutput swfbuffer;
	int swflength;
	byte *buffer;
	int n;
	int level = SWF_compression;

	swfbuffer = SWFMovie_toOutput(movie, level);
	swflength = SWFOutput_getLength(swfbuffer);
	buffer = SWFOutput_getBuffer(swfbuffer);
	
	for(n = 0 ; n < swflength ; n++)
		method(*buffer++, data);
	destroySWFOutput(swfbuffer);
	return swflength;
}

/*
 * Save movie to file 
 * returns the number of bytes written. -1 on error.
 */
int
SWFMovie_save(SWFMovie movie, const char *filename)
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
 * Output movie to FILE stream 
 * returns the number of bytes written
 */
int
SWFMovie_output_to_stream(SWFMovie movie, FILE *fp)
{
    return SWFMovie_output(movie,fileOutputMethod,fp);
}


int
completeSWFImportCharacter(SWFBlock block)
{	block->type = SWF_UNUSEDBLOCK;
	return -2;
}

SWFImportBlock
SWFMovie_addImport(SWFMovie movie, const char *filename, const char *name, int id)
{	int n;
	SWFImportBlock imports;
	struct importitem *ip;
	char *p;

	for(n = 0 ; n < movie->nImports ; n++)
		if(!strcmp(movie->imports[n]->filename, filename))
			break;

	if(n == movie->nImports)
	{	movie->imports = (SWFImportBlock *) realloc(movie->imports,
			 (movie->nImports+1) * sizeof(SWFImportBlock));
		movie->imports[movie->nImports++] = newSWFImportBlock(filename);
	}
	imports = movie->imports[n];
	for(ip = (struct importitem *)&(imports->importlist) ; ip->next ; )
		ip = ip->next;
	ip = ip->next = (struct importitem *)malloc(sizeof(struct importitem));
	ip->next = NULL;
	ip->id = id;
	ip->name = p = (char *)malloc(strlen(name)+1);
	while((*p++ = *name++) != 0)
		;
	return movie->imports[n];
}

/* 
 * Import a character to the movie
 * Imports characters from an other movie. filename (URL) points to a SWF 
 * file with exported characters. 
 *
 * returns a SWFCharacter object
 */
SWFCharacter
SWFMovie_importCharacter(SWFMovie movie, 
                         const char *filename /* URL to movie */, 
                         const char *name /* idetifier of character */)
{
	SWFCharacter res;
	SWFImportBlock importer;
	int id;

	res = (SWFCharacter) malloc(sizeof(struct SWFCharacter_s));
	SWFCharacterInit(res);
	CHARACTERID(res) = id = ++SWF_gNumCharacters;
	BLOCK(res)->type = SWF_DEFINESPRITE;
	BLOCK(res)->writeBlock = NULL;
	BLOCK(res)->complete = completeSWFImportCharacter;
	BLOCK(res)->dtor = (destroySWFBlockMethod) destroySWFCharacter;
	importer = SWFMovie_addImport(movie, filename, name, id);
	SWFCharacter_addDependency(res, (SWFCharacter) importer);
	return res;
}

/* 
 * Import a font from an other SWFFile
 * see also SWFMovie_importCharacter
 * returns a SWFFontCharacter object
 */
SWFFontCharacter
SWFMovie_importFont(SWFMovie movie, const char *filename, const char *name)
{
	SWFFontCharacter res;
	SWFImportBlock importer;
	int id;

	res = newSWFDummyFontCharacter();
	id = CHARACTERID(res);
	importer = SWFMovie_addImport(movie, filename, name, id);
	SWFCharacter_addDependency((SWFCharacter) res, (SWFCharacter) importer);
	return res;
}


/* 
 * sets SWF network / fileaccess policy
 * if the flag is set to 0 a localy loaded movie will be unable to access the network 
 * but is allowed to access local files
 *
 * if the flag ist set to 1 a localy loaded movie will be unable to access local files
 * but is allowed to access the network
 *
 * see also http://www.adobe.com/devnet/flash/articles/fplayer8_security.html 
 * 
 * For SWF >= 8: default is 0
 */
void 
SWFMovie_setNetworkAccess(SWFMovie movie, int flag)
{
	if(!movie->fattrs)
		movie->fattrs = newSWFFileAttributes();

	SWFFileAttributes_useNetwork(movie->fattrs, flag);
}

/*
 * adds Metadata to the movie
 * This function inserts a metadata-tag into the movie. Only one metadata-tag can
 * be set to a movie. 
 * Metadata is specified as an XML string. For more details see:
 * http://www.adobe.com/products/xmp
 */
void
SWFMovie_addMetadata(SWFMovie movie, const char *xml)
{
	if(!movie->fattrs)
		movie->fattrs = newSWFFileAttributes();

	SWFFileAttributes_hasMetadata(movie->fattrs, 1);
	
	if(movie->metadata)
		destroySWFMetadata(movie->metadata);
	movie->metadata = newSWFMetadata(xml);
}

/*
 * modifies scruipt limits
 * default recursion depth is 265
 * default timeout is 15-20 sec
 */
void 
SWFMovie_setScriptLimits(SWFMovie movie, 
                         int maxRecursion /* max recursion */,
                         int timeout /* timeout in sec */)
{
	if(!movie->limits)
		movie->limits = newSWFScriptLimits();

	SWFScriptLimits_maxRecursion(movie->limits, maxRecursion);
	SWFScriptLimits_setTimeout(movie->limits, timeout);
}

/*
 * set tabindex for specified depth level
 */
void 
SWFMovie_setTabIndex(SWFMovie movie,
                     int depth /* depth level */,
                     int index /* index */)
{
	SWFTabIndex ti = newSWFTabIndex(depth, index);
	SWFMovie_addBlock(movie, (SWFBlock) ti);
}

/* 
 * assigns a symbolic name for a SWFCharacter
 * Such classes are available for usage in other SWF files
 * and can be referenced from inside the current movie.
 *
 * To assign a symbol to the root movie use NULL as character
 * value.
 */
void 
SWFMovie_assignSymbol(SWFMovie m, 
                      SWFCharacter character /* target */,
                      const char *name /* symbol */)
{
	if(m->symbolClass == NULL)
		m->symbolClass = newSWFSymbolClass();
	SWFSymbolClass_addSymbol(m->symbolClass, character, name);
}

/*
 * define scenes for the movies' main timeline
 */
void
SWFMovie_defineScene(SWFMovie m, unsigned int offset /* frame offset */, 
                  const char *name /* name of the scene */)
{
	if(m->sceneData == NULL)
		m->sceneData = newSWFSceneData();
	SWFSceneData_addScene(m->sceneData, offset, name);
}
/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */

