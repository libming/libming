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

/* ttffont.h
 * 
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_TTFFONT_H_INCLUDED
#define SWF_TTFFONT_H_INCLUDED

#include <truetype.h>

#include "ming.h"

#define SWF_FONT_UNICODE    (1<<5)
#define SWF_FONT_SHIFTJIS   (1<<4)
#define SWF_FONT_ANSI       (1<<3)
#define SWF_FONT_ISITALIC   (1<<2)
#define SWF_FONT_ISBOLD     (1<<1)
#define SWF_FONT_WIDECODES  (1<<0)

struct KernInfo
{
  byte code1;
  byte code2;
  short adjustment;
};

struct _textList
{
  struct _textList *next;
  struct _textRecord *text;		/* hrm.  any way around this? */
};
typedef struct _textList *SWFTextList;

#define TEXTLIST_SIZE sizeof(struct _textList)

struct _font
{
  swfCharacter character;
  char *file;
  int nGlyphs;
  byte flags;
  byte codeToGlyph[256];
  byte glyphToCode[256];
  byte *glyphOffset[257];
  byte codeTable[256];
  short advances[256];
  SWFTextList textList;
  SWFTextList currentList;
};
typedef struct _font *SWFFont;

#define SWFFONT_SIZE sizeof(struct _font)

struct _textRecord
{
  struct _textRecord *next;
  byte flags;
  SWFFont font;
  byte r;
  byte g;
  byte b;
  byte a;
  int x;
  int y;
  int height;
  float spacing;
  byte *string;
  int *advance;
};
typedef struct _textRecord *SWFTextRecord;

#define TEXTRECORD_SIZE sizeof(struct _textRecord)

SWFTextRecord newSWFTextRecord();
void destroySWFTextRecord(SWFTextRecord record);

SWFFont newSWFFont();
void destroySWFFont(SWFFont font);
SWFFont loadSWFFontFromTTF(char *file);

void SWFFont_addTextToList(SWFFont font, struct _textRecord *text);
void SWFFont_resolveTextList(SWFFont font);

#endif /* SWF_TTFFONT_H_INCLUDED */
