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

#include <math.h>
#include "font.h"
#include "method.h"

#define glyphLength(font,glyph) \
  ((font)->glyphOffset[(glyph)+1] - (font)->glyphOffset[(glyph)])

int completeSWFFont(SWFBlock block)
{
  SWFFont font = (SWFFont)block;
  int size, i;

  SWFFont_resolveTextList(font);

  size = 2 + 2*font->nGlyphs;

  /* get length of each glyph from its output buffer */
  for(i=0; i<font->nGlyphs; ++i)
    size += glyphLength(font, font->codeToGlyph[i]);

  return size;
}
void writeSWFFontToMethod(SWFBlock block,
			  SWFByteOutputMethod method, void *data)
{
  SWFFont font = (SWFFont)block;
  int offset, i;
  byte *p, *s;

  methodWriteUInt16(CHARACTERID(font), method, data);

  offset = font->nGlyphs*2;

  /* write offset table for glyphs */
  for(i=0; i<font->nGlyphs; ++i)
  {
    methodWriteUInt16(offset, method, data);
    offset += glyphLength(font, font->codeToGlyph[i]);
  }

  /* write shape records for glyphs */
  for(i=0; i<font->nGlyphs; ++i)
  {
    p = font->glyphOffset[font->codeToGlyph[i]];
    s = font->glyphOffset[font->codeToGlyph[i]+1];

    SWF_assert(p < s);

    while(p < s)
      method(*(p++), data);
  }
}
void destroySWFFont(SWFBlock block)
{
  SWFFont font = (SWFFont)block;

  if(font->shapes)
    free(font->shapes);
  if(font->bounds)
    free(font->bounds);
  if(font->name)
    free(font->name);
  if(font->kernTable)
    free(font->kernTable);

  free(font);
}
SWFFont newSWFFont()
{
  SWFFont font = calloc(1, SWFFONT_SIZE);

  CHARACTER(font)->number = ++SWF_gNumCharacters;
  BLOCK(font)->type = SWF_DEFINEFONT;
  BLOCK(font)->writeBlock = writeSWFFontToMethod;
  BLOCK(font)->complete = completeSWFFont;
  BLOCK(font)->dtor = destroySWFFont;

  memset(&(font->glyphToCode), 0xff, 256);

  return font;
}

byte *SWFFont_findCharacterGlyph(SWFFont font, unsigned char c)
{
  return font->glyphOffset[font->codeTable[c]];
}

void SWFFont_addTextToList(SWFFont font, struct _textRecord *text)
{
  SWFTextList textList = calloc(1, TEXTLIST_SIZE);
  textList->next = NULL;
  textList->text = text;

  if(font->currentList != NULL)
    font->currentList->next = textList;
  else
    font->textList = textList;

  font->currentList = textList;
}

void SWFFont_addCharToTable(SWFFont font, byte c)
{
  if(font->glyphToCode[c]==0xff) /* assuming one can't actually use all 255 */
  {
    font->codeToGlyph[font->nGlyphs] = font->codeTable[c];
    font->glyphToCode[c] = font->nGlyphs;
    ++font->nGlyphs;
  }
}

/* XXX - big confusion between terms here.  CodeTable isn't font->codeTable */
void SWFFont_buildCodeTable(SWFFont font, SWFTextRecord text)
{
  SWFTextRecord textRecord;
  const byte *string;
  int l, i;

  textRecord = text;
  while(textRecord != NULL)
  {
    string = textRecord->string;

    if(string != NULL)
    {
      l = strlen(string);
      for(i=0; i<l; ++i)
      	SWFFont_addCharToTable(font, string[i]);
    }

    textRecord = textRecord->next;
  }
}

/* build code table from text in all proceding Text blocks */
void SWFFont_resolveTextList(SWFFont font)
{
  SWFTextList textList, oldList;

  textList = font->textList;
  while(textList != NULL)
  {
    oldList = textList;
    SWFFont_buildCodeTable(font, textList->text);
    textList = textList->next;
    free(oldList);
  }

  font->textList = NULL;
}

/* return length of given string in whatever units these are we're using */
int SWFFont_getScaledStringWidth(SWFFont font, const unsigned char *string)
{
  int i, j, width = 0, l = strlen(string);
  int glyph, glyph2;

  for(i=0; i<l; ++i)
  {
    glyph = font->codeTable[string[i]];

    if(font->advances)
      width += font->advances[glyph];

    /* looking in kernTable */
    if (i<l-1 && font->kernTable)
    {
      glyph2 = font->codeTable[string[i+1]];

      j=font->kernCount;

      while (--j >= 0)
      {
	if (glyph == font->kernTable[j].code1 &&
	    glyph2 == font->kernTable[j].code2)
	{
	  width += font->kernTable[j].adjustment;
	  break;
	}
      }
    }
  }

  return width;
}

/* get some font metrics */
short SWFFont_getScaledAscent(SWFFont font)
{
  return font->ascent;
}
short SWFFont_getScaledDescent(SWFFont font)
{
  return font->descent;
}
short SWFFont_getScaledLeading(SWFFont font)
{
  return font->leading;
}
