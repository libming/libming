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

#ifndef SWF_BROWSERFONT_H_INCLUDED
#define SWF_BROWSERFONT_H_INCLUDED

#include "libswf.h"
#include "output.h"
#include "character.h"

struct _browserFont
{
  swfCharacter character;
  SWFOutput out;
};
typedef struct _browserFont *SWFBrowserFont;

#define FONTID(t) (((t)->isBrowserFont) ? CHARACTERID((t)->font.browserFont) : CHARACTERID((t)->font.font))

#define SWFBROWSERFONT_SIZE sizeof(struct _browserFont)

SWFBrowserFont newSWFBrowserFont(char *name);

#endif /* SWF_BROWSERFONT_H_INCLUDED */
