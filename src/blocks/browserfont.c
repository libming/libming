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


#include "browserfont.h"

static void writeSWFBrowserFontToMethod(SWFBlock block,
					SWFByteOutputMethod method, void *data)
{
  SWFOutput out = ((SWFBrowserFont)block)->out;
  SWFOutput_writeToMethod(out, method, data);
}
static int completeSWFBrowserFont(SWFBlock block)
{
  SWFBrowserFont font = (SWFBrowserFont)block;
  SWFOutput out = font->out;
  SWFOutput_byteAlign(out);
  BLOCK(font)->type = SWF_DEFINEFONT2; /* see below */
  return SWFOutput_length(out);
}
void destroySWFBrowserFont(SWFBlock block)
{
  SWFBrowserFont f = (SWFBrowserFont)block;
  destroySWFOutput(f->out);
  free(f);
}
SWFBrowserFont newSWFBrowserFont(char *name)
{
  unsigned int i;
  SWFBrowserFont font = calloc(1, SWFBROWSERFONT_SIZE);
  SWFOutput out = newSWFOutput();
  memset(font, 0, SWFBROWSERFONT_SIZE);

  BLOCK(font)->writeBlock = writeSWFBrowserFontToMethod;
  BLOCK(font)->complete = completeSWFBrowserFont;
  BLOCK(font)->dtor = destroySWFBrowserFont;

  /* XXX - hack here: we change type to defineFont2 on completion
     so that we can tell the difference in setFont: */

  BLOCK(font)->type = SWF_TEXTFIELD;
  CHARACTERID(font) = ++SWF_gNumCharacters;

  SWFOutput_writeUInt16(out, CHARACTERID(font));
  SWFOutput_writeUInt8(out, 0); /* maybe italic or bold flag? */
  SWFOutput_writeUInt8(out, 0); /* reserved flags */
  SWFOutput_writeUInt8(out, strlen(name));

  for(i=0; i<strlen(name); ++i)
    SWFOutput_writeUInt8(out, name[i]);

  SWFOutput_writeUInt16(out, 0); /* number of glyphs */
  SWFOutput_writeSInt16(out, 2); /* offset */

  font->out = out;

  return font;
}
