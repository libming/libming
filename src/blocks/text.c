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

#include <string.h>
#include <math.h>

#include "text.h"
#include "rect.h"


struct SWFText_s
{
  struct SWFCharacter_s character;

  SWFOutput out;
  SWFMatrix matrix;
  byte nAdvanceBits;
  byte nGlyphBits;
  SWFTextRecord textRecord;
  SWFTextRecord currentRecord;
};

struct SWFTextRecord_s
{
  struct SWFTextRecord_s *next;

  byte flags;
  byte isBrowserFont;

  union
  {
    SWFFont font;
    SWFBrowserFont browserFont;
  } font;

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


static void writeSWFTextToMethod(SWFBlock block,
				 SWFByteOutputMethod method, void *data)
{
  SWFText text = (SWFText)block;
  int length = 0;
  SWFOutput out;

  if(text->matrix == NULL)
    text->matrix = newSWFMatrix(0, 0, 0, 0, 0, 0);

  length += (SWFMatrix_numBits(text->matrix)+7)/8;
  length += (SWFRect_numBits(CHARACTER(text)->bounds)+7)/8;
  length += 4;

  out = newSizedSWFOutput(length);

  SWFOutput_writeUInt16(out, CHARACTERID(text));
  SWFOutput_writeRect(out, CHARACTER(text)->bounds);
  SWFOutput_writeMatrix(out, text->matrix);
  SWFOutput_writeUInt8(out, text->nGlyphBits);
  SWFOutput_writeUInt8(out, text->nAdvanceBits);

  SWFOutput_writeToMethod(out, method, data);
  SWFOutput_writeToMethod(text->out, method, data);

  destroySWFOutput(out);
}


static int completeSWFText(SWFBlock block)
{
  int length = 4;

  SWFText text = (SWFText)block;
  SWFText_resolveCodes(text);

  length += SWFOutput_getLength(text->out);

  if(text->matrix)
    length += (SWFMatrix_numBits(text->matrix)+7)/8;
  else
    ++length;

  length += (SWFRect_numBits(CHARACTER(text)->bounds)+7)/8;

  return length;
}


void destroySWFText(SWFBlock block)
{
  SWFText text = (SWFText)block;
  SWFTextRecord record = text->textRecord, next;

  SWFCharacter_clearDependencies(CHARACTER(block));

  destroySWFOutput(text->out);

  if(text->matrix != NULL)
    destroySWFMatrix(text->matrix);

  while(record != NULL)
  {
    next = record->next;
    destroySWFTextRecord(record);
    record = next;
  }

  destroySWFRect(CHARACTER(text)->bounds);
  sec_free((void**)&text);
}


SWFText newSWFText()
{
  SWFText text = malloc(sizeof(struct SWFText_s));

  SWFCharacterInit((SWFCharacter)text);

  CHARACTERID(text) = ++SWF_gNumCharacters;

  BLOCK(text)->type = SWF_DEFINETEXT;
  BLOCK(text)->writeBlock = writeSWFTextToMethod;
  BLOCK(text)->complete = completeSWFText;
  BLOCK(text)->dtor = destroySWFText;

  CHARACTER(text)->bounds = newSWFRect(0,0,0,0);

  text->out = newSWFOutput();
  text->currentRecord = text->textRecord = newSWFTextRecord();
  text->matrix = NULL;
  text->nAdvanceBits = 0;

  return text;
}


/* only dif is type 2 allows transparency.. */
SWFText newSWFText2()
{
  SWFText text = newSWFText();
  BLOCK(text)->type = SWF_DEFINETEXT2;
  return text;
}


SWFTextRecord newSWFTextRecord()
{
  SWFTextRecord textRecord = malloc(sizeof(struct SWFTextRecord_s));

  textRecord->spacing = 1.0;
  textRecord->height = 240;
  textRecord->flags = 0;
  textRecord->string = NULL;
  textRecord->next = NULL;

  return textRecord;
}


void destroySWFTextRecord(SWFTextRecord record)
{
  if(record->string != NULL)
    sec_free((void**)&record->string);

  if(record->advance != NULL)
  sec_free((void**)&record->advance);
  sec_free((void**)&record);
}


char *SWFTextRecord_getString(SWFTextRecord record)
{
  return record->string;
}


SWFTextRecord SWFTextRecord_getNext(SWFTextRecord record)
{
  return record->next;
}


int SWFText_getScaledStringWidth(SWFText text, const char *string)
{
  SWFFont font = text->currentRecord->font.font;
  int height = text->currentRecord->height;

  if(text->currentRecord->isBrowserFont)
    return 0;
  else
    return SWFFont_getScaledStringWidth(font, string)*height/1024;
}


short SWFText_getScaledAscent(SWFText text)
{
  SWFFont font = text->currentRecord->font.font;
  int height = text->currentRecord->height;

  if(text->currentRecord->isBrowserFont)
    return 0;
  else
    return SWFFont_getScaledAscent(font)*height/1024;
}


short SWFText_getScaledDescent(SWFText text)
{
  SWFFont font = text->currentRecord->font.font;
  int height = text->currentRecord->height;

  if(text->currentRecord->isBrowserFont)
    return 0;
  else
    return SWFFont_getScaledDescent(font)*height/1024;
}


short SWFText_getScaledLeading(SWFText text)
{
  SWFFont font = text->currentRecord->font.font;
  int height = text->currentRecord->height;

  if(text->currentRecord->isBrowserFont)
    return 0;
  else
    return SWFFont_getScaledLeading(font)*height/1024;
}


void SWFText_setFont(SWFText text, SWFBlock font)
{
  SWFTextRecord textRecord = text->currentRecord;

  if(textRecord->string != NULL)
  {
    textRecord->next = newSWFTextRecord();
    textRecord->next->height = textRecord->height;
    textRecord = textRecord->next;
    text->currentRecord = textRecord;
  }

  textRecord->flags |= SWF_TEXT_HAS_FONT;

  /* XXX */  
  if((textRecord->isBrowserFont = (BLOCK(font)->type == SWF_DEFINEEDITTEXT)))
  {
    textRecord->font.browserFont = (SWFBrowserFont)font;
  }
  else
  {
    textRecord->font.font = (SWFFont)font;

    /* add pointer to this textrecord in specified font
       so that it can figure which letter glyphs it needs to define */

    SWFFont_addTextToList((SWFFont)font, textRecord);
  }

  SWFCharacter_addDependency((SWFCharacter)text, font);
}


void SWFText_setScaledHeight(SWFText text, int height)
{
  SWFTextRecord textRecord = text->currentRecord;

  if(textRecord->string != NULL && height != textRecord->height)
  {
    textRecord->next = newSWFTextRecord();
    textRecord->next->isBrowserFont = textRecord->isBrowserFont;
    textRecord->next->font = textRecord->font;
    textRecord->next->spacing = textRecord->spacing;
    textRecord = textRecord->next;
    text->currentRecord = textRecord;
  }

  textRecord->flags |= SWF_TEXT_HAS_FONT;
  textRecord->height = height;
}


void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a)
{
  SWFTextRecord textRecord = text->currentRecord;

  if(textRecord->string != NULL)
  {
    textRecord->next = newSWFTextRecord();
    textRecord->next->isBrowserFont = textRecord->isBrowserFont;
    textRecord->next->font = textRecord->font;
    textRecord->next->spacing = textRecord->spacing;
    textRecord->next->height = textRecord->height;
    textRecord = textRecord->next;
    text->currentRecord = textRecord;
  }

  textRecord->flags |= SWF_TEXT_HAS_COLOR;
  textRecord->r = r;
  textRecord->g = g;
  textRecord->b = b;
  textRecord->a = a;
}


void SWFText_scaledMoveTo(SWFText text, int x, int y)
{
  SWFTextRecord textRecord = text->currentRecord;

  if(textRecord->string != NULL)
  {
    textRecord->next = newSWFTextRecord();
    textRecord->next->isBrowserFont = textRecord->isBrowserFont;
    textRecord->next->font = textRecord->font;
    textRecord->next->spacing = textRecord->spacing;
    textRecord->next->height = textRecord->height;
    textRecord = textRecord->next;
    text->currentRecord = textRecord;
  }

  if(x!=0)
  {
    textRecord->flags |= SWF_TEXT_HAS_X;
    textRecord->x = x;
  }

  if(y!=0)
  {
    textRecord->flags |= SWF_TEXT_HAS_Y;
    textRecord->y = y;
  }
}


void SWFText_setSpacing(SWFText text, float spacing)
{
  text->currentRecord->spacing = spacing;
}


/* adds textrecord type 0 to output */
/* font defn is normalized to height 1024 */
/* added kernTable lookups Oct 4 2k tg */
/* glyph codes changed to (correct) character codes by Raff */

void SWFText_addString(SWFText text, const char *string, int *advance)
{
  SWFTextRecord textRecord = text->currentRecord;
  SWFFont font = textRecord->font.font;
  int l, i, adv;

  /* marginally sloppy, but I don't much want to deal with concats */
  if(textRecord->string != NULL)
  {
    textRecord->next = newSWFTextRecord();
    textRecord->next->isBrowserFont = textRecord->isBrowserFont;
    textRecord->next->font = textRecord->font;
    textRecord->next->height = textRecord->height;
    textRecord = textRecord->next;
    text->currentRecord = textRecord;
  }

  textRecord->string = strdup(string);

  l = strlen(string);
  textRecord->advance = malloc(sizeof(int)*l);

  if(textRecord->isBrowserFont)
  {
    memset(textRecord->advance, 0, l);
    return;
  }

  /* compute advances (spacing) from spacing, advance and kern table */
  /* XXX - spacing is cross-character, not between-character, so it doesn't
     scale right, a character twice as wide gets twice as much space after.
     spacing should be additive, not multiplicative */

  for(i=0; i<l; ++i)
  {
    adv = SWFFont_getCharacterAdvance(font, (byte)string[i]);

    /* looking in kernTable */
    if (i<l-1)
      adv += SWFFont_getCharacterKern(font, (byte)string[i], (byte)string[i+1]);

    if(advance != NULL)
      adv += advance[i];

    textRecord->advance[i] = adv * textRecord->height / 1024;

    text->nAdvanceBits = max(text->nAdvanceBits,
			     SWFOutput_numSBits(textRecord->advance[i]));
  }
}


/* turn textRecord list into output code */
void SWFText_resolveCodes(SWFText text)
{
  SWFTextRecord textRecord, oldRecord;
  SWFOutput out = text->out;
  int nGlyphBits=0;
  int advance;
  int l, i, curX=0, curY=0, curH=0;

  textRecord = text->textRecord;

  while(textRecord != NULL)
  {
    if(textRecord->flags & SWF_TEXT_HAS_FONT)
    {
      if(textRecord->isBrowserFont)
	nGlyphBits = max(nGlyphBits, 8); /* XXX - assume browser fonts have 8bit glyph table? */
      else
      {
	nGlyphBits = max(nGlyphBits,
			 SWFOutput_numBits(SWFFont_getNGlyphs(textRecord->font.font)-1));
      }
    }

    textRecord = textRecord->next;
  }

  textRecord = text->textRecord;

  while(textRecord != NULL)
  {
    oldRecord = textRecord;

    if(textRecord->string == NULL || strlen(textRecord->string) == 0)
    {
      textRecord = textRecord->next;
      destroySWFTextRecord(oldRecord);
      continue;
    }

    SWFOutput_byteAlign(out);

    /* Raff says the spec lies- there's always a change record, even if
       it's empty, and the string record length is the full 8 bits. */

    SWFOutput_writeUInt8(out, textRecord->flags | SWF_TEXT_STATE_CHANGE);

    if(textRecord->flags & SWF_TEXT_HAS_FONT)
      SWFOutput_writeUInt16(out, FONTID(textRecord));

    if(textRecord->flags & SWF_TEXT_HAS_COLOR)
    {
      SWFOutput_writeUInt8(out, textRecord->r);
      SWFOutput_writeUInt8(out, textRecord->g);
      SWFOutput_writeUInt8(out, textRecord->b);

      if(BLOCK(text)->type == SWF_DEFINETEXT2)
	SWFOutput_writeUInt8(out, textRecord->a);
    }

    if(textRecord->flags & SWF_TEXT_HAS_X)
    {
      SWFOutput_writeUInt16(out, textRecord->x);
      curX = textRecord->x;
    }

    if(textRecord->flags & SWF_TEXT_HAS_Y)
    {
      SWFOutput_writeUInt16(out, textRecord->y);
      curY = textRecord->y;
    }

    if(textRecord->flags & SWF_TEXT_HAS_FONT)
    {
      SWFOutput_writeUInt16(out, textRecord->height);
      curH = textRecord->height;
    }

    /* record type 0 */
    l = strlen(textRecord->string);

    if(l >= 256)
      SWF_error("Found text record >= 256 characters!");

    SWFOutput_writeUInt8(out, l);

    /* XXX - er, browser fonts in text objects crash the player..
       Maybe because there's no definefontinfo block? */

    if(textRecord->isBrowserFont)
    {
      for(i=0; i<l; ++i)
      {
	SWFOutput_writeBits(out, textRecord->string[i], nGlyphBits);

	advance = (int)floor(textRecord->advance[i]);
	SWFOutput_writeBits(out, advance, text->nAdvanceBits);

	/* XXX */
	if(CHARACTER(text)->bounds)
	{
	  SWFRect_includePoint(CHARACTER(text)->bounds, curX, curY, 0);
	  SWFRect_includePoint(CHARACTER(text)->bounds,
			       curX + curH, curY + curH, 0);
	}
	else
	{
	  CHARACTER(text)->bounds =
	    newSWFRect(curX, curX+curH, curY, curY+curH);
	}

	curX += curH;
      }
    }
    else
    {
      SWFFont font = textRecord->font.font;

      if(font == NULL)
	SWF_error("Couldn't find font");

      for(i=0; i<l; ++i)
      {
	int minX, maxX, minY, maxY;

	SWFRect glyphBounds =
	  SWFFont_getGlyphBounds(font, (byte)textRecord->string[i]);

	SWFRect_getBounds(glyphBounds, &minX, &maxX, &minY, &maxY);

	SWFOutput_writeBits(out,
			    SWFFont_getGlyphCode(font, textRecord->string[i]),
			    nGlyphBits);

	advance = (int)floor(textRecord->advance[i]);
	SWFOutput_writeBits(out, advance, text->nAdvanceBits);

	if(CHARACTER(text)->bounds)
	{
	  SWFRect_includePoint(CHARACTER(text)->bounds,
			       curX + minX * curH / 1024,
			       curY + minY * curH / 1024,
			       0);

	  SWFRect_includePoint(CHARACTER(text)->bounds,
			       curX + maxX * curH / 1024,
			       curY + maxY * curH / 1024,
			       0);
	}
	else
	{
	  CHARACTER(text)->bounds =
	    newSWFRect(curX + minX * curH /1024, curX + maxX * curH /1024,
		       curY + minY * curH /1024, curY + maxY * curH /1024);
	}

	curX += advance;
      }
    }

    textRecord = textRecord->next;
    destroySWFTextRecord(oldRecord);
  }

  SWFOutput_writeUInt8(out, 0); /* end text records */

  text->nGlyphBits = nGlyphBits;
  text->textRecord = NULL;
}
