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

#ifndef SWF_TEXT_H_INCLUDED
#define SWF_TEXT_H_INCLUDED

typedef struct SWFText_s *SWFText;
typedef struct SWFTextRecord_s *SWFTextRecord;

#include "libswf.h"

#include "output.h"
#include "block.h"
#include "matrix.h"
#include "shape.h"
#include "font.h"
#include "rect.h"
#include "character.h"


#define SWF_TEXT_STATE_CHANGE (1<<7)
#define SWF_TEXT_HAS_FONT     (1<<3)
#define SWF_TEXT_HAS_COLOR    (1<<2)
#define SWF_TEXT_HAS_Y        (1<<1)
#define SWF_TEXT_HAS_X        (1<<0)


SWFText
newSWFText();

SWFText
newSWFText2();

void
destroySWFText(SWFBlock block);

void
SWFOutput_writeText(SWFOutput out, SWFText text);

void
SWFText_setFont(SWFText text, void* font);

void
SWFTextRecord_setFontCharacter(SWFTextRecord record, SWFFontCharacter font);

void
SWFText_setScaledHeight(SWFText text, int height);

void
SWFText_scaledMoveTo(SWFText text, int x, int y);

void
SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);

void
SWFText_addString(SWFText text, const char* string, int* advance);

void
SWFText_addUTF8String(SWFText text, const char* string, int* advance);

void
SWFText_addWideString(SWFText text, const unsigned short* string,
		      int strlen, int* advance);

void
SWFText_setScaledSpacing(SWFText text, int spacing);

int
SWFText_getScaledStringWidth(SWFText text, const char *string);

short
SWFText_getScaledAscent(SWFText text);

short
SWFText_getScaledDescent(SWFText text);

short
SWFText_getScaledLeading(SWFText text);

void
SWFText_resolveCodes(SWFText text);


SWFTextRecord
newSWFTextRecord();

void
destroySWFTextRecord(SWFTextRecord record);

int
SWFTextRecord_getString(SWFTextRecord record, unsigned short** outStr);

SWFTextRecord
SWFText_getInitialRecord(SWFText text);

SWFTextRecord
SWFTextRecord_getNextRecord(SWFTextRecord record);

SWFFont
SWFTextRecord_getFont(SWFTextRecord record);

void
SWFTextRecord_setFontCharacter(SWFTextRecord record, SWFFontCharacter font);

#endif /* SWF_TEXT_H_INCLUDED */
