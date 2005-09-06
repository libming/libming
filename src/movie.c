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
#include <string.h>
#include <math.h>
#include <zlib.h>

#include "ming.h"

#include "movie.h"
#include "shape_util.h"
#include "blocklist.h"
#include "displaylist.h"

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
#include "libming.h"


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
	byte r;
	byte g;
	byte b;

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

	if (movie->imports)
		free(movie->imports);

#if TRACK_ALLOCS
	ming_gc_remove_node(movie->gcnode);
#endif

	free(movie);
}


SWFMovie
newSWFMovieWithVersion(int version)
{
	SWFMovie movie = (SWFMovie) malloc(sizeof(struct SWFMovie_s));

	movie->version = version;
	movie->blockList = newSWFBlockList();
	movie->displayList = newSWFDisplayList();
	movie->bounds = newSWFRect(0, 320*20, 0, 240*20);
	movie->rate = 12.0;
	movie->totalFrames = 1;
	movie->nFrames = 0;

	movie->nExports = 0;
	movie->exports = NULL;

	movie->nImports = 0;
	movie->imports = NULL;

	movie->nFonts = 0;
	movie->fonts = NULL;

	movie->r = 0xff;
	movie->g = 0xff;
	movie->b = 0xff;

#if TRACK_ALLOCS
	movie->gcnode = ming_gc_add_node(movie, (dtorfunctype) destroySWFMovie);
#endif

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


void
SWFMovie_addExport(SWFMovie movie, SWFBlock block, const char *name)
{
	switch( SWFBlock_getType(block))
	{	case SWF_DEFINESPRITE:
		case SWF_DEFINEFONT2:
			movie->exports = (struct SWFExport_s*)realloc(movie->exports,
														 (movie->nExports+1) * sizeof(struct SWFExport_s));

			movie->exports[movie->nExports].block = block;
			movie->exports[movie->nExports].name = strdup(name);
			++movie->nExports;
		default:
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

		return SWFDisplayList_add(movie->displayList, (SWFCharacter)block);
	}
	else
		SWFMovie_addBlock(movie, block);

	return NULL;
}


void
SWFMovie_remove(SWFMovie movie , SWFDisplayItem item)
{
	SWFDisplayItem_remove(item);
}

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

void
SWFMovie_setSoundStream(SWFMovie movie, SWFSoundStream stream)
{
	SWFMovie_setSoundStreamAt(movie, stream, 0);
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
SWFMovie_labelFrame(SWFMovie movie, const char *label)
{
	SWFMovie_addBlock(movie, (SWFBlock)newSWFFrameLabelBlock(label));
}

void
SWFMovie_namedAnchor(SWFMovie movie, const char *label)
{
	SWFMovie_addBlock(movie, (SWFBlock)newSWFNamedAnchorBlock(label));
}

/* old outputfunction without possible compression */
/*
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

	
	//hack
        SWFDisplayList_rewindSoundStream(movie->displayList);

        header = newSizedSWFOutput(20);

        // figure size, write header

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
*/

SWFOutput
SWFMovie_toOutput(SWFMovie movie, int level)
{
	int swflength, status;
	SWFOutput header, tempbuffer=0, buffer, swfbuffer;
	SWFBlock backgroundBlock;
	unsigned long compresslength;

	if ( movie->nExports > 0 )
		SWFMovie_writeExports(movie);

	while ( movie->nFrames < movie->totalFrames )
		SWFMovie_nextFrame(movie);

	SWFMovie_addBlock(movie, newSWFEndBlock());

	// add five for the setbackground block..
	swflength = SWFBlockList_completeBlocks(movie->blockList);

	/* XXX - hack */
	SWFDisplayList_rewindSoundStream(movie->displayList);

	header = newSizedSWFOutput(23);

	SWFOutput_writeRect(header, movie->bounds);
	SWFOutput_writeUInt16(header, (int)floor(movie->rate*256));
	SWFOutput_writeUInt16(header, movie->nFrames);

	backgroundBlock = (SWFBlock)newSWFSetBackgroundBlock(movie->r, movie->g, movie->b);
        writeSWFBlockToMethod(backgroundBlock, SWFOutputMethod, header);
	destroySWFBlock(backgroundBlock);
	
	SWFOutput_byteAlign(header);
	swflength += 8 + SWFOutput_getLength(header);

	// compression level check
	if (level < -1) level = -1;
	if (level >  9) level = 9;

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

	if (level >= 0)
	{
		status = compress2 ( (Bytef*) SWFOutput_getBuffer(swfbuffer)+8, &compresslength, SWFOutput_getBuffer(tempbuffer), SWFOutput_getLength(tempbuffer), level);
		if (status == Z_OK) {
			SWFOutput_truncate(swfbuffer, compresslength+8);
			destroySWFOutput(tempbuffer);
		} else SWF_error("compression failed");
	}
	return swfbuffer;
}

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

SWFCharacter
SWFMovie_importCharacter(SWFMovie movie, const char *filename, const char *name)
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
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */

