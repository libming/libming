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

#ifndef MING_FONT_UTIL_H_INCLUDED
#define MING_FONT_UTIL_H_INCLUDED

#include "blocks/font.h"

SWFFont
Ming_getFont(const char* name);

SWFFont
Ming_loadFont(const char* path, const char* name);

void
Ming_cleanupFonts();

float
SWFFont_getStringWidth(SWFFont font, const unsigned char* string);

float
SWFFont_getWideStringWidth(SWFFont font, const unsigned short* string);

float
SWFFont_getAscent(SWFFont font);

float
SWFFont_getDescent(SWFFont font);

float
SWFFont_getLeading(SWFFont font);

#endif /* MING_FONT_UTIL_H_INCLUDED */
