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

/* $Id$ */

#include <math.h>

#include "font.h"
#include "method.h"


struct kernInfo
{
  byte code1;
  byte code2;
  short adjustment;
};


struct SWFTextList_s
{
  struct SWFTextList_s *next;
  struct SWFTextRecord_s *text;	/* hrm.  any way around this? */
};


struct SWFFont_s
{
  struct SWFCharacter_s character;

  byte flags;
  byte *name;

  short nGlyphs;
  short nFontGlyphs;

  byte codeToGlyph[256];
  byte glyphToCode[256];
  byte *glyphOffset[257];
  byte codeTable[256];
  short ascent;
  short descent;
  short leading;
  unsigned short kernCount;
  short advances[256];
  SWFRect *bounds;
  struct kernInfo *kernTable;
  SWFTextList textList;
  SWFTextList currentList;
  byte *shapes;			/* loaded in from file */
  /* note that that means we can't compute bounds */
  /* w/out bounds table */
};


#define glyphLength(font,glyph) \
  ((font)->glyphOffset[(glyph)+1] - (font)->glyphOffset[(glyph)])

int
completeSWFFont (SWFBlock block)
{
  SWFFont font = (SWFFont) block;
  int size, i;

  SWFFont_resolveTextList (font);

  /* 2 bytes id, 2 bytes flags, 1 byte name length, 2 bytes nGlyphs,
     2*(nGlyphs+1) bytes offsets, nGlyphs bytes character map: */

  size = 9 + strlen (font->name) + 3 * font->nGlyphs;

  /* get length of each glyph from its output buffer */
  for (i = 0; i < font->nGlyphs; ++i)
    size += glyphLength (font, font->codeToGlyph[i]);

  if (size > 65500)
  {
    size += 2 * (font->nGlyphs + 1);
    font->flags |= SWF_FONT_WIDEOFFSETS;
  }

  return size;
}


void
writeSWFFontToMethod (SWFBlock block, SWFByteOutputMethod method, void *data)
{
  SWFFont font = (SWFFont) block;
  int offset, i;
  byte *p, *s;

  methodWriteUInt16 (CHARACTERID (font), method, data);

  method (font->flags & SWF_FONT_WIDEOFFSETS ? 8 : 0, data);	/* main flags */
  method (0, data);		/* more flags */
  method (strlen (font->name), data);

  for (p = font->name; *p != '\0'; ++p)
    method (*p, data);

  methodWriteUInt16 (font->nGlyphs, method, data);

  offset = (font->nGlyphs + 1) * (font->flags & SWF_FONT_WIDEOFFSETS ? 4 : 2);

  /* write offset table for glyphs */
  for (i = 0; i <= font->nGlyphs; ++i)
  {
    if (font->flags & SWF_FONT_WIDEOFFSETS)
      methodWriteUInt32 (offset, method, data);
    else
      methodWriteUInt16 (offset, method, data);

    if (i < font->nGlyphs)
      offset += glyphLength (font, font->codeToGlyph[i]);
  }

  /* write shape records for glyphs */
  for (i = 0; i < font->nGlyphs; ++i)
  {
    p = font->glyphOffset[font->codeToGlyph[i]];
    s = font->glyphOffset[font->codeToGlyph[i] + 1];

    SWF_assert (p < s);

    while (p < s)
      method (*(p++), data);
  }

  /* write (dummy) character mapping */
  for (i = 0; i < font->nGlyphs; ++i)	/* this is rubbish - would need another */
    method (i, data);		/* table to reverse mapping */
}


void
destroySWFFont (SWFBlock block)
{
  SWFFont font = (SWFFont) block;

  sec_free ((void **) &font->shapes);

  if (font->bounds)
  {
    int i;

    for (i = 0; i < font->nFontGlyphs; ++i)
      destroySWFRect (font->bounds[i]);

    sec_free ((void **) &font->bounds);
  }

  sec_free ((void **) &font->name);
  sec_free ((void **) &font->kernTable);
  sec_free ((void **) &font);
}


SWFFont
newSWFFont ()
{
  SWFFont font = calloc (1, sizeof (struct SWFFont_s));

  SWFCharacterInit ((SWFCharacter) font);

  CHARACTERID (font) = ++SWF_gNumCharacters;
  BLOCK (font)->type = SWF_DEFINEFONT2;
  BLOCK (font)->writeBlock = writeSWFFontToMethod;
  BLOCK (font)->complete = completeSWFFont;
  BLOCK (font)->dtor = destroySWFFont;

  memset (&(font->glyphToCode), 0xff, 256);
  font->currentList = NULL;

  return font;
}


byte *
SWFFont_findCharacterGlyph (SWFFont font, unsigned char c)
{
  return font->glyphOffset[font->codeTable[c]];
}


void
SWFFont_addTextToList (SWFFont font, SWFTextRecord text)
{
  SWFTextList textList = malloc (sizeof (struct SWFTextList_s));

  textList->next = NULL;
  textList->text = text;

  if (font->currentList != NULL)
    font->currentList->next = textList;
  else
    font->textList = textList;

  font->currentList = textList;
}


void
SWFFont_addCharToTable (SWFFont font, byte c)
{
  if (font->glyphToCode[c] == 0xff)	/* assuming one can't actually use all 255 */
  {
    font->codeToGlyph[font->nGlyphs] = font->codeTable[c];
    font->glyphToCode[c] = font->nGlyphs;
    ++font->nGlyphs;
  }
}


/* XXX - big confusion between terms here.  CodeTable isn't font->codeTable */
void
SWFFont_buildCodeTable (SWFFont font, SWFTextRecord text)
{
  SWFTextRecord textRecord;
  const byte *string;
  int l, i;

  textRecord = text;

  while (textRecord != NULL)
  {
    string = SWFTextRecord_getString (textRecord);

    if (string != NULL)
    {
      l = strlen (string);

      for (i = 0; i < l; ++i)
	SWFFont_addCharToTable (font, string[i]);
    }

    textRecord = SWFTextRecord_getNext (textRecord);
  }
}


/* build code table from text in all proceding Text blocks */
void
SWFFont_resolveTextList (SWFFont font)
{
  SWFTextList textList, oldList;

  textList = font->textList;

  while (textList != NULL)
  {
    oldList = textList;
    SWFFont_buildCodeTable (font, textList->text);
    textList = textList->next;
    sec_free ((void **) &oldList);
  }

  font->textList = NULL;
}


const char *
SWFFont_getName (SWFFont font)
{
  return font->name;
}


byte
SWFFont_getFlags (SWFFont font)
{
  return font->flags;
}


int
SWFFont_getNGlyphs (SWFFont font)
{
  return font->nGlyphs;
}


/* return length of given string in whatever units these are we're using */
int
SWFFont_getScaledStringWidth (SWFFont font, const unsigned char *string)
{
  int i, j, width = 0, l = strlen (string);
  int glyph, glyph2;

  for (i = 0; i < l; ++i)
  {
    glyph = font->codeTable[string[i]];

    if (font->advances)
      width += font->advances[glyph];

    /* looking in kernTable */
    if (i < l - 1 && font->kernTable)
    {
      glyph2 = font->codeTable[string[i + 1]];

      j = font->kernCount;

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
short
SWFFont_getScaledAscent (SWFFont font)
{
  return font->ascent;
}


short
SWFFont_getScaledDescent (SWFFont font)
{
  return font->descent;
}


short
SWFFont_getScaledLeading (SWFFont font)
{
  return font->leading;
}


int
SWFFont_getGlyphCode (SWFFont font, byte c)
{
  return font->glyphToCode[c];
}


SWFRect
SWFFont_getGlyphBounds (SWFFont font, byte c)
{
  return font->bounds[font->codeTable[c]];
}


int
SWFFont_getCharacterAdvance (SWFFont font, byte c)
{
  if (font->advances)
    return font->advances[font->codeTable[c]];
  else
    return 0;
}


int
SWFFont_getCharacterKern (SWFFont font, byte c1, byte c2)
{
  int j = font->kernCount;
  int kern = 0;

  int g1 = font->codeTable[c1];
  int g2 = font->codeTable[c2];

  if (!font->kernTable)
    return 0;

  while (--j >= 0)
  {
    if (g1 == font->kernTable[j].code1 && g2 == font->kernTable[j].code2)
    {
      kern += font->kernTable[j].adjustment;
      break;
    }
  }

  return kern;
}


/* font loader */

#define SWF_LOAD_FONT_HASLAYOUT    (1<<7)
#define SWF_LOAD_FONT_SHIFTJIS     (1<<6)
#define SWF_LOAD_FONT_UNICODE      (1<<5)
#define SWF_LOAD_FONT_ANSI         (1<<4)
#define SWF_LOAD_FONT_WIDEOFFSETS  (1<<3)
#define SWF_LOAD_FONT_WIDECODES    (1<<2)
#define SWF_LOAD_FONT_ITALIC       (1<<1)
#define SWF_LOAD_FONT_BOLD         (1<<0)


static int
readUInt16 (FILE * f)
{
  return fgetc (f) + (fgetc (f) << 8);
}


static int
readSInt16 (FILE * f)
{
  return fgetc (f) + fgetc (f) * 256;
}


static unsigned long
readUInt32 (FILE * f)
{
  return (unsigned long) (fgetc (f) + (fgetc (f) << 8) + (fgetc (f) << 16) +
			  (fgetc (f) << 24));
}


int buffer;
int bufbits = 0;		/* # of bits in buffer */

static void
byteAlign ()
{
  if (bufbits > 0)
  {
    bufbits = 0;
    buffer = 0;
  }
}


static int
readBits (FILE * f, int number)
{
  int ret = buffer;

  if (number == bufbits)
  {
    bufbits = 0;
    buffer = 0;
    return ret;
  }

  if (number > bufbits)
  {
    number -= bufbits;

    while (number > 8)
    {
      ret <<= 8;
      ret += fgetc (f);
      number -= 8;
    }

    buffer = fgetc (f);

    if (number > 0)
    {
      ret <<= number;
      bufbits = 8 - number;
      ret += buffer >> (8 - number);
      buffer &= (1 << bufbits) - 1;
    }

    return ret;
  }

  ret = buffer >> (bufbits - number);
  bufbits -= number;
  buffer &= (1 << bufbits) - 1;

  return ret;
}


static int
readSBits (FILE * f, int number)
{
  int num = readBits (f, number);

  if (num & (1 << (number - 1)))
    return num - (1 << number);
  else
    return num;
}


SWFRect
readBounds (FILE * file)
{
  int nBits;
  int minX, maxX, minY, maxY;

  byteAlign ();

  nBits = readBits (file, 5);
  minX = readSBits (file, nBits);
  maxX = readSBits (file, nBits);
  minY = readSBits (file, nBits);
  maxY = readSBits (file, nBits);

  return newSWFRect (minX, maxX, minY, maxY);
}


void
readKernInfo (FILE * file, struct kernInfo *kern)
{
  kern->code1 = fgetc (file);
  kern->code2 = fgetc (file);
  kern->adjustment = readSInt16 (file);
}


SWFFont
loadSWFFontFromFile (FILE * file)
{
  /* pull font definition from fdb (font def block) file */

  SWFFont font = newSWFFont ();
  int namelen, flags, i, nGlyphs;
  byte *p;

  if (file == NULL)
    return NULL;

  fgetc (file);			/* header whatever */
  fgetc (file);
  fgetc (file);
  fgetc (file);

  flags = fgetc (file);

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

  fgetc (file);			/* "reserved" */

  namelen = fgetc (file);
  font->name = malloc (namelen + 1);

  for (i = 0; i < namelen; ++i)
    font->name[i] = fgetc (file);

  font->name[namelen] = '\0';

  nGlyphs = readUInt16 (file);

  font->nFontGlyphs = nGlyphs;

  font->bounds = malloc (nGlyphs * sizeof (SWFRect));

  if (flags & SWF_LOAD_FONT_WIDEOFFSETS)
  {
    for (i = 0; i <= nGlyphs; ++i)
      font->glyphOffset[i] = (byte *) (readUInt32 (file) - 4 * nGlyphs - 4);
  }
  else
  {
    for (i = 0; i <= nGlyphs; ++i)
      font->glyphOffset[i] = (byte *) (readUInt16 (file) - 2 * nGlyphs - 2);
  }

  /* it helps to allocate the right amount. (thanks, Tim!) */
  font->shapes =
    malloc (font->glyphOffset[nGlyphs] - font->glyphOffset[0] + 1);

  /* adjust offset table to point to shapes */
  for (i = 0; i <= nGlyphs; ++i)
    font->glyphOffset[i] += (unsigned int) font->shapes;

  p = font->shapes;

  for (i = (int) (font->glyphOffset[nGlyphs] - font->glyphOffset[0]); i > 0;
       --i)
    *(p++) = fgetc (file);

  for (i = 0; i < nGlyphs; ++i)
    font->codeTable[fgetc (file)] = i;

  if (flags & SWF_LOAD_FONT_HASLAYOUT)
  {
    font->ascent = readSInt16 (file);
    font->descent = readSInt16 (file);
    font->leading = readSInt16 (file);

    /* get advances */
    for (i = 0; i < nGlyphs; ++i)
      font->advances[i] = readSInt16 (file);

    /* get bounds */
    for (i = 0; i < nGlyphs; ++i)
      font->bounds[i] = readBounds (file);

    /* get kern table */
    font->kernCount = readUInt16 (file);

    if (font->kernCount > 0)
      font->kernTable = malloc (sizeof (struct kernInfo) * font->kernCount);
    else
      font->kernTable = NULL;

    for (i = 0; i < font->kernCount; ++i)
      readKernInfo (file, &(font->kernTable[i]));
  }

  font->nGlyphs = 0;		/* prepare for adding chars.. */

  return font;
}
