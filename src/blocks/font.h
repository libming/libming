/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

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

/* font.h
 * 
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_FONT_H_INCLUDED
#define SWF_FONT_H_INCLUDED

#include "ming.h"
#include "rect.h"
#include "text.h"
#include "block.h"

#define SWF_FONT_HASLAYOUT    (1<<7)
#define SWF_FONT_SHIFTJIS     (1<<6)
#define SWF_FONT_SMALLTEXT    (1<<5)
#define SWF_FONT_ANSI         (1<<4)
#define SWF_FONT_WIDEOFFSETS  (1<<3)
#define SWF_FONT_WIDECODES    (1<<2)
#define SWF_FONT_ISITALIC     (1<<1)
#define SWF_FONT_ISBOLD       (1<<0)

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

struct SWFFont_s
{
	/* even though SWFFont isn't represented in the SWF file, */
	/* this lets us call destroySWFBlock(font) */
	struct SWFBlock_s block;

	// 0 for SWF < 7
	byte langCode;

	char *name;
	byte flags;

	int nGlyphs;

	/* map from glyphs to char codes */
	unsigned short* glyphToCode; 

	/* shape table */
	SWFShape* shapes;

	/* glyph metrics */
	short* advances;

	/* font metrics */
	short ascent;
	short descent;
	short leading;
	
	union
	{
		byte* charMap;
		unsigned short** wideMap; /* array of 256 arrays of 256 shorts */
	} codeToGlyph;

	/* font's kern table, if one is defined */
	/* XXX - should be sorted for faster lookups */
	unsigned short kernCount;
	union
	{	struct kernInfo* k;
		struct kernInfo16* w;
	} kernTable;
};

struct SWFFontCollection_s
{
	SWFFont *fontList;
	int numFonts;
};

SWFFont newSWFFont();

void SWFFontCollection_addFont(SWFFontCollection collection, SWFFont font);
void destroySWFFontCollection(SWFFontCollection collection);
SWFFontCollection newSWFFontCollection();

SWFShape SWFFont_getGlyph(SWFFont font, unsigned short c);

byte SWFFont_getFlags(SWFFont font);

int SWFFont_getScaledWideStringWidth(SWFFont font,
	              		     const unsigned short* string, int len);

int SWFFont_getScaledUTF8StringWidth(SWFFont font, const char* string);

int SWFFont_getScaledStringWidth(SWFFont font, const char* string);

short SWFFont_getScaledAscent(SWFFont font);

short SWFFont_getScaledDescent(SWFFont font);

short SWFFont_getScaledLeading(SWFFont font);

SWFRect SWFFont_getGlyphBounds(SWFFont font, unsigned short c);

int SWFFont_getCharacterAdvance(SWFFont font, unsigned short c);

int SWFFont_getCharacterKern(SWFFont font, unsigned short c1, unsigned short c2);


/* SWFFontCharacter is a movie's instance of a font */

SWFFontCharacter newSWFFontCharacter(SWFFont font);

SWFFontCharacter newSWFDummyFontCharacter();

void destroySWFFontCharacter(SWFFontCharacter fontCharacter);

void SWFFontCharacter_exportCharacterRange(SWFFontCharacter font,
			     unsigned short start, unsigned short end);

void SWFFontCharacter_addWideChars(SWFFontCharacter font, unsigned short *string, int len);

void SWFFontCharacter_addChars(SWFFontCharacter font, const char *string);

void SWFFontCharacter_addUTF8Chars(SWFFontCharacter font, const char *string);

SWFFont SWFFontCharacter_getFont(SWFFontCharacter font);

int SWFFontCharacter_getNGlyphs(SWFFontCharacter font);

void SWFFontCharacter_addTextToList(SWFFontCharacter font, SWFTextRecord text);

/* swf codetable: glyph --> char */
unsigned short SWFFontCharacter_getGlyphCode(SWFFontCharacter font, 
					     unsigned short c);

/* swf codetable: char --> glyph */
int SWFFontCharacter_findGlyphCode(SWFFontCharacter font, unsigned short c);

/* source font: glyph --> char */
unsigned short SWFFont_getGlyphCode(SWFFont font, unsigned short c);

/* source font: char --> glyph */
int SWFFont_findGlyphCode(SWFFont font, unsigned short c);

void SWFFont_buildReverseMapping(SWFFont font);

#endif /* SWF_FONT_H_INCLUDED */
