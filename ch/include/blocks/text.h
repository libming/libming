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

/* text.h
 *
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_TEXT_H_INCLUDED
#define SWF_TEXT_H_INCLUDED

#include "ming.h"
#include "output.h"


typedef struct SWFTextRecord_s *SWFTextRecord;

#define SWF_TEXT_STATE_CHANGE (1<<7)
#define SWF_TEXT_HAS_FONT     (1<<3)
#define SWF_TEXT_HAS_COLOR    (1<<2)
#define SWF_TEXT_HAS_Y        (1<<1)
#define SWF_TEXT_HAS_X        (1<<0)

void SWFOutput_writeText(SWFOutput out, SWFText text);

void SWFTextRecord_setFontCharacter(SWFTextRecord record, SWFFontCharacter font);

void SWFText_setScaledHeight(SWFText text, int height);

void SWFText_scaledMoveTo(SWFText text, int x, int y);

void SWFText_setScaledSpacing(SWFText text, int spacing);

int SWFText_getScaledStringWidth(SWFText text, const char *string);

int SWFText_getScaledUTF8StringWidth(SWFText text, const char *string);

int SWFText_getScaledWideStringWidth(SWFText text, const unsigned short *string);

short SWFText_getScaledAscent(SWFText text);

short SWFText_getScaledDescent(SWFText text);

short SWFText_getScaledLeading(SWFText text);

void SWFText_resolveCodes(SWFText text);

SWFTextRecord newSWFTextRecord();

void destroySWFTextRecord(SWFTextRecord record);

int SWFTextRecord_getString(SWFTextRecord record, unsigned short** outStr);

SWFTextRecord SWFText_getInitialRecord(SWFText text);

SWFTextRecord SWFTextRecord_getNextRecord(SWFTextRecord record);

SWFFont SWFTextRecord_getUnresolvedFont(SWFTextRecord record);

void SWFTextRecord_setFontCharacter(SWFTextRecord record, SWFFontCharacter font);

#endif /* SWF_TEXT_H_INCLUDED */
