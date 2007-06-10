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
#include "shape.h"
#include "fdbfont.h"


// #define HAS_MMAP 1

#ifdef HAS_MMAP
	#include <unistd.h>
	#include <sys/mman.h>
#endif

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

	SWFOutput out;
};

static void
SWFFontCharacter_resolveTextCodes(SWFFontCharacter font);

void
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
completeSWFFontCharacter(SWFBlock block)
{
	SWFFontCharacter inst = (SWFFontCharacter)block;
	SWFFont font = inst->font;
	SWFOutput buffer;
	int i, tablen, offset;
	char c, *string;

	SWFFontCharacter_resolveTextCodes(inst);
	
	SWF_assert(!inst->out);
	inst->out = newSWFOutput();
	SWFOutput_writeUInt16(inst->out, CHARACTERID(inst));
	SWFOutput_writeUInt8(inst->out, inst->flags);
	SWFOutput_writeUInt8(inst->out, font->langCode);

	SWFOutput_writeUInt8(inst->out, strlen(font->name));
	string = font->name;
	while ( (c = *(string++)) != 0 )
		SWFOutput_writeUInt8(inst->out, c);

	SWFOutput_writeUInt16(inst->out, inst->nGlyphs);
	
	tablen = (inst->nGlyphs+1) * 
		(inst->flags & SWF_FONT_WIDEOFFSETS ? 4 : 2);
	buffer = newSWFOutput();
	for (i = 0; i < inst->nGlyphs; ++i)
	{
		SWFShape shape = font->shapes[inst->codeTable[i]];
		offset = SWFOutput_getLength(buffer) + tablen;
		SWFOutput_writeGlyphShape(buffer, shape);
		if(inst->flags & SWF_FONT_WIDEOFFSETS)
			SWFOutput_writeUInt32(inst->out, offset);
		else 
			SWFOutput_writeUInt16(inst->out, offset);
	}
	// codeTableOffset
	offset = SWFOutput_getLength(buffer) + tablen;
	if(inst->flags & SWF_FONT_WIDEOFFSETS)
		SWFOutput_writeUInt32(inst->out, offset);
	else
		SWFOutput_writeUInt16(inst->out, offset);

	/* user buffer from here! */
	SWFOutput_setNext(inst->out, buffer);
	
	for (i = 0; i < inst->nGlyphs; ++i)
	{
		unsigned short c = font->glyphToCode[inst->codeTable[i]];
		if (inst->flags & SWF_FONT_WIDECODES)
			SWFOutput_writeUInt16(buffer, c);
		else
			SWFOutput_writeUInt8(buffer, c);
	}
	
	/* write font layout */
	if (inst->flags & SWF_FONT_HASLAYOUT )
	{	SWFOutput_writeUInt16(buffer, font->ascent);
		SWFOutput_writeUInt16(buffer, font->descent);
		SWFOutput_writeUInt16(buffer, font->leading);
		for (i = 0; i < inst->nGlyphs; ++i)
			SWFOutput_writeSInt16(buffer,
				font->advances[inst->codeTable[i]]);
	
		for (i = 0; i < inst->nGlyphs; ++i)
		{	SWFOutput_writeRect(buffer, 
				&font->bounds[inst->codeTable[i]]);
			SWFOutput_byteAlign(buffer);
		}
		SWFOutput_writeUInt16(buffer, 0); /* no kerning */
	}
	return SWFOutput_getLength(inst->out);
}


void
writeSWFFontCharacterToMethod(SWFBlock block,
                              SWFByteOutputMethod method, void *data)
{
	SWFFontCharacter inst = (SWFFontCharacter)block;
	SWFOutput_writeToMethod(inst->out, method, data);
}


void
destroySWFFont(SWFFont font)
{	// should not happen but seemingly does
	if (!font)
		return;
	
	if(font->shapes)
		free(font->shapes);

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
	if( font->out != NULL);
		destroySWFOutput(font->out);
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
	font->langCode = 0;

	font->nGlyphs = 0;
	font->glyphToCode = NULL;
	
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
	SWFFontCharacter inst;

	inst = (SWFFontCharacter) malloc(sizeof(struct SWFFontCharacter_s));
	SWFCharacterInit((SWFCharacter)inst);

	BLOCK(inst)->type = SWF_DEFINEFONT2;
	BLOCK(inst)->writeBlock = writeSWFFontCharacterToMethod;
	BLOCK(inst)->complete = completeSWFFontCharacter;
	BLOCK(inst)->dtor = (destroySWFBlockMethod) destroySWFFontCharacter;

	CHARACTERID(inst) = ++SWF_gNumCharacters;
	inst->font = font;
	inst->flags = font->flags; // (unsigned char)(font->flags & /*(~SWF_FONT_HASLAYOUT) &*/ (~SWF_FONT_WIDEOFFSETS));

	inst->nGlyphs = 0;
	inst->codeTable = NULL;

	inst->textList = NULL;
	inst->currentList = NULL;
	
	inst->out = NULL;

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
	ret->out = NULL;
	
	return ret;
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


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
