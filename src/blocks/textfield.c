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

#include <stdlib.h>
#include <string.h>

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
		newSizedSWFOutput(42 + ((field->varName)?strlen(field->varName):0) +
											((field->string)?strlen(field->string):0));

	field->out = out;

	resetBounds(field);

	SWFOutput_writeUInt16(out, CHARACTERID(field));
	SWFOutput_writeRect(out, CHARACTER(field)->bounds);

// fix flags here...
	if(field->fonttype == Imported)
	{	if(!field->font.fontchar)
			SWF_error("no font given for textfield\n");
	}
	else if(field->fonttype != BrowserFont)
	{	if(!field->font.fontchar)
			SWF_error("no font given for textfield\n");
		else if((SWFFont_getFlags(SWFFontCharacter_getFont(field->font.fontchar)) & SWF_FONT_HASLAYOUT) == 0 ||
				SWFFontCharacter_getNGlyphs(field->font.fontchar) == 0)
			field->flags &= ~SWFTEXTFIELD_USEFONT;
	}
	else
		field->flags &= ~SWFTEXTFIELD_USEFONT;
	if(field->string && *field->string)
		field->flags |= SWFTEXTFIELD_HASTEXT;

	SWFOutput_writeUInt16(out, field->flags);
	SWFOutput_writeUInt16(out, CHARACTERID(field->font.fontchar));
	SWFOutput_writeUInt16(out, field->fontHeight);
	SWFOutput_writeUInt8(out, field->r);
	SWFOutput_writeUInt8(out, field->g);
	SWFOutput_writeUInt8(out, field->b);
	SWFOutput_writeUInt8(out, field->a);

	if ( field->flags & SWFTEXTFIELD_HASLENGTH )
		SWFOutput_writeUInt16(out, field->length);

	SWFOutput_writeUInt8(out, field->alignment);

	SWFOutput_writeUInt16(out, field->leftMargin);
	SWFOutput_writeUInt16(out, field->rightMargin);
	SWFOutput_writeUInt16(out, field->indentation);
	SWFOutput_writeUInt16(out, field->lineSpacing);

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
	SWFTextField field = (SWFTextField)malloc(sizeof(struct SWFTextField_s));

	SWFCharacterInit((SWFCharacter)field);

	BLOCK(field)->writeBlock = writeSWFTextFieldToMethod;
	BLOCK(field)->complete = completeSWFTextField;
	BLOCK(field)->dtor = (destroySWFBlockMethod) destroySWFTextField;
	BLOCK(field)->type = SWF_DEFINEEDITTEXT;

	CHARACTERID(field) = ++SWF_gNumCharacters;
	CHARACTER(field)->bounds = newSWFRect(-40, 280, -40, 280);

	field->out = NULL;

	field->lineSpacing = 40;
	field->padding = 40;
	field->fontHeight = 240;
	field->fieldHeight = 0;
	field->width = 0;
	field->a = 0xff;
	field->nLines = 1;

	field->flags = 0x302d;

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

/* font machinery:
	if a regular font (outlines in fdb) is used, it is added to the textfield
	as type Font and later converted to a FontChar
	while a Font, characters can be added (embedded)
	an Imported font stays as is, so does a BrowserFont
 */
void
SWFTextField_setFont(SWFTextField field, SWFBlock font)
{	
	if ( BLOCK(font)->type == SWF_DEFINEEDITTEXT )
	{
		field->fonttype = BrowserFont;
		field->font.browserFont = (SWFBrowserFont)font;
		SWFCharacter_addDependency((SWFCharacter)field, (SWFCharacter)font);
	}
	else if ( BLOCK(font)->type == SWF_DEFINEFONT )
	{
		field->fonttype = Imported;
		field->font.fontchar = (SWFFontCharacter)font;
		SWFCharacter_addDependency((SWFCharacter)field, (SWFCharacter)font);
	}
	else
	{
		field->fonttype = Font;
		field->font.font = (SWFFont)font;
	}
}

SWFFont
SWFTextField_getUnresolvedFont(SWFTextField field)
{	switch(field->fonttype)
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
	if((field->fonttype == Font) && field->font.font &&
	 (SWFFont_getFlags(field->font.font) & SWF_FONT_HASLAYOUT))
	{	field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + len) * 2);
		for(n = 0 ; n < len  ; n++)
			field->embeds[field->embedlen++] = string[n] & 0xff;
		field->flags |= SWFTEXTFIELD_USEFONT;
	}
}

void
SWFTextField_addUTF8Chars(SWFTextField field, const char *string)
{
	unsigned short *widestring;
	int n, len;
	if((field->fonttype == Font) && field->font.font &&
	 (SWFFont_getFlags(field->font.font) & SWF_FONT_HASLAYOUT))
	{	len = UTF8ExpandString(string, &widestring);
		field->embeds = (unsigned short *)realloc(
			field->embeds, (field->embedlen + len) * 2);
		for(n = 0 ; n < len  ; n++)
			field->embeds[field->embedlen++] = widestring[n];
		field->flags |= SWFTEXTFIELD_USEFONT;
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
	SWFFontCharacter_addWideChars(fontchar, field->embeds, field->embedlen);
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
	field->flags = (flags & SWFTEXTFIELD_OFFMASK) | SWFTEXTFIELD_ONMASK;
}


void
SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a)
{
	field->r = r;
	field->g = g;
	field->b = b;
	field->a = a;
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

	resetBounds(field);
}

void
SWFTextField_addString(SWFTextField field, const char *string)
{
	int l, n;

	l = strlen(string);

	SWFTextField_addStringOnly(field, string);
	if((field->flags & SWFTEXTFIELD_USEFONT) && (field->fonttype == FontChar) &&
	 field->font.font && (SWFFont_getFlags(field->font.font) & SWF_FONT_HASLAYOUT))
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
	if((field->flags & SWFTEXTFIELD_USEFONT) && (field->fonttype == FontChar) &&
	 field->font.font && (SWFFont_getFlags(field->font.font) & SWF_FONT_HASLAYOUT))
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
}


void
SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin)
{
	field->rightMargin = rightMargin;
}


void
SWFTextField_setScaledIndentation(SWFTextField field, int indentation)
{
	field->indentation = indentation;
}


void
SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing)
{
	field->lineSpacing = lineSpacing;
	resetBounds(field);
}


void
SWFTextField_setAlignment(SWFTextField field, SWFTextFieldAlignment alignment)
{
	field->alignment = alignment;
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
