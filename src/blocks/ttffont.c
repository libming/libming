/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
 *  Copyright (C) 2007 Klaus Rechert
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
#include "ming_config.h"
#if USE_FREETYPE
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_OUTLINE_H

// Methods of FT_Outline_Funcs take a 'const FT_Vector*' in 2.2
// and a non-const one in 2.1, so we use an FT_CONST macro to
// support both
#if FREETYPE_MAJOR == 2 && FREETYPE_MINOR < 2
#define FT_CONST 
#else
#define FT_CONST const
#endif

#include "shape.h"
#include "font.h"

struct outl_data
{
	SWFShape shape;
	double ratio_EM;
}; 

static int
outl_moveto(FT_CONST FT_Vector *to, void *user)
{
	struct outl_data *data = (struct outl_data *)user;
	SWFShape shape = data->shape;
	double ratio_EM = data->ratio_EM;

	int dx = (int)(to->x*ratio_EM);
	int dy = -(int)(to->y*ratio_EM);

	SWFShape_moveScaledPenTo(shape, dx, dy);	

	return 0;
}

static int
outl_lineto(FT_CONST FT_Vector *to, void *user)
{
	struct outl_data *data = (struct outl_data *)user;
	SWFShape shape = data->shape;
	double ratio_EM = data->ratio_EM;

	int x = (int)(to->x*ratio_EM);
	int y = -(int)(to->y*ratio_EM);

	SWFShape_drawScaledLineTo(shape, x, y);
	return 0;
}

static int
outl_conicto(FT_CONST FT_Vector *ctl, FT_CONST FT_Vector *to, void *user)
{
	struct outl_data *data = (struct outl_data *)user;
	SWFShape shape = data->shape;
	double ratio_EM = data->ratio_EM;

	int cx = (int)(ctl->x*ratio_EM);
	int cy = -(int)(ctl->y*ratio_EM);
	int ax = (int)(to->x*ratio_EM);
	int ay = -(int)(to->y*ratio_EM);

	SWFShape_drawScaledCurveTo(shape, cx, cy, ax, ay);

	return 0;
}

static int
outl_cubicto(FT_CONST FT_Vector *ctl1, FT_CONST FT_Vector *ctl2,
             FT_CONST FT_Vector *to, void *user)
{
	FT_Vector midpnt;
	int cx, cy, ax, ay;
	struct outl_data *data = (struct outl_data *)user;
	SWFShape shape = data->shape;
	double ratio_EM = data->ratio_EM;

	/* This is handled by breaking the cubic into 2 conic segments */
	midpnt.x=(ctl1->x+ctl2->x)/2;
	midpnt.y=(ctl1->y+ctl2->y)/2;

	/* First half */
	cx = (int)(ctl1->x*ratio_EM);
	cy = -(int)(ctl1->y*ratio_EM);
	ax = (int)(midpnt.x*ratio_EM);
	ay = -(int)(midpnt.y*ratio_EM);
	SWFShape_drawScaledCurveTo(shape, cx, cy, ax, ay);

	/* Second half */
	cx = (int)(ctl2->x*ratio_EM);
	cy = -(int)(ctl2->y*ratio_EM);
	ax = (int)(to->x*ratio_EM);
	ay = -(int)(to->y*ratio_EM);
	SWFShape_drawScaledCurveTo(shape, cx, cy, ax, ay);
	return 0;
}

static FT_Outline_Funcs ft_outl_funcs = {
	outl_moveto,
	outl_lineto,
	outl_conicto,
	outl_cubicto,
	0,
	0
};

	
static void readGlyphs(SWFFont font, FT_Face face)
{
	int glyphCount = 0;
	FT_UInt gindex;
	FT_ULong charcode;
	double ratio_EM = 1024.0 / face->units_per_EM;

	font->shapes = (SWFShape *)malloc(sizeof(SWFShape) * face->num_glyphs);
	font->advances = (short *)malloc(sizeof(short) * face->num_glyphs);
	font->glyphToCode = (unsigned short *)malloc(sizeof(unsigned short) * face->num_glyphs);
	charcode = FT_Get_First_Char(face, &gindex );
	while ( gindex != 0 ) 
	{
		struct outl_data data;
		if( FT_Load_Glyph(face, gindex, FT_LOAD_NO_BITMAP|FT_LOAD_NO_SCALE)) 
		{
			SWF_warn("readGlyphsTTF: Can't load glyph %d, skipped\n", gindex);
			charcode = FT_Get_Next_Char(face, charcode, &gindex);
			continue;
		}
		
		data.shape = newSWFGlyphShape();
		data.ratio_EM = ratio_EM;	
		if(FT_Outline_Decompose(&(face->glyph->outline), 
			&ft_outl_funcs, &data)) 
		{
			SWF_warn("readGlyphsTTF: Can't decompose outline for glyph %d\n", gindex);
			destroySWFShape(data.shape);
			charcode = FT_Get_Next_Char(face, charcode, &gindex);
			continue;
		}
		font->shapes[glyphCount] = data.shape;
		font->glyphToCode[glyphCount] = charcode;
		font->advances[glyphCount] = (short)(face->glyph->advance.x * ratio_EM);
		if(charcode > 255)
			font->flags |= SWF_FONT_WIDECODES;
		charcode = FT_Get_Next_Char(face, charcode, &gindex);
		glyphCount++;
	}
	font->nGlyphs = glyphCount;

	if(font->nGlyphs > 255) // XXX: very simple estimation right now
		font->flags |=  SWF_FONT_WIDEOFFSETS;
}

static SWFFont loadFontFromFace(FT_Face face)
{
	// FT_CharMap charmap = NULL;
	SWFFont font;
	double ratio_EM;

	/*
	for(i=0; i < face->num_charmaps; i++) 
	{
		printf("map %d encoding pid=%d eid=%d\n", i,
			face->charmaps[i]->platform_id,
			face->charmaps[i]->encoding_id);
		if( face->charmaps[i]->platform_id == TT_PLATFORM_MACINTOSH &&
			face->charmaps[i]->encoding_id == TT_MAC_ID_ROMAN ) 
		{
			charmap = face->charmaps[i];
			break;
		}
	}
	
	if( charmap == NULL ) 
	{
		SWF_warn("loadSWFFontTTF: Unable to find an ANSI charactermap.");
		goto error_face;
	}
	FT_Set_Charmap(face, charmap);
	*/

	font = newSWFFont();
	font->flags = SWF_FONT_WIDECODES | SWF_FONT_HASLAYOUT;
	font->name = strdup(face->family_name);
	font->langCode = 0;

	if( face->style_flags & FT_STYLE_FLAG_BOLD) 
		font->flags |= SWF_FONT_ISBOLD ; 

	if( face->style_flags & FT_STYLE_FLAG_ITALIC ) 
		font->flags |= SWF_FONT_ISITALIC;
	
	readGlyphs(font, face); 
	
	ratio_EM = 1024.0 / face->units_per_EM;
	font->ascent = (short)(face->ascender * ratio_EM);
	font->descent = (short)(face->descender * -ratio_EM);
	font->leading = ((face->height-face->ascender + face->descender) * ratio_EM);

	SWFFont_buildReverseMapping(font);
	return font;
}

SWFFontCollection loadTTFCollection(const char *filename)
{
	FT_Error error;
	FT_Library library;
	FT_Face face;
	int numFaces, i;
	SWFFontCollection collection;
	SWFFont font;

	if( FT_Init_FreeType( &library ) ) {
		SWF_warn("loadSWFFontTTF: FreeType initialization failed\n");
		return NULL;
	}

	if( (error = FT_New_Face(library, filename, 0, &face )) ) 
	{
		if ( error == FT_Err_Unknown_File_Format )
			SWF_warn("loadTTFCollection: %s has format unknown to FreeType\n",
				filename);
		else
			SWF_warn("loadTTFCollection: Cannot access %s ****\n", filename);
		goto error_ft;
	}
	numFaces = face->num_faces;
	collection = newSWFFontCollection();
	
	font = loadFontFromFace(face);
	SWFFontCollection_addFont(collection, font);

	for(i = 1; i < numFaces; i++)
	{
		if((error = FT_New_Face(library, filename, i, &face ))) 
			goto error_ft;
		font = loadFontFromFace(face);
		SWFFontCollection_addFont(collection, font);
	}
	return collection;

error_ft:
	FT_Done_FreeType(library);
	return NULL;	
}

SWFFont loadSWFFontTTF(const char *filename)
{
	FT_Error error;
	FT_Library library;
	FT_Face face;
	SWFFont font;
		
	if( FT_Init_FreeType( &library ) ) {
		SWF_warn("loadSWFFontTTF: FreeType initialization failed\n");
		return NULL;
	}

	if( (error = FT_New_Face( library, filename, 0, &face )) ) 
	{
		if ( error == FT_Err_Unknown_File_Format )
			SWF_warn("loadSWFFontTTF: %s has format unknown to FreeType\n",
				filename);
		else
			SWF_warn("loadSWFFontTTF: Cannot access %s ****\n", filename);
		goto error_ft;
	}
	
	font = loadFontFromFace(face);
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return font;

error_ft:
	FT_Done_FreeType(library);
	return NULL;
}
#endif // USE_FREETYPE
