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

#include "font.h"
#include "method.h"
#include "utf8.h"

#define HAS_MMAP 1

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

struct textList
{
	struct textList* next;
	SWFTextRecord text;
};

struct SWFFont_s
{
	byte *name;
	byte flags;

	int nGlyphs;

	// map from glyphs to char codes, loaded from fdb file
	unsigned short* glyphToCode; 

	// list of pointers to glyph shapes
	byte** glyphOffset;

	// shape table, mapped in from file
	byte* shapes;

	// glyph metrics
	short* advances;
	struct SWFRect_s* bounds;

	// map from char codes to glyphs, constructed from glyphToCode map
	// XXX - would be nice if this was in the fdb..
	union
	{
		byte* charMap;
		unsigned short** wideMap; // array of 256 arrays of 256 shorts
	} codeToGlyph;

	// font metrics
	short ascent;
	short descent;
	short leading;

	// font's kern table, if one is defined
	// XXX - should be sorted for faster lookups
	unsigned short kernCount;
	struct kernInfo* kernTable;
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
	int size, i;
	int charmap_len;

	// get charcodes from text records, translate into local code map
	SWFFontCharacter_resolveTextCodes(inst);

	/* 2 bytes id, 2 bytes flags, 1 byte name length, 2 bytes nGlyphs,
		 2*(nGlyphs+1) bytes offsets, nGlyphs bytes character map: */

	if ( font->flags & SWF_FONT_WIDECODES )
		charmap_len = 2 * inst->nGlyphs;
	else
		charmap_len = inst->nGlyphs;

	size = 9 + strlen(font->name) + 2*inst->nGlyphs + charmap_len;

	/* get length of each glyph from its output buffer */

	for ( i=0; i<inst->nGlyphs; ++i )
		size += glyphLength(font, inst->codeTable[i]);

	if ( size > 65500 )
	{
		size += 2 * (inst->nGlyphs+1);
		inst->flags |= SWF_FONT_WIDEOFFSETS;
	}

	return size;
}


void
writeSWFFontCharacterToMethod(SWFBlock block,
															SWFByteOutputMethod method, void *data)
{
	SWFFontCharacter inst = (SWFFontCharacter)block;
	SWFFont font = inst->font;
	int offset, i;
	byte *p, *s;

	methodWriteUInt16(CHARACTERID(inst), method, data);

	method(inst->flags & SWF_FONT_WIDEOFFSETS ? 8 : 0, data); /* main flags */
	method(0, data);																					/* more flags */
	method(strlen(font->name), data);

	for ( p = font->name; *p != '\0'; ++p )
		method(*p, data);

	methodWriteUInt16(inst->nGlyphs, method, data);

	offset = (inst->nGlyphs+1) * (inst->flags & SWF_FONT_WIDEOFFSETS ? 4 : 2);

	/* write offset table for glyphs */

	for ( i=0; i<=inst->nGlyphs; ++i )
	{
		if ( font->flags & SWF_FONT_WIDEOFFSETS )
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

	if ( font->flags & SWF_FONT_WIDECODES )
	{
		for ( i=0; i<inst->nGlyphs; ++i )
			methodWriteUInt16(font->glyphToCode[inst->codeTable[i]], method, data);
	}
	else
	{
		for ( i=0; i<inst->nGlyphs; ++i )
			method((byte)font->glyphToCode[inst->codeTable[i]], data);
	}
}


void
destroySWFFont(SWFFont font)
{
	if ( font->shapes != NULL )
	{
#if HAS_MMAP
		int len = font->glyphOffset[font->nGlyphs] - font->glyphOffset[0];
		munmap(font->shapes, len);
#else
		free(font->shapes);
#endif
	}

	if ( font->bounds != NULL )
		free(font->bounds);

	if ( font->name != NULL )
		free(font->name);

	if ( font->kernTable != NULL )
		free(font->kernTable);

	free(font);
}


void
destroySWFFontCharacter(SWFBlock block)
{
	SWFFontCharacter font = (SWFFontCharacter)block;

	if ( font->codeTable != NULL )
		free(font->codeTable);

	free(font);
}


SWFFont
newSWFFont()
{
	SWFFont font = malloc(sizeof(struct SWFFont_s));

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
	font->kernTable = NULL;

	font->shapes = NULL;

	return font;
}


SWFFontCharacter
newSWFFontCharacter(SWFFont font)
{
	SWFFontCharacter inst = malloc(sizeof(struct SWFFontCharacter_s));

	SWFCharacterInit((SWFCharacter)inst);

	BLOCK(inst)->type = SWF_DEFINEFONT2;
	BLOCK(inst)->writeBlock = writeSWFFontCharacterToMethod;
	BLOCK(inst)->complete = completeSWFFontCharacter;
	BLOCK(inst)->dtor = destroySWFFontCharacter;

	CHARACTERID(inst) = ++SWF_gNumCharacters;

	inst->font = font;
	inst->flags = font->flags;

	inst->nGlyphs = 0;
	inst->codeTable = NULL;

	inst->textList = NULL;
	inst->currentList = NULL;

	return inst;
}


static void
SWFFont_buildReverseMapping(SWFFont font)
{
	int i;

	if ( font->flags & SWF_FONT_WIDECODES )
	{
		font->codeToGlyph.wideMap = malloc(256 * sizeof(unsigned short*));

		for ( i=0; i<256; ++i )
			font->codeToGlyph.wideMap[i] = NULL;

		for ( i=0; i<font->nGlyphs; ++i )
		{
			unsigned short charcode = font->glyphToCode[i];
			byte high = charcode >> 8;
			byte low = charcode & 0xff;

			if ( font->codeToGlyph.wideMap[high] == NULL )
			{
				font->codeToGlyph.wideMap[high] = malloc(256 * sizeof(unsigned short));
				memset(font->codeToGlyph.wideMap[high], 0, 256 * sizeof(unsigned short));
			}

			font->codeToGlyph.wideMap[high][low] = i;
		}
	}
	else
	{
		font->codeToGlyph.charMap = malloc(256 * sizeof(char));
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
	struct textList* textList = malloc(sizeof(struct textList));

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
		font->codeTable = realloc(font->codeTable,
															(font->nGlyphs + CODETABLE_INCREMENT) *
															sizeof(unsigned short));
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

	// translate codeTable's char codes into glyph codes so we include the
	// right glyphs

	for ( i=0; i<font->nGlyphs; ++i )
	{
		// XXX - if font doesn't have the character?
		font->codeTable[i] = SWFFont_findGlyphCode(font->font, font->codeTable[i]);
	}
}


const char*
SWFFont_getName(SWFFont font)
{
	return font->name;
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
SWFFont_getScaledStringWidth(SWFFont font,
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

		if ( i < len-1 && font->kernTable )
		{
			glyph2 = SWFFont_findGlyphCode(font, string[i+1]);

			if ( glyph2 == -1 )
				continue; // XXX - ???

			j = font->kernCount;

			while ( --j >= 0 )
			{
				if ( glyph == font->kernTable[j].code1 &&
						 glyph2 == font->kernTable[j].code2 )
				{
					width += font->kernTable[j].adjustment;
					break;
				}
			}
		}
	}

	return width;
}


int
SWFFont_getScaledUTF8StringWidth(SWFFont font, const char* string)
{
	unsigned short* widestr;
	int len = UTF8ExpandString(string, &widestr);

	return SWFFont_getScaledStringWidth(font, widestr, len);
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


int
SWFFontCharacter_getGlyphCode(SWFFontCharacter font, unsigned short c)
{
	return font->codeTable[c];
}


SWFRect
SWFFont_getGlyphBounds(SWFFont font, unsigned short c)
{
	int code = SWFFont_findGlyphCode(font, c);

	if ( code >= 0 )
		return &font->bounds[code];

	return NULL;
}


int
SWFFont_getCharacterAdvance(SWFFont font, unsigned short c)
{
	if ( font->advances )
	{
		int code = SWFFont_findGlyphCode(font, c);

		if ( code >= 0 )
			return font->advances[code];
	}

	return 0;
}


int
SWFFont_getCharacterKern(SWFFont font, unsigned short c1, unsigned short c2)
{
	int j = font->kernCount;
	int kern = 0;
	int g1, g2;

	if( !font->kernTable )
		return 0;

	g1 = SWFFont_findGlyphCode(font, c1);

	if ( g1 == -1 )
		return 0;

	g2 = SWFFont_findGlyphCode(font, c2);

	if ( g2 == -1 )
		return 0;

	// XXX - kernTable should be sorted to make this faster

	while ( --j >= 0 )
	{
		if ( g1 == font->kernTable[j].code1 &&
				 g2 == font->kernTable[j].code2 )
		{
			kern += font->kernTable[j].adjustment;
			break;
		}
	}

	return kern;
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
	return fgetc(f) + (fgetc(f)<<8);
}


static int
readSInt16(FILE *f)
{
	return fgetc(f) + fgetc(f)*256;
}


static unsigned long
readUInt32(FILE *f)
{
	return (unsigned long)(fgetc(f) + (fgetc(f)<<8) + (fgetc(f)<<16) + (fgetc(f)<<24));
}


int buffer;
int bufbits = 0; /* # of bits in buffer */

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

/* this isn't right, and I don't know why..

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
*/

	font->flags |= SWF_FONT_SHIFTJIS;

	fgetc(file); /* "reserved" */

	namelen = fgetc(file);
	font->name = malloc(namelen+1);

	for ( i=0; i<namelen; ++i )
		font->name[i] = fgetc(file);

	font->name[namelen] = '\0';

	nGlyphs = readUInt16(file);

	font->nGlyphs = nGlyphs;

	font->bounds = malloc(nGlyphs * sizeof(struct SWFRect_s));
	font->glyphOffset = malloc(nGlyphs * sizeof(byte*));
	font->glyphToCode = malloc(nGlyphs * sizeof(unsigned char));
	font->advances = malloc(nGlyphs * sizeof(int));

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
	font->shapes = malloc(font->glyphOffset[nGlyphs] - font->glyphOffset[0] + 1);

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
		font->glyphOffset[i] += (unsigned int)font->shapes;

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
			font->kernTable = malloc(sizeof(struct kernInfo) * font->kernCount);
		else
			font->kernTable = NULL;

		for ( i=0; i<font->kernCount; ++i )
			readKernInfo(file, &(font->kernTable[i]));
	}

	return font;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
