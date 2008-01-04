/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

#include "ming.h"
#include "text_util.h"
#include "movie.h"
#include "blocks/text.h"
#include "blocks/font.h"
#include "blocks/textfield.h"
#include "libming.h"


void
SWFText_setHeight(SWFText text, float height)
{
	SWFText_setScaledHeight(text, (int)rint(Ming_scale*height));
}


void
SWFText_setSpacing(SWFText text, float spacing)
{
	SWFText_setScaledSpacing(text, (int)rint(Ming_scale*spacing));
}


void
SWFText_moveTo(SWFText text, float x, float y)
{
	SWFText_scaledMoveTo(text, (int)rint(Ming_scale*x), (int)rint(Ming_scale*y));
}


float
SWFText_getStringWidth(SWFText text, const char *string)
{
	return SWFText_getScaledStringWidth(text, string)/Ming_scale;
}


float
SWFText_getUTF8StringWidth(SWFText text, const char *string)
{
	return SWFText_getScaledUTF8StringWidth(text, string)/Ming_scale;
}


float
SWFText_getWideStringWidth(SWFText text, const unsigned short *string)
{
	return SWFText_getScaledWideStringWidth(text, string)/Ming_scale;
}


float
SWFText_getAscent(SWFText text)
{
	return SWFText_getScaledAscent(text)/Ming_scale;
}


float
SWFText_getDescent(SWFText text)
{
	return SWFText_getScaledDescent(text)/Ming_scale;
}


float
SWFText_getLeading(SWFText text)
{
	return SWFText_getScaledLeading(text)/Ming_scale;
}


float
SWFFont_getStringWidth(SWFFont font, const char *string)
{
	return SWFFont_getScaledStringWidth(font, string) / Ming_scale;
}


float
SWFFont_getUTF8StringWidth(SWFFont font, const char *string)
{
	return SWFFont_getScaledUTF8StringWidth(font, string) / Ming_scale;
}


float
SWFFont_getWideStringWidth(SWFFont font, const unsigned short *string, int len)
{
	return SWFFont_getScaledWideStringWidth(font, string, len) / Ming_scale;
}


float
SWFFont_getAscent(SWFFont font)
{
	return SWFFont_getScaledAscent(font)/Ming_scale;
}


float
SWFFont_getDescent(SWFFont font)
{
	return SWFFont_getScaledDescent(font)/Ming_scale;
}


float
SWFFont_getLeading(SWFFont font)
{
	return SWFFont_getScaledLeading(font)/Ming_scale;
}


void
SWFTextField_setBounds(SWFTextField field, float width, float height)
{
	SWFTextField_setScaledBounds(field, (int)rint(Ming_scale*width),
						 (int)rint(Ming_scale*height));
}


void
SWFTextField_setHeight(SWFTextField field, float height)
{
	SWFTextField_setScaledFontHeight(field, (int)rint(Ming_scale*height));
}


void
SWFTextField_setFieldHeight(SWFTextField field, float height)
{
	SWFTextField_setScaledFieldHeight(field, (int)rint(Ming_scale*height));
}


void
SWFTextField_setLeftMargin(SWFTextField field, float leftMargin)
{
	SWFTextField_setScaledLeftMargin(field, (int)rint(Ming_scale*leftMargin));
}


void
SWFTextField_setRightMargin(SWFTextField field, float rightMargin)
{
	SWFTextField_setScaledRightMargin(field, (int)rint(Ming_scale*rightMargin));
}


void
SWFTextField_setIndentation(SWFTextField field, float indentation)
{
	SWFTextField_setScaledIndentation(field, (int)rint(Ming_scale*indentation));
}


void
SWFTextField_setLineSpacing(SWFTextField field, float lineSpacing)
{
	SWFTextField_setScaledLineSpacing(field, (int)rint(Ming_scale*lineSpacing));
}


void
SWFTextField_setPadding(SWFTextField field, float padding)
{
	SWFTextField_setScaledPadding(field, (int)rint(Ming_scale*padding));
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
