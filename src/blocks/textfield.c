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

#include "libming.h"
#include "textfield.h"
#include "utf8.h"
#include "character.h"
#include "browserfont.h"
#include "font.h"


typedef enum {Unresolved, BrowserFont, Font, FontChar, Imported} FontType;
struct SWFTextField_s
{
	struct SWFCharacter_s character;

	SWFOutput out; /* cheap way out */
	int flags;
	FontType fonttype;

	union
	{
		SWFFont font;
		SWFFontCharacter fontchar;
		SWFBrowserFont browserFont;
	} font;

	int nLines;
	int fontHeight;
	int fieldHeight;
	int width;
	int padding;

	byte r;
	byte g;
	byte b;
	byte a;

	short length;
	byte alignment;
	short leftMargin;
	short rightMargin;
	short indentation;
	short lineSpacing;

	char *varName;
	char *string;
	unsigned short *embeds;
	int embedlen;
};


/* after every metric change, we update the bounds for the
	 SWFCharacter_getBounds function */

static void
resetBounds(SWFTextField field)
{
	int minX, maxX, minY, maxY;

	SWFRect_getBounds(CHARACTER(field)->bounds, &minX, &maxX, &minY, &maxY);

	minX = -field->padding;
	minY = -field->padding;

	if ( field->width == 0 )
	{
		int width = field->fontHeight*(field->string ? strlen(field->string) : 0);
		maxX = field->padding + width;
	}
	else
		maxX = field->padding + field->width;

	if ( field->fieldHeight == 0 )
	{
		maxY = field->padding + field->fontHeight*field->nLines +
			(field->nLines-1)*field->lineSpacing;
	}
	else
		maxY = field->padding + field->fieldHeight;

	SWFRect_setBounds(CHARACTER(field)->bounds, minX, maxX, minY, maxY);
}


void
writeSWFTextFieldToMethod(SWFBlock block,
													SWFByteOutputMethod method, void *data)
{
	SWFOutput out = SWFBrowserFont_getOutput((SWFBrowserFont)block);
	SWFOutput_writeToMethod(out, method, data);
}


int
completeSWFTextField(SWFBlock block)
{
	SWFTextField field = (SWFTextField)block;

	/* we're guessing how big the block's going to be.. */
	SWFOutput out =
		newSizedSWFOutput(42 
			+ ((field->varName)?strlen(field->varName):0) 
			+ ((field->string)?strlen(field->string):0));

	field->out = out;

	resetBounds(field);

	SWFOutput_writeUInt16(out, CHARACTERID(field));
	SWFOutput_writeRect(out, CHARACTER(field)->bounds);
	SWFOutput_writeUInt16(out, field->flags);

	if(field->flags & SWFTEXTFIELD_HASFONT)
	{
		SWFOutput_writeUInt16(out, CHARACTERID(field->font.fontchar));
		SWFOutput_writeUInt16(out, field->fontHeight);
	}

	if(field->flags & SWFTEXTFIELD_HASCOLOR)
	{
		SWFOutput_writeUInt8(out, field->r);
		SWFOutput_writeUInt8(out, field->g);
		SWFOutput_writeUInt8(out, field->b);
		SWFOutput_writeUInt8(out, field->a);
	}

	if ( field->flags & SWFTEXTFIELD_HASLENGTH )
		SWFOutput_writeUInt16(out, field->length);

	if(field->flags & SWFTEXTFIELD_HASLAYOUT)
	{
		SWFOutput_writeUInt8(out, field->alignment);
		SWFOutput_writeUInt16(out, field->leftMargin);
		SWFOutput_writeUInt16(out, field->rightMargin);
		SWFOutput_writeUInt16(out, field->indentation);
		SWFOutput_writeUInt16(out, field->lineSpacing);
	}

	SWFOutput_writeString(out, (byte*) field->varName);
	if ( field->flags & SWFTEXTFIELD_HASTEXT )
		SWFOutput_writeString(out, (byte*)field->string);

	/*
		XXX - if font is a real font, do we need to talk to it?
		flash 4 just makes a browser font for (editable) textfields for all fonts
	*/

	SWFOutput_byteAlign(out);
	return SWFOutput_getLength(out);
}


void
destroySWFTextField(SWFTextField field)
{
	destroySWFOutput(field->out);

	if ( field->varName != NULL )
		free(field->varName);

	if ( field->string != NULL )
		free(field->string);

	if ( field->embeds != NULL )
		free(field->embeds);

	destroySWFCharacter((SWFCharacter) field);
}


SWFTextField
newSWFTextField()
{
	SWFRect temp_rect;

	SWFTextField field = (SWFTextField)malloc(sizeof(struct SWFTextField_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == field)
		return NULL;

	SWFCharacterInit((SWFCharacter)field);

	BLOCK(field)->writeBlock = writeSWFTextFieldToMethod;
	BLOCK(field)->complete = completeSWFTextField;
	BLOCK(field)->dtor = (destroySWFBlockMethod) destroySWFTextField;
	BLOCK(field)->type = SWF_DEFINEEDITTEXT;

	CHARACTERID(field) = ++SWF_gNumCharacters;
	temp_rect = newSWFRect(-40, 280, -40, 280);

	/* If newSWFRect() failed, return NULL to signify this */
	if (NULL == temp_rect)
	{
		free(field);
		return NULL;
	}

	CHARACTER(field)->bounds = temp_rect;

	field->out = NULL;

	field->lineSpacing = 40;
	field->padding = 40;
	field->fontHeight = 240;
	field->fieldHeight = 0;
	field->width = 0;
	field->a = 0xff;
	field->nLines = 1;

	field->flags = 0;

	field->font.font = NULL;
	field->fonttype = Unresolved;
	field->varName = NULL;
	field->string = NULL;

	field->r = 0;
	field->g = 0;
	field->b = 0;
	field->a = 0xff;

	field->length = 0;
	field->alignment = SWFTEXTFIELD_ALIGN_LEFT;
	field->leftMargin = 0;
	field->rightMargin = 0;
	field->indentation = 0;

	field->embeds = NULL;
	field->embedlen = 0;

	return field;
}

static inline int checkSWFFontCharacter(SWFFontCharacter fc)
{
	int font_flags;
	int nGlyphs;

	SWFFont font = SWFFontCharacter_getFont(fc);
	font_flags = SWFFont_getFlags(font);
	nGlyphs = SWFFontCharacter_getNGlyphs(fc);
	
	if((font_flags & SWF_FONT_HASLAYOUT) == 0 && nGlyphs == 0)
		return -1;

	return 0;
}

/* font machinery:
	if a regular font (outlines in fdb) is used, it is added to the textfield
	as type Font and later converted to a FontChar
	while a Font, characters can be added (embedded)
	an Imported font stays as is, so does a BrowserFont
 */
void
SWFTextField_setFont(SWFTextField field, SWFBlock font)
{
	if(font == NULL)
		return;
	if ( BLOCK(font)->type == SWF_BROWSERFONT )
	{
		field->fonttype = BrowserFont;
		field->font.browserFont = (SWFBrowserFont)font;
		SWFCharacter_addDependency((SWFCharacter)field, (SWFCharacter)font);
		field->flags |= SWFTEXTFIELD_HASFONT;
	}
	else if ( BLOCK(font)->type == SWF_DEFINEFONT 
		|| BLOCK(font)->type == SWF_DEFINEFONT2)
	{
		SWFFontCharacter fc = (SWFFontCharacter)font;
		if(checkSWFFontCharacter(fc))	
		{
			SWF_warn("font is empty or has no layout information\n");
			return;
		}
		field->fonttype = Imported;
		field->font.fontchar = fc;
		SWFCharacter_addDependency(
			(SWFCharacter)field, (SWFCharacter)font);
		field->flags |= SWFTEXTFIELD_HASFONT | SWFTEXTFIELD_USEFONT;
	}
	else if (BLOCK(font)->type == SWF_MINGFONT)
	{
		if(!(SWFFont_getFlags((SWFFont)font) & SWF_FONT_HASLAYOUT))
		{
			SWF_warn("font is empty or has no layout information\n");
			return;
		}
		field->fonttype = Font;
		field->font.font = (SWFFont)font;
		field->flags |= SWFTEXTFIELD_HASFONT | SWFTEXTFIELD_USEFONT;
	}
	else
		SWF_warn("SWFTextField_setFont: not a valid font object\n");
}

SWFFont
SWFTextField_getUnresolvedFont(SWFTextField field)
{	
	switch(field->fonttype)
	{	case Font:
			return field->font.font;
		default:
			return NULL;
	}
}


void
SWFTextField_addChars(SWFTextField field, const char *string)
{
	int n, len = strlen(string);
	if(field->fonttype == Font || field->fonttype == FontChar)
	{	field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + len) * 2);
		for(n = 0 ; n < len  ; n++)
			field->embeds[field->embedlen++] = string[n] & 0xff;
	}
}

void
SWFTextField_addUTF8Chars(SWFTextField field, const char *string)
{
	unsigned short *widestring;
	int n, len;
	if(field->fonttype == FontChar || field->fonttype == Font)
	{	len = UTF8ExpandString(string, &widestring);
		field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + len) * 2);
		for(n = 0 ; n < len  ; n++)
			field->embeds[field->embedlen++] = widestring[n];
		free(widestring);
	}
}

/*
	this is called when the textfield is added to the movie,
	so no more changes to it from that point
	other type may still add characters to the fontchar
 */
void SWFTextField_setFontCharacter(SWFTextField field, SWFFontCharacter fontchar)
{
	field->fonttype = FontChar;
	field->font.fontchar = fontchar;
	if((field->flags & SWFTEXTFIELD_NOEDIT) == 0)
		SWFFontCharacter_addAllChars(fontchar);
	else
		SWFFontCharacter_addWideChars(fontchar, 
				field->embeds, field->embedlen);
}
	
void
SWFTextField_setScaledBounds(SWFTextField field, int width, int height)
{
	field->width = width;
	field->fieldHeight = height;
	resetBounds(field);
}


void
SWFTextField_setFlags(SWFTextField field, int flags)
{
	field->flags |= flags; 
}


void
SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a)
{
	field->r = r;
	field->g = g;
	field->b = b;
	field->a = a;
	field->flags |= SWFTEXTFIELD_HASCOLOR;
}


void
SWFTextField_setVariableName(SWFTextField field, const char *name)
{
	field->varName = strdup(name);
}


static void
SWFTextField_addStringOnly(SWFTextField field, const char *string)
{
	int l;

	for ( l=0; string[l]!='\0'; ++l )
	{
		if ( string[l] == '\n' )
			++field->nLines;
	}

	if ( field->string )
	{
		field->string = (char*)realloc(field->string, strlen(field->string)+l+1);
		strcat(field->string, string);
	}
	else
		field->string = strdup(string);
	field->flags |= SWFTEXTFIELD_HASTEXT;
	resetBounds(field);
}

void
SWFTextField_addString(SWFTextField field, const char *string)
{
	int l, n;

	l = strlen(string);

	SWFTextField_addStringOnly(field, string);
	if(field->fonttype == FontChar || field->fonttype == Font) 
	{	field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + l) * 2);
		for(n = 0 ; n < l  ; n++)
			field->embeds[field->embedlen++] = string[n] & 0xff;
	}
}

void
SWFTextField_addUTF8String(SWFTextField field, const char *string)
{
	unsigned short *widestring;
	int l, n;

	SWFTextField_addStringOnly(field, string);
	if(field->fonttype == FontChar || field->fonttype == Font)
	{	l = UTF8ExpandString(string, &widestring);
		field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + l) * 2);
		for(n = 0 ; n < l  ; n++)
			field->embeds[field->embedlen++] = widestring[n];
		free(widestring);
	}
}


void
SWFTextField_setScaledFontHeight(SWFTextField field, int height)
{
	field->fontHeight = height;
	resetBounds(field);
}


void
SWFTextField_setScaledFieldHeight(SWFTextField field, int height)
{
	field->fieldHeight = height;
	resetBounds(field);
}


void
SWFTextField_setScaledWidth(SWFTextField field, int width)
{
	field->width = width;
	resetBounds(field);
}


void
SWFTextField_setScaledPadding(SWFTextField field, int padding)
{
	field->padding = padding;
	resetBounds(field);
}


void
SWFTextField_setScaledLeftMargin(SWFTextField field, int leftMargin)
{
	field->leftMargin = leftMargin;
	field->flags |= SWFTEXTFIELD_HASLAYOUT;
}


void
SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin)
{
	field->rightMargin = rightMargin;
	field->flags |= SWFTEXTFIELD_HASLAYOUT;
}


void
SWFTextField_setScaledIndentation(SWFTextField field, int indentation)
{
	field->indentation = indentation;
	field->flags |= SWFTEXTFIELD_HASLAYOUT;
}


void
SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing)
{
	field->lineSpacing = lineSpacing;
	field->flags |= SWFTEXTFIELD_HASLAYOUT;
	resetBounds(field);
}


void
SWFTextField_setAlignment(SWFTextField field, SWFTextFieldAlignment alignment)
{
	field->alignment = alignment;
	field->flags |= SWFTEXTFIELD_HASLAYOUT;
}


void
SWFTextField_setLength(SWFTextField field, int length)
{
	field->length = length;
	field->flags |= SWFTEXTFIELD_HASLENGTH;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
