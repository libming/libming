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

#include "fontinfo.h"
#include "method.h"

void writeDefineSWFFontInfoBlock(SWFBlock block,
				 SWFByteOutputMethod method, void *data)
{
  int l, i;
  SWFFontInfo info = (SWFFontInfo)block;
  SWFFont font = info->font;

  methodWriteUInt16(CHARACTERID(font), method, data);

  l = strlen(font->name);

  SWF_assert(l<256);
  method(l, data);

  for(i=0; i<l; ++i)
    method(font->name[i], data);

  method(font->flags, data);

  /* write glyph-to-code table */
  /* XXX - no support for wide codes */
  for(i=0; i<font->nGlyphs; ++i)
    method(font->codeToGlyph[i], data);
}
int completeDefineSWFFontInfoBlock(SWFBlock block)
{
  SWFFontInfo info = (SWFFontInfo)block;
  return strlen(info->font->name) + 4 + info->font->nGlyphs;
}
SWFFontInfo newDefineSWFFontInfo(SWFFont font)
{
  SWFFontInfo fontInfo = calloc(1, FONTINFO_SIZE);

  BLOCK(fontInfo)->type = SWF_DEFINEFONTINFO;
  BLOCK(fontInfo)->writeBlock = writeDefineSWFFontInfoBlock;
  BLOCK(fontInfo)->complete = completeDefineSWFFontInfoBlock;
  fontInfo->font = font;
  return fontInfo;
}
