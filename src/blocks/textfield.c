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

#include <stdlib.h>
#include <string.h>
#include "textfield.h"

/* after every metric change, we update the bounds for the
   SWFCharacter_getBounds function */

static void resetBounds(SWFTextField field)
{
  CHARACTER(field)->bounds->minX = -field->padding;
  CHARACTER(field)->bounds->minY = -field->padding;

  if(field->width == 0)
  {
    int width = field->fontHeight*(field->string ? strlen(field->string) : 0);
    CHARACTER(field)->bounds->maxX = field->padding + width;
  }
  else
    CHARACTER(field)->bounds->maxX = field->padding + field->width;

  if(field->fieldHeight == 0)
  {
    CHARACTER(field)->bounds->maxY =
      field->padding + field->fontHeight*field->nLines +
      (field->nLines-1)*field->lineSpacing;
  }
  else
    CHARACTER(field)->bounds->maxY = field->padding + field->fieldHeight;
}


void writeSWFTextFieldToMethod(SWFBlock block,
			       SWFByteOutputMethod method, void *data)
{
  SWFOutput out = ((SWFBrowserFont)block)->out;
  SWFOutput_writeToMethod(out, method, data);
}
int completeSWFTextField(SWFBlock block)
{
  SWFTextField field = (SWFTextField)block;

  /* we're guessing how big the block's going to be.. */
  SWFOutput out = newSizedSWFOutput(42 +
				    ((field->varName)?strlen(field->varName):0)+
				    ((field->string)?strlen(field->string):0));

  field->out = out;

  resetBounds(field);

  SWFOutput_writeUInt16(out, CHARACTERID(field));
  SWFOutput_writeRect(out, CHARACTER(field)->bounds);
  SWFOutput_writeUInt16(out, field->flags);
  SWFOutput_writeUInt16(out, FONTID(field));
  SWFOutput_writeUInt16(out, field->fontHeight);
  SWFOutput_writeUInt8(out, field->r);
  SWFOutput_writeUInt8(out, field->g);
  SWFOutput_writeUInt8(out, field->b);
  SWFOutput_writeUInt8(out, field->a);

  if(field->flags & SWFTEXTFIELD_HASLENGTH)
    SWFOutput_writeUInt16(out, field->length);

  SWFOutput_writeUInt8(out, field->alignment);

  SWFOutput_writeUInt16(out, field->leftMargin);
  SWFOutput_writeUInt16(out, field->rightMargin);
  SWFOutput_writeUInt16(out, field->indentation);
  SWFOutput_writeUInt16(out, field->lineSpacing);

  SWFOutput_writeString(out, field->varName);
  SWFOutput_writeString(out, field->string);

  /* XXX - if font is a real font, do we need to talk to it? */
  /* flash 4 just makes a browser font for (editable) textfields for all fonts */

  SWFOutput_byteAlign(out);
  return SWFOutput_length(out);
}
void destroySWFTextField(SWFBlock block)
{
  SWFTextField f = (SWFTextField)block;
  SWFCharacter_clearDependencies(CHARACTER(block));
  destroySWFRect(CHARACTER(f)->bounds);
  destroySWFOutput(f->out);
  free(f->varName);
  free(f->string);
  free(f);
}
SWFTextField newSWFTextField()
{
  SWFTextField field = calloc(1, SWFTEXTFIELD_SIZE);

  BLOCK(field)->writeBlock = writeSWFTextFieldToMethod;
  BLOCK(field)->complete = completeSWFTextField;
  BLOCK(field)->dtor = destroySWFTextField;
  BLOCK(field)->type = SWF_TEXTFIELD;
  CHARACTERID(field) = ++SWF_gNumCharacters;
  CHARACTER(field)->bounds = newSWFRect(-40, 280, -40, 280);

  field->lineSpacing = 40;
  field->padding = 40;
  field->fontHeight = 240;
  field->fieldHeight = 0;
  field->width = 0;
  field->a = 0xff;
  field->nLines = 1;

  field->flags = 0x30ad;

  field->font.font = NULL;
  field->varName = NULL;
  field->string = NULL;

  return field;
}

void SWFTextField_setFont(SWFTextField field, SWFBlock font)
{
  if(BLOCK(font)->type == SWF_DEFINEFONT2)
  {
    field->isBrowserFont = FALSE;
    field->font.font = (SWFFont)font;
  }
  else
  {
    field->isBrowserFont = TRUE;
    field->font.browserFont = (SWFBrowserFont)font;
  }

  SWFCharacter_addDependency((SWFCharacter)field, font);
}

void SWFTextField_setScaledBounds(SWFTextField field, int width, int height)
{
  field->width = width;
  field->fieldHeight = height;
  resetBounds(field);
}

void SWFTextField_setFlags(SWFTextField field, int flags)
{
  field->flags = (flags & SWFTEXTFIELD_OFFMASK) | SWFTEXTFIELD_ONMASK;
}
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a)
{
  field->r = r;
  field->g = g;
  field->b = b;
  field->a = a;
}
void SWFTextField_setVariableName(SWFTextField field, char *name)
{
  field->varName = strdup(name);
}
void SWFTextField_addString(SWFTextField field, char *string)
{
  int l;

  for(l=0; string[l]!='\0'; ++l)
  {
    if(string[l] == '\n')
      ++field->nLines;
  }

  if(field->string)
  {
    field->string = realloc(field->string, strlen(field->string)+l+1);
    strcat(field->string, string);
  }
  else
    field->string = strdup(string);

  resetBounds(field);
}
void SWFTextField_setScaledFontHeight(SWFTextField field, int height)
{
  field->fontHeight = height;
  resetBounds(field);
}
void SWFTextField_setScaledFieldHeight(SWFTextField field, int height)
{
  field->fieldHeight = height;
  resetBounds(field);
}
void SWFTextField_setScaledWidth(SWFTextField field, int width)
{
  field->width = width;
  resetBounds(field);
}
void SWFTextField_setScaledPadding(SWFTextField field, int padding)
{
  field->padding = padding;
  resetBounds(field);
}
void SWFTextField_setScaledLeftMargin(SWFTextField field, int leftMargin)
{
  field->leftMargin = leftMargin;
}
void SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin)
{
  field->rightMargin = rightMargin;
}
void SWFTextField_setScaledIndentation(SWFTextField field, int indentation)
{
  field->indentation = indentation;
}
void SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing)
{
  field->lineSpacing = lineSpacing;
  resetBounds(field);
}
void SWFTextField_setAlignment(SWFTextField field,
			       SWFTextFieldAlignment alignment)
{
  field->alignment = alignment;
}
void SWFTextField_setLength(SWFTextField field, int length)
{
  field->length = length;
  field->flags |= SWFTEXTFIELD_HASLENGTH;
}
