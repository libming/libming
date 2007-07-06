/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

struct Rect
{ 
  int xMin;
  int xMax;
  int yMin;
  int yMax;
};

struct FontInfo
{
  int ID;
  int GlyphCount;
};

struct Movie
{ 
  int version;
  int size;                     /* in bytes */
  struct Rect frame;
  float rate;
  int nFrames;
  int soundStreamFmt;
  struct FontInfo *fonts;
  int numFonts;
};

static inline void 
Movie_addFontInfo(struct Movie *m, int ID, int count)
{
  m->fonts = realloc(m->fonts, (m->numFonts + 1) * sizeof(struct FontInfo));
  m->fonts[m->numFonts].ID = ID;
  m->fonts[m->numFonts].GlyphCount = count;
  m->numFonts++;
}

static inline int 
Movie_getFontGlyphCount(struct Movie *m, int ID)
{
  int i;
  for(i = 0; i < m->numFonts; i++)
  {
    if(m->fonts[i].ID == ID)
      return m->fonts[i].GlyphCount;
  }
  return -1;
}

#include "swftypes.h"

typedef SWF_Parserstruct *(*SWFParseFunc)(FILE *, int);

#include "parserdecl.h"
