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

/* $Id$ */

#if 0

#include "fontinfo.h"
#include "method.h"
#include "libming.h"

struct SWFFontInfo_s
{
	struct SWFCharacter_s character;
	SWFFontCharacter font;
};


static void writeDefineSWFFontInfoBlock(SWFBlock block,
					SWFByteOutputMethod method, void *data)
{
	SWFFontInfo info = (SWFFontInfo)block;
	SWFFontCharacter font = info->font;
	const char* fontname = SWFFont_getName(font);

	int l, i;

	methodWriteUInt16(CHARACTERID(font), method, data);

	l = strlen(fontname);

	SWF_assert(l<256);
	method(l, data);

	for ( i=0; i<l; ++i )
		method(fontname[i], data);

	method(SWFFont_getFlags(font), data);

	/* write glyph-to-code table */
	/* XXX - no support for wide codes */

	for ( i=0; i<SWFFontCharacter_getNGlyphs(font); ++i )
		method(SWFFontCharacter_getGlyphCode(font, i), data);
}


static int completeDefineSWFFontInfoBlock(SWFBlock block)
{
	SWFFontInfo info = (SWFFontInfo)block;
	SWFFont font = info->font;

	return strlen(SWFFont_getName(font)) + 4 + SWFFont_getNGlyphs(font);
}


SWFFontInfo newDefineSWFFontInfo(SWFFont font)
{
	SWFFontInfo fontInfo = malloc(sizeof(struct SWFFontInfo_s));

	SWFCharacterInit((SWFCharacter)fontInfo);

	BLOCK(fontInfo)->type = SWF_DEFINEFONTINFO;
	BLOCK(fontInfo)->writeBlock = writeDefineSWFFontInfoBlock;
	BLOCK(fontInfo)->complete = completeDefineSWFFontInfoBlock;
	fontInfo->font = font;

	return fontInfo;
}

#endif /* 0 */

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
