/*
		Ming, an SWF output library
		Copyright (C) 2002	Opaque Industries - http://www.opaque.net/

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA
*/

/* $Id$ */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "font.h"
#include "method.h"
#include "utf8.h"
#include "character.h"
#include "error.h"
#include "movie.h"
#include "libming.h"


// #define HAS_MMAP 1

#ifdef HAS_MMAP
	#include <unistd.h>
	#include <sys/mman.h>
#endif


struct kernInfo
{
	byte code1;
	byte code2;
	short adjustment;
};

struct kernInfo16
{
	unsigned short code1;
	unsigned short code2;
	short adjustment;
};

struct textList
{
	struct textList* next;
	SWFTextRecord text;
};

struct SWFFontCharacter_s
{
	struct SWFCharacter_s character;

	SWFFont font;
	byte flags;

	// list of text records that reference this font
	struct textList* textList;
	struct textList* currentList;

	// list of chars used in text objects that reference this font-
	// idx into this table is stored in text records
	int nGlyphs;
	unsigned short* codeTable;
};


#define glyphLength(font,glyph) \
	((font)->glyphOffset[(glyph)+1] - (font)->glyphOffset[(glyph)])


static void
SWFFontCharacter_resolveTextCodes(SWFFontCharacter font);


int
completeSWFFontCharacter(SWFBlock block)
{
	SWFFontCharacter inst = (SWFFontCharacter)block;
	SWFFont font = inst->font;
	int size = 0;
	int i, nbits;

	// get charcodes from text records, translate into local code map
	SWFFontCharacter_resolveTextCodes(inst);

	size += 2; // character id
	size += 2; // flags
	size += 1; // name length
	size += strlen((const char*)font->name); // font name
	size += 2; // nglyphs
	size += 2 * (inst->nGlyphs + 1); // offset table

	/* get length of each glyph from its output buffer */

	for ( i=0; i<inst->nGlyphs; ++i )
		size += glyphLength(font, inst->codeTable[i]);

	if ( font->flags & SWF_FONT_WIDECODES )
		size += 2 * inst->nGlyphs;
	else
		size += inst->nGlyphs;

	if ( size > 65500 )
	{
		size += 2 * (inst->nGlyphs+1);
		inst->flags |= SWF_FONT_WIDEOFFSETS;
	}

	if( inst->flags & SWF_FONT_HASLAYOUT )
	{	
		size += 8;	// ascent, descent, leading, 0 kern pairs
		size += 2 * inst->nGlyphs;	// width table
		for(i = 0 ; i < inst->nGlyphs ; i++)
		{
			nbits = SWFRect_numBits(font->bounds + inst->codeTable[i]);
			size += (nbits+7) >> 3;
		}
	}
	return size;
}


void
writeSWFFontCharacterToMethod(SWFBlock block,
															SWFByteOutputMethod method, void *data)
{
	SWFFontCharacter inst = (SWFFontCharacter)block;
	SWFFont font = inst->font;
	SWFOutput bounds;
	int offset, i;
	byte *p, *s;

	methodWriteUInt16(CHARACTERID(inst), method, data);

	method(inst->flags, data); /* main flags */
	method(0, data);																					/* more flags */
	method((unsigned char)strlen((const char*)font->name), data);

	for ( p = font->name; *p != '\0'; ++p )
		method(*p, data);

	methodWriteUInt16(inst->nGlyphs, method, data);

	offset = (inst->nGlyphs+1) * (inst->flags & SWF_FONT_WIDEOFFSETS ? 4 : 2);

	/* write offset table for glyphs */

	for ( i=0; i<=inst->nGlyphs; ++i )
	{
		if ( inst->flags & SWF_FONT_WIDEOFFSETS )
			methodWriteUInt32(offset, method, data);
		else
			methodWriteUInt16(offset, method, data);

		if ( i < inst->nGlyphs )
			offset += glyphLength(font, inst->codeTable[i]);
	}

	/* write shape records for glyphs */

	for ( i=0; i<inst->nGlyphs; ++i )
	{
		p = font->glyphOffset[inst->codeTable[i]];
		s = font->glyphOffset[inst->codeTable[i]+1];

		SWF_assert(p < s);

		while(p < s)
			method(*(p++), data);
	}
	
	/* write glyph to code map */

	if ( inst->flags & SWF_FONT_WIDECODES )
	{
		for ( i=0; i<inst->nGlyphs; ++i )
			methodWriteUInt16(font->glyphToCode[inst->codeTable[i]], method, data);
	}
	else
	{
		for ( i=0; i<inst->nGlyphs; ++i )
			method((byte)font->glyphToCode[inst->codeTable[i]], data);
	}

	/* write font layout */
	if (inst->flags & SWF_FONT_HASLAYOUT )
	{	methodWriteUInt16(font->ascent, method, data);
		methodWriteUInt16(font->descent, method, data);
		methodWriteUInt16(font->leading, method, data);
		for ( i=0; i<inst->nGlyphs; ++i )
			methodWriteUInt16(font->advances[inst->codeTable[i]], method, data);
		bounds = newSWFOutput();
		for ( i=0; i<inst->nGlyphs; ++i )
		{	SWFOutput_writeRect(bounds, font->bounds + inst->codeTable[i]);
			SWFOutput_byteAlign(bounds);
		}
		SWFOutput_writeToMethod(bounds, method, data);
		destroySWFOutput(bounds);
		methodWriteUInt16(0, method, data);		/* no kerning */
	}
			
}


void
destroySWFFont(SWFFont font)
{	// should not happen but seemingly does
	if (!font)
		return;

	if ( font->shapes != NULL )
	{
#if HAS_MMAP
		// XXX - if mmap failed, we malloced this!
		int len = font->glyphOffset[font->nGlyphs] - font->glyphOffset[0];
		munmap(font->shapes, len);
#else
		free(font->shapes);
#endif
	}

	if ( font->flags & SWF_FONT_WIDECODES )
	{
		if ( font->codeToGlyph.wideMap != NULL )
		{
			int i;

			for ( i = 0; i < 256; ++i )
			{
				if ( font->codeToGlyph.wideMap[i] != NULL )
					free(font->codeToGlyph.wideMap[i]);
			}

			free(font->codeToGlyph.wideMap);
		}
	}
	else
	{
		if ( font->codeToGlyph.charMap != NULL )
			free(font->codeToGlyph.charMap);
	}

	if ( font->bounds != NULL )
		free(font->bounds);

	if ( font->name != NULL )
		free(font->name);

	if ( font->kernTable.k != NULL )	// union of pointers ...
		free(font->kernTable.k);

	if ( font->glyphOffset != NULL )
		free(font->glyphOffset);

	if ( font->glyphToCode != NULL )
		free(font->glyphToCode);

	if ( font->advances != NULL )
		free(font->advances);

	free(font);
}


void
destroySWFFontCharacter(SWFFontCharacter font)
{
	struct textList* text = font->textList;

	while ( text != NULL )
	{
		struct textList* next = text->next;
		free(text);
		text = next;
	}

	if ( font->codeTable != NULL )
		free(font->codeTable);

	free(font);
}


SWFFont
newSWFFont()
{
	SWFFont font = (SWFFont) malloc(sizeof(struct SWFFont_s));

	SWFBlockInit((SWFBlock)font);

	BLOCK(font)->type = SWF_UNUSEDBLOCK;
	BLOCK(font)->writeBlock = NULL;
	BLOCK(font)->complete = NULL;
	BLOCK(font)->dtor = (destroySWFBlockMethod) destroySWFFont;

	font->name = NULL;
	font->flags = 0;

	font->nGlyphs = 0;
	font->glyphToCode = NULL;
	font->codeToGlyph.charMap = NULL;
	font->glyphOffset = NULL;
	font->advances = NULL;
	font->bounds = NULL;

	font->ascent = 0;
	font->descent = 0;
	font->leading = 0;

	font->kernCount = 0;
	font->kernTable.k = NULL;

	font->shapes = NULL;

	return font;
}


SWFFontCharacter
newSWFFontCharacter(SWFFont font)
{
	SWFFontCharacter inst = (SWFFontCharacter) malloc(sizeof(struct SWFFontCharacter_s));

	SWFCharacterInit((SWFCharacter)inst);

	BLOCK(inst)->type = SWF_DEFINEFONT2;
	BLOCK(inst)->writeBlock = writeSWFFontCharacterToMethod;
	BLOCK(inst)->complete = completeSWFFontCharacter;
	BLOCK(inst)->dtor = (destroySWFBlockMethod) destroySWFFontCharacter;

	CHARACTERID(inst) = ++SWF_gNumCharacters;

	inst->font = font;
	inst->flags = (unsigned char)(font->flags & /*(~SWF_FONT_HASLAYOUT) &*/ (~SWF_FONT_WIDEOFFSETS));

	inst->nGlyphs = 0;
	inst->codeTable = NULL;

	inst->textList = NULL;
	inst->currentList = NULL;

	return inst;
}

SWFFontCharacter
newSWFDummyFontCharacter()
{	SWFFontCharacter ret = (SWFFontCharacter) malloc(sizeof (struct SWFFontCharacter_s));
	SWFCharacterInit((SWFCharacter) ret);
	BLOCK(ret)->type = SWF_DEFINEFONT;
	BLOCK(ret)->complete = completeSWFImportCharacter;
	BLOCK(ret)->writeBlock = NULL;
	BLOCK(ret)->dtor = NULL;
	CHARACTERID(ret) = ++SWF_gNumCharacters;
	
	ret->flags = SWF_FONT_HASLAYOUT;
	ret->nGlyphs = 1;
	ret->codeTable = NULL;
	
	return ret;
}
	
static void
SWFFont_buildReverseMapping(SWFFont font)
{
	int i;

	if ( font->flags & SWF_FONT_WIDECODES )
	{
		font->codeToGlyph.wideMap = (unsigned short**)malloc(256 * sizeof(unsigned short*));

		for ( i=0; i<256; ++i )
			font->codeToGlyph.wideMap[i] = NULL;

		for ( i=0; i<font->nGlyphs; ++i )
		{
			unsigned short charcode = font->glyphToCode[i];
			byte high = charcode >> 8;
			byte low = charcode & 0xff;

			if ( font->codeToGlyph.wideMap[high] == NULL )
			{
				font->codeToGlyph.wideMap[high] = (unsigned short*)malloc(256 * sizeof(unsigned short));
				memset(font->codeToGlyph.wideMap[high], 0, 256 * sizeof(unsigned short));
			}

			font->codeToGlyph.wideMap[high][low] = i;
		}
	}
	else
	{
		font->codeToGlyph.charMap = (byte*) malloc(256 * sizeof(char));
		memset(font->codeToGlyph.charMap, 0, 256 * sizeof(char));

		for ( i=0; i<font->nGlyphs; ++i )
			font->codeToGlyph.charMap[font->glyphToCode[i]] = i;
	}
}


static int
SWFFont_findGlyphCode(SWFFont font, unsigned short c)
{
	if ( font->flags & SWF_FONT_WIDECODES )
	{
		byte high = c >> 8;
		byte low = c & 0xff;

		if ( font->codeToGlyph.wideMap[high] != NULL )
			return font->codeToGlyph.wideMap[high][low];
		else
			return -1;
	}
	else
	{
		if ( (c & 0xff00) == 0 )
			return font->codeToGlyph.charMap[(byte)c];
		else
			return -1;
	}
}


byte*
SWFFont_findGlyph(SWFFont font, unsigned short c)
{
	int code = SWFFont_findGlyphCode(font, c);

	if ( code >= 0 )
		return font->glyphOffset[code];

	return NULL;
}


void
SWFFontCharacter_addTextToList(SWFFontCharacter font, SWFTextRecord text)
{
	struct textList* textList = (struct textList* )malloc(sizeof(struct textList));

	textList->next = font->textList;
	textList->text = text;

	font->textList = textList;
}


static int
findCodeValue(unsigned short c, unsigned short* list, int start, int end)
{
	int pos;

	if ( start == end )
		return start;

	if ( c <= list[start] )
		return start;

	pos = (start + end) / 2;

	if ( c < list[pos] )
		return findCodeValue(c, list, start, pos);
	else if ( c > list[pos] )
		return findCodeValue(c, list, pos+1, end);
	else
		return pos;
}


#define CODETABLE_INCREMENT 32

void
SWFFontCharacter_addCharToTable(SWFFontCharacter font, unsigned short c)
{
	// insert the char into font's codeTable if it isn't already there

	int p;

	p = findCodeValue(c, font->codeTable, 0, font->nGlyphs);

	if ( font->codeTable != NULL && font->codeTable[p] == c )
		return;

	if ( font->nGlyphs % CODETABLE_INCREMENT == 0 )
	{
		font->codeTable = (unsigned short*) realloc(font->codeTable,
			(font->nGlyphs + CODETABLE_INCREMENT) *
				sizeof(unsigned short));
		memset(font->codeTable+font->nGlyphs, 0,
			CODETABLE_INCREMENT*sizeof(unsigned short));
	}

	// keep list sorted

	if ( p < font->nGlyphs )
	{
		memmove(&font->codeTable[p + 1], &font->codeTable[p],
						(font->nGlyphs - p) * sizeof(*font->codeTable));
	}

	font->codeTable[p] = c;
	++font->nGlyphs;
}

void
SWFFontCharacter_addWideChars(SWFFontCharacter font, unsigned short *string, int len)
{
	while(--len >= 0)
		SWFFontCharacter_addCharToTable(font, *string++);
}

void
SWFFontCharacter_addUTF8Chars(SWFFontCharacter font, const char *string)
{
	unsigned short *widestring;
	int len;
	len = UTF8ExpandString(string, &widestring);
	SWFFontCharacter_addWideChars(font, widestring, len);
	free(widestring);
}

void
SWFFontCharacter_addChars(SWFFontCharacter font, const char *string)
{
	while(*string)
		SWFFontCharacter_addCharToTable(font, *string++ & 0xff);
}

SWFFont
SWFFontCharacter_getFont(SWFFontCharacter font)
{
	return font->font;
}


void
SWFFontCharacter_exportCharacterRange(SWFFontCharacter font,
						 unsigned short start, unsigned short end)
{
	// insert the char into font's codeTable if it isn't already there

	//int p = findCodeValue(start, font->codeTable, 0, font->nGlyphs);
	//int q = findCodeValue(end, font->codeTable, 0, font->nGlyphs);
	
	// XXX
}


static int
SWFFontCharacter_findCharCode(SWFFontCharacter font, unsigned short c)
{
	// return the index in font->codeTable for the given character

	int p = findCodeValue(c, font->codeTable, 0, font->nGlyphs);

	if ( font->codeTable[p] == c )
		return p;

	return -1;
}


static void
SWFFontCharacter_resolveTextCodes(SWFFontCharacter font)
{
	struct textList* text = font->textList;
	unsigned short* string;
	int len, i;

	// find out which characters from this font are being used

	while ( text != NULL )
	{
		len = SWFTextRecord_getString(text->text, &string);

		for ( i=0; i<len; ++i )
			SWFFontCharacter_addCharToTable(font, string[i]);

		text = text->next;
	}

	// translate text records' strings into lists of font char codes

	text = font->textList;

	while ( text != NULL )
	{
		len = SWFTextRecord_getString(text->text, &string);

		for ( i=0; i<len; ++i )
		{
			int code = SWFFontCharacter_findCharCode(font, string[i]);

			if ( code >= 0 )
				string[i] = (unsigned short)code;

			// XXX - else?
		}

		text = text->next;
	}

	// translate codeTable's char codes into glyph codes

	for ( i=0; i<font->nGlyphs; ++i )
	{
		// XXX - if font doesn't have the character?
		font->codeTable[i] = SWFFont_findGlyphCode(font->font, font->codeTable[i]);
	}
}


const char*
SWFFont_getName(SWFFont font)
{
	return (const char*) font->name;
}


byte
SWFFont_getFlags(SWFFont font)
{
	return font->flags;
}


int
SWFFontCharacter_getNGlyphs(SWFFontCharacter font)
{
	return font->nGlyphs;
}


int
SWFFont_getScaledWideStringWidth(SWFFont font,
														 const unsigned short* string, int len)
{
	/* return length of given string in whatever units these are we're using */

	int i, j;
	int width = 0;
	int glyph, glyph2;

	for ( i=0; i<len; ++i )
	{
		glyph = SWFFont_findGlyphCode(font, string[i]);

		if ( glyph == -1 )
			continue; // XXX - ???

		if ( font->advances )
			width += font->advances[glyph];

		// looking in kernTable
		// XXX - kernTable should be sorted to make this faster

		if ( i < len-1 && font->kernTable.k )
		{
			glyph2 = SWFFont_findGlyphCode(font, string[i+1]);

			if ( glyph2 == -1 )
				continue; // XXX - ???

			j = font->kernCount;

			if(font->flags & SWF_FONT_WIDECODES)
				while ( --j >= 0 )
				{
					if ( glyph == font->kernTable.w[j].code1 &&
							 glyph2 == font->kernTable.w[j].code2 )
					{
						width += font->kernTable.w[j].adjustment;
						break;
					}
				}
			else
				while ( --j >= 0 )
				{
					if ( glyph == font->kernTable.k[j].code1 &&
							 glyph2 == font->kernTable.k[j].code2 )
					{
						width += font->kernTable.k[j].adjustment;
						break;
					}
				}
		}
	}

	return width;
}


int
SWFFont_getScaledStringWidth(SWFFont font, const char* string)
{
	unsigned short* widestr;
	int len = strlen(string);
	int n, width;
	widestr = (unsigned short*) malloc(2 * len);
	for(n = 0 ; n < len ; n++)
		widestr[n] = (unsigned char)string[n];
	width = SWFFont_getScaledWideStringWidth(font, widestr, len);
	free(widestr);

	return width;
}

int
SWFFont_getScaledUTF8StringWidth(SWFFont font, const char* string)
{
	unsigned short* widestr;
	int len = UTF8ExpandString(string, &widestr);
	int width = SWFFont_getScaledWideStringWidth(font, widestr, len);

	free(widestr);

	return width;
}


/* get some font metrics */
short
SWFFont_getScaledAscent(SWFFont font)
{
	return font->ascent;
}


short
SWFFont_getScaledDescent(SWFFont font)
{
	return font->descent;
}


short
SWFFont_getScaledLeading(SWFFont font)
{
	return font->leading;
}


unsigned short
SWFFontCharacter_getGlyphCode(SWFFontCharacter font, unsigned short c)
{
	return font->codeTable[c];
}


SWFRect
SWFFont_getGlyphBounds(SWFFont font, unsigned short glyphcode)
{
	if ( glyphcode >= font->nGlyphs )
		SWF_error("SWFFont_getGlyphBounds: glyphcode >= nGlyphs");

	return &font->bounds[glyphcode];
}


int
SWFFont_getCharacterAdvance(SWFFont font, unsigned short glyphcode)
{
	if ( font->advances )
	{
		if ( glyphcode >= font->nGlyphs )
			SWF_error("SWFFont_getCharacterAdvance: glyphcode >= nGlyphs");

		return font->advances[glyphcode];
	}

	return 0;
}


int
SWFFont_getCharacterKern(SWFFont font,
												 unsigned short code1, unsigned short code2)
{
	int j = font->kernCount;

	if( !font->kernTable.k )
		return 0;

	if ( code1 >= font->nGlyphs || code2 >= font->nGlyphs )
		SWF_error("SWFFont_getCharacterKern: glyphcode >= nGlyphs");

	// XXX - kernTable should be sorted to make this faster

	if(font->flags & SWF_FONT_WIDECODES)
		while ( --j >= 0 )
		{
			if ( code1 == font->kernTable.w[j].code1 &&
					 code2 == font->kernTable.w[j].code2 )
			{
				return font->kernTable.w[j].adjustment;
			}
		}
	else
		while ( --j >= 0 )
		{
			if ( code1 == font->kernTable.k[j].code1 &&
					 code2 == font->kernTable.k[j].code2 )
			{
				return font->kernTable.k[j].adjustment;
			}
		}

	return 0;
}


/* font loader */

#define SWF_LOAD_FONT_HASLAYOUT		 (1<<7)
#define SWF_LOAD_FONT_SHIFTJIS		 (1<<6)
#define SWF_LOAD_FONT_UNICODE			 (1<<5)
#define SWF_LOAD_FONT_ANSI				 (1<<4)
#define SWF_LOAD_FONT_WIDEOFFSETS	 (1<<3)
#define SWF_LOAD_FONT_WIDECODES		 (1<<2)
#define SWF_LOAD_FONT_ITALIC			 (1<<1)
#define SWF_LOAD_FONT_BOLD				 (1<<0)


static int
readUInt16(FILE *f)
{
	int low = fgetc(f);
	int high = fgetc(f);
	return low + (high<<8);
}


static int
readSInt16(FILE *f)
{
	int low = fgetc(f);
	int high = fgetc(f);
	return low + high*256;
}


static unsigned long
readUInt32(FILE *f)
{
	return (unsigned long)(fgetc(f) + (fgetc(f)<<8) + (fgetc(f)<<16) + (fgetc(f)<<24));
}


static int buffer;
static int bufbits = 0; /* # of bits in buffer */

static void
byteAlign()
{
	if ( bufbits > 0 )
	{
		bufbits = 0;
		buffer = 0;
	}
}


static int
readBits(FILE *f, int number)
{
	int ret = buffer;

	if ( number == bufbits )
	{
		bufbits = 0;
		buffer = 0;
		return ret;
	}

	if ( number > bufbits )
	{
		number -= bufbits;

		while( number > 8 )
		{
			ret <<= 8;
			ret += fgetc(f);
			number -= 8;
		}

		buffer = fgetc(f);

		if ( number > 0 )
		{
			ret <<= number;
			bufbits = 8-number;
			ret += buffer >> (8-number);
			buffer &= (1<<bufbits)-1;
		}

		return ret;
	}

	ret = buffer >> (bufbits-number);
	bufbits -= number;
	buffer &= (1<<bufbits)-1;

	return ret;
}


static int
readSBits(FILE *f, int number)
{
	int num = readBits(f, number);

	if ( num & (1<<(number-1)) )
		return num - (1<<number);
	else
		return num;
}


static void
readBounds(FILE *file, struct SWFRect_s* bounds)
{
	int nBits;

	byteAlign();

	nBits = readBits(file, 5);
	bounds->minX = readSBits(file, nBits);
	bounds->maxX = readSBits(file, nBits);
	bounds->minY = readSBits(file, nBits);
	bounds->maxY = readSBits(file, nBits);
}


static void
readKernInfo(FILE *file, struct kernInfo *kern)
{
	kern->code1 = fgetc(file);
	kern->code2 = fgetc(file);
	kern->adjustment = readSInt16(file);
}

static void
readKernInfo16(FILE *file, struct kernInfo16 *kern)
{
	kern->code1 = readUInt16(file);
	kern->code2 = readUInt16(file);
	kern->adjustment = readSInt16(file);
}


SWFFont
loadSWFFontFromFile(FILE *file)
{
	/* pull font definition from fdb (font def block) file */

	SWFFont font = newSWFFont();
	int namelen, flags, i, nGlyphs;
	int shapeTableLen;
	byte *p;

	if ( file == NULL )
		return NULL;

	fgetc(file); /* header whatever */
	fgetc(file);
	fgetc(file);
	fgetc(file);

	flags = fgetc(file);

/* this isn't right, and I don't know why.. */

	if(flags & SWF_LOAD_FONT_HASLAYOUT)
		font->flags |= SWF_FONT_HASLAYOUT;
	if(flags & SWF_LOAD_FONT_SHIFTJIS)
		font->flags |= SWF_FONT_SHIFTJIS;
	if(flags & SWF_LOAD_FONT_ANSI)
		font->flags |= SWF_FONT_ANSI;
	if(flags & SWF_LOAD_FONT_UNICODE)
		font->flags |= SWF_FONT_UNICODE;
	if(flags & SWF_LOAD_FONT_ITALIC)
		font->flags |= SWF_FONT_ISITALIC;
	if(flags & SWF_LOAD_FONT_BOLD)
		font->flags |= SWF_FONT_ISBOLD;
	if(flags & SWF_LOAD_FONT_WIDEOFFSETS)
		font->flags |= SWF_FONT_WIDEOFFSETS;
	if(flags & SWF_LOAD_FONT_WIDECODES)
		font->flags |= SWF_FONT_WIDECODES;

//	font->flags = flags; // XXX - ???
// new rules... SWF6 said to require unicode, no ansi, no shiftjis
//	font->flags |= SWF_FONT_SHIFTJIS;

	fgetc(file); /* "reserved" */

	namelen = fgetc(file);
	font->name = (byte*) malloc(namelen+1);

	for ( i=0; i<namelen; ++i )
		font->name[i] = fgetc(file);

	font->name[namelen] = '\0';

	nGlyphs = readUInt16(file);

	font->nGlyphs = nGlyphs;

	font->bounds = (struct SWFRect_s*) malloc(nGlyphs * sizeof(struct SWFRect_s));
	font->glyphOffset = (byte**)malloc((nGlyphs + 1) * sizeof(*font->glyphOffset));
	font->glyphToCode = (unsigned short*)malloc(nGlyphs * sizeof(*font->glyphToCode));
	font->advances = (short*) malloc(nGlyphs * sizeof(*font->advances));

	if ( flags & SWF_LOAD_FONT_WIDEOFFSETS )
	{
		for ( i=0; i<=nGlyphs; ++i )
			font->glyphOffset[i] = (byte *)(readUInt32(file) - 4*nGlyphs - 4);
	}
	else
	{
		for(i=0; i<=nGlyphs; ++i)
			font->glyphOffset[i] = (byte *)(readUInt16(file) - 2*nGlyphs - 2);
	}

	shapeTableLen = font->glyphOffset[nGlyphs] - font->glyphOffset[0];

#if HAS_MMAP
	font->shapes =
		mmap(NULL, shapeTableLen + 1, PROT_READ, MAP_PRIVATE,
				 fileno(file), ftell(file));

	if ( (void*)font->shapes != MAP_FAILED )
	{
		fseek(file, shapeTableLen, SEEK_CUR);
	}
	else
	{
#endif

	/* it helps to allocate the right amount. (thanks, Tim!) */
	font->shapes = (byte*)malloc(font->glyphOffset[nGlyphs] - font->glyphOffset[0] + 1);

	p = font->shapes;

	while ( shapeTableLen > 0 )
	{
		int count = fread(p, 1, shapeTableLen, file);
		shapeTableLen -= count;
		p += count;
	}

#if HAS_MMAP
  }
#endif

	/* adjust offset table to point to shapes */

	for ( i=0; i<=nGlyphs; ++i )
		font->glyphOffset[i] += (unsigned long)font->shapes;

	/* read glyph-to-code table */

	if ( flags & SWF_LOAD_FONT_WIDECODES )
	{
		for ( i=0; i<nGlyphs; ++i )
			font->glyphToCode[i] = readUInt16(file);
	}
	else
	{
		for ( i=0; i<nGlyphs; ++i )
			font->glyphToCode[i] = fgetc(file);
	}

	// build code-to-glyph table
	// XXX - make new version of FDB with reverse map?
	SWFFont_buildReverseMapping(font);

	/* read metrics */

	if ( flags & SWF_LOAD_FONT_HASLAYOUT )
	{
		font->ascent = readSInt16(file);
		font->descent = readSInt16(file);
		font->leading = readSInt16(file);

		/* get advances */
		for ( i=0; i<nGlyphs; ++i )
			font->advances[i] = readSInt16(file);

		/* get bounds */
		for ( i=0; i<nGlyphs; ++i )
			readBounds(file, &font->bounds[i]);

		/* get kern table */
		font->kernCount = readUInt16(file);

		if ( font->kernCount > 0 )
			if(font->flags & SWF_FONT_WIDECODES)
				font->kernTable.w = (struct kernInfo16*) malloc(sizeof(struct kernInfo16) * font->kernCount);
			else
				font->kernTable.k = (struct kernInfo*)malloc(sizeof(struct kernInfo) * font->kernCount);
		else
			font->kernTable.k = NULL;

		if(font->flags & SWF_FONT_WIDECODES)
			for ( i=0; i<font->kernCount; ++i )
				readKernInfo16(file, &(font->kernTable.w[i]));
		else
			for ( i=0; i<font->kernCount; ++i )
				readKernInfo(file, &(font->kernTable.k[i]));
	}

	return font;
}



/* retrieve glyph shape of a font */
#include <stdio.h>
#include <stdarg.h>
/* basic IO */

struct out
{	char *buf, *ptr;
	int len;
};

static void oprintf(struct out *op, const char *fmt, ...)
{	va_list ap;
	char buf[256];
	int d, l;
	
	va_start(ap, fmt);
	l = vsprintf(buf, fmt, ap);
	while((d = op->ptr - op->buf) + l >= op->len-1)
	{	op->buf = (char *) realloc(op->buf, op->len += 100);
		op->ptr = op->buf + d;
	}
	for(d = 0 ; d < l ; d++)
		*op->ptr++ = buf[d];
}

static int readBitsP(byte **p, int number)
{
	byte *ptr = *p;
	int ret = buffer;

	if(number == bufbits)
	{
		ret = buffer;
		bufbits = 0;
		buffer = 0;
	}
	else if(number > bufbits)
	{
		number -= bufbits;

		while(number>8)
		{
			ret <<= 8;
			ret += *ptr++;
			number -= 8;
		}

		buffer = *ptr++;

		if(number>0)
		{
			ret <<= number;
			bufbits = 8-number;
			ret += buffer >> (8-number);
			buffer &= (1<<bufbits)-1;
		}
	}
	else
	{
		ret = buffer >> (bufbits-number);
		bufbits -= number;
		buffer &= (1<<bufbits)-1;
	}

	*p = ptr;
	return ret;
}

static int readSBitsP(byte **p, int number)
{
	int num = readBitsP(p, number);

	if(num & (1<<(number-1)))
		return num - (1<<number);
	else
		return num;
}

// return a malloc'ed string describing the glyph shape
char *
SWFFont_getShape(SWFFont font, unsigned short c)
{
	byte *p = SWFFont_findGlyph(font, c);
	byte **f = &p;
	struct out o;
	int moveBits, x = 0, y = 0;
	int straight, numBits;
	int numFillBits, numLineBits;
	int startX = 0;
	int startY = 0;
	int style;

	o.len = 0;
	o.ptr = o.buf = (char *)malloc(1);
	*o.ptr = 0;

	byteAlign();

	if ( (numFillBits = readBitsP(f, 4)) != 1 ) /* fill bits */
		SWF_error("SWFFont_getShape: bad file format (was expecting fill bits = 1)");

	if ( (numLineBits = readBitsP(f, 4)) > 1 ) /* line bits */
		SWF_error("SWFFont_getShape: bad file format (was expecting line bits = 0)");

	/* now we get to parse the shape commands.	Oh boy.
		 the first one will be a non-edge block- grab the moveto loc */

	readBitsP(f, 2); /* type 0, newstyles */
	style = readBitsP(f, 3);
	
	if(readBitsP(f, 1))
	{	moveBits = readBitsP(f, 5);
		x = startX + readSBitsP(f, moveBits);
		y = startY + readSBitsP(f, moveBits);

		oprintf(&o, "moveto %d,%d\n", x, y);
	}
	else if(style == 0)	/* no style, no move => space character */
		return o.buf;

	if ( style & 1 )
		if ( readBitsP(f, numFillBits) != 0 ) /* fill0 = 0 */
			SWF_error("SWFFont_getShape: bad file format (was expecting fill0 = 0)");
	if ( style & 2 )
		if ( readBitsP(f, numFillBits) != 1 ) /* fill1 = 1 */
			SWF_error("SWFFont_getShape: bad file format (was expecting fill1 = 1)");
	if ( style & 4 )
		if ( readBitsP(f, numLineBits) != 0 ) /* line = 1 */
			SWF_error("SWFFont_getShape: bad file format (was expecting line = 0)");

	/* translate the glyph's shape records into drawing commands */

	for ( ;; )
	{
		if ( readBitsP(f, 1) == 0 )
		{
			/* it's a moveTo or a shape end */

			if ( readBitsP(f, 5) == 0 )
				break;

			moveBits = readBitsP(f, 5);
			x = startX + readSBitsP(f, moveBits);
			y = startY + readSBitsP(f, moveBits);

			oprintf(&o, "moveto %d,%d\n", x, y);

			continue;
		}

		straight = readBitsP(f, 1);
		numBits = readBitsP(f, 4)+2;

		if ( straight==1 )
		{
			if ( readBitsP(f, 1) ) /* general line */
			{
				x += readSBitsP(f, numBits);
				y += readSBitsP(f, numBits);
			}
			else
			{
				if ( readBitsP(f, 1) ) /* vert = 1 */
					y += readSBitsP(f, numBits);
				else
					x += readSBitsP(f, numBits);
			}

			oprintf(&o, "lineto %d,%d\n", x, y);
		}
		else
		{
			int controlX = readSBitsP(f, numBits);
			int controlY = readSBitsP(f, numBits);
			int anchorX = readSBitsP(f, numBits);
			int anchorY = readSBitsP(f, numBits);

			oprintf(&o, "curveto %d,%d %d,%d\n",
				 x+controlX, y+controlY,
				 x+controlX+anchorX, y+controlY+anchorY);

			x += controlX + anchorX;
			y += controlY + anchorY;
		}
	}

	*o.ptr = 0;
	return o.buf;
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
