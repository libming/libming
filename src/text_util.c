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

#include "libming.h"
#include "text_util.h"

extern float Ming_scale;

void SWFText_setHeight(SWFText text, float height)
{
  SWFText_setScaledHeight(text, (int)rint(Ming_scale*height));
}

void SWFText_moveTo(SWFText text, float x, float y)
{
  SWFText_scaledMoveTo(text, (int)rint(Ming_scale*x), (int)rint(Ming_scale*y));
}
float SWFText_getStringWidth(SWFText text, const unsigned char *string)
{
  return SWFText_getScaledStringWidth(text, string)/Ming_scale;
}
float SWFText_getAscent(SWFText text)
{
  return SWFText_getScaledAscent(text)/Ming_scale;
}
float SWFText_getDescent(SWFText text)
{
  return SWFText_getDescent(text)/Ming_scale;
}
float SWFText_getLeading(SWFText text)
{
  return SWFText_getLeading(text)/Ming_scale;
}

float SWFFont_getStringWidth(SWFFont font, const unsigned char *string)
{
  return SWFFont_getScaledStringWidth(font, string)/Ming_scale;
}
float SWFFont_getAscent(SWFFont font)
{
  return SWFFont_getScaledAscent(font)/Ming_scale;
}
float SWFFont_getDescent(SWFFont font)
{
  return SWFFont_getDescent(font)/Ming_scale;
}
float SWFFont_getLeading(SWFFont font)
{
  return SWFFont_getLeading(font)/Ming_scale;
}

void SWFTextField_setBounds(SWFTextField field, float width, float height)
{
  SWFTextField_setScaledBounds(field, (int)rint(Ming_scale*width),
			       (int)rint(Ming_scale*height));
}
void SWFTextField_setHeight(SWFTextField field, float height)
{
  SWFTextField_setScaledFontHeight(field, (int)rint(Ming_scale*height));
}
void SWFTextField_setFieldHeight(SWFTextField field, float height)
{
  SWFTextField_setScaledFieldHeight(field, (int)rint(Ming_scale*height));
}
void SWFTextField_setLeftMargin(SWFTextField field, float leftMargin)
{
  SWFTextField_setScaledLeftMargin(field, (int)rint(Ming_scale*leftMargin));
}
void SWFTextField_setRightMargin(SWFTextField field, float rightMargin)
{
  SWFTextField_setScaledRightMargin(field, (int)rint(Ming_scale*rightMargin));
}
void SWFTextField_setIndentation(SWFTextField field, float indentation)
{
  SWFTextField_setScaledIndentation(field, (int)rint(Ming_scale*indentation));
}
void SWFTextField_setLineSpacing(SWFTextField field, float lineSpacing)
{
  SWFTextField_setScaledLineSpacing(field, (int)rint(Ming_scale*lineSpacing));
}
