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

#ifndef SWF_FONT_H_INCLUDED
#define SWF_FONT_H_INCLUDED

typedef struct SWFTextList_s *SWFTextList;
typedef struct SWFFont_s *SWFFont;

#include "libswf.h"

#include "blocktypes.h"
#include "block.h"
#include "output.h"
#include "rect.h"
#include "character.h"
#include "browserfont.h"
#include "text.h"

#define SWF_FONT_UNICODE      (1<<6)
#define SWF_FONT_SHIFTJIS     (1<<5)
#define SWF_FONT_ANSI         (1<<4)
#define SWF_FONT_WIDEOFFSETS  (1<<3)
#define SWF_FONT_WIDECODES    (1<<2)
#define SWF_FONT_ISBOLD       (1<<1)
#define SWF_FONT_ISITALIC     (1<<0)


SWFFont newSWFFont();

void destroySWFFont();

SWFFont loadSWFFontFromFile(FILE *file);

void SWFFont_addTextToList(SWFFont font, SWFTextRecord text);

void SWFFont_resolveTextList(SWFFont font);

byte *SWFFont_findCharacterGlyph(SWFFont font, unsigned char c);

const char* SWFFont_getName(SWFFont font);

byte SWFFont_getFlags(SWFFont font);

int SWFFont_getNGlyphs(SWFFont font);

int SWFFont_getScaledStringWidth(SWFFont font, const unsigned char *string);

short SWFFont_getScaledAscent(SWFFont font);

short SWFFont_getScaledDescent(SWFFont font);

short SWFFont_getScaledLeading(SWFFont font);

int SWFFont_getGlyphCode(SWFFont font, byte c);

SWFRect SWFFont_getGlyphBounds(SWFFont font, byte c);

int SWFFont_getCharacterAdvance(SWFFont font, byte c);

int SWFFont_getCharacterKern(SWFFont font, byte c1, byte c2);

#endif /* SWF_FONT_H_INCLUDED */
