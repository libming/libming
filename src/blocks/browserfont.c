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

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#include "browserfont.h"
#include "font.h"
#include "character.h"
#include "libming.h"


struct SWFBrowserFont_s
{
	struct SWFCharacter_s character;
	SWFOutput out;
	char *name;
};


static
void writeSWFBrowserFontToMethod(SWFBlock block,
																 SWFByteOutputMethod method, void *data)
{
	SWFOutput out = ((SWFBrowserFont)block)->out;
	SWFOutput_writeToMethod(out, method, data);
}


static void
finishBrowserFont(SWFBrowserFont font)
{
	unsigned int i;
	SWFOutput out;
	SWF_assert(BLOCK(font)->swfVersion);

	out = newSWFOutput();
	font->out = out;	
	
	SWFOutput_writeUInt16(out, CHARACTERID(font));
	if(BLOCK(font)->swfVersion > 5)	/* maybe italic or bold flag? */
		SWFOutput_writeUInt8(out, SWF_FONT_WIDECODES);
	else
	 	SWFOutput_writeUInt8(out, 0); 

	SWFOutput_writeUInt8(out, 0); /* reserved flags */
	SWFOutput_writeUInt8(out, strlen(font->name));

	for ( i=0; i<strlen(font->name); ++i )
		SWFOutput_writeUInt8(out, font->name[i]);

	SWFOutput_writeUInt16(out, 0); /* number of glyphs */
	SWFOutput_writeSInt16(out, 2); /* offset */

	SWFOutput_byteAlign(out);
}

static int
completeSWFBrowserFont(SWFBlock block)
{
	SWFBrowserFont font = (SWFBrowserFont)block;
	if(font->out == NULL) 
		finishBrowserFont(font);
	
	BLOCK(font)->type = SWF_DEFINEFONT2; /* see below */
	return SWFOutput_getLength(font->out);
}

/**
 * destroys a SWFBrowserFont instance.
 */
void
destroySWFBrowserFont(SWFBrowserFont font)
{
	destroySWFOutput(font->out);
	free(font->name);
	free(font);
}

/**
 * creates a browser font instance
 * This Function allows the usage of built in fonts like "_sans".
 * Takes the name of the font as an argument.
 */
SWFBrowserFont
newSWFBrowserFont(const char *name)
{
	SWFBrowserFont font = (SWFBrowserFont) malloc(sizeof(struct SWFBrowserFont_s));

	SWFCharacterInit((SWFCharacter)font);

	BLOCK(font)->writeBlock = writeSWFBrowserFontToMethod;
	BLOCK(font)->complete = completeSWFBrowserFont;
	BLOCK(font)->dtor = (destroySWFBlockMethod) destroySWFBrowserFont;

	BLOCK(font)->type = SWF_BROWSERFONT;
	CHARACTERID(font) = ++SWF_gNumCharacters;
	font->out = NULL;
	font->name = strdup(name);

	return font;
}

SWFOutput
SWFBrowserFont_getOutput(SWFBrowserFont font)
{
	if(font->out == NULL) // in case complete is called twice
		finishBrowserFont(font);

	return font->out;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
