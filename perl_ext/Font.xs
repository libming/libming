/* ====================================================================
 * Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Author$
 * $Id$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::Font		PACKAGE = SWF::Font		PREFIX = SWFFont_
PROTOTYPES: ENABLE

SWF::Font
SWFFont_new(package="SWF::Font", filename)
	char *package
	char *filename
	CODE:
	RETVAL = newSWFFont_fromFile(filename);
	ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
destroySWFFont(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Font::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "FONT DESTROY CALLED\n"));
	destroySWFBlock((SWFBlock)block);

float
SWFFont_getStringWidth(font, string)
	SWF::Font	font
	char *	string
        ALIAS:
        SWF::Font::getWidth = 1

float
SWFFont_getUTF8StringWidth(font, string)
	SWF::Font	font
	char *	string

short
SWFFont_getAscent(font)
	SWF::Font	font

short
SWFFont_getDescent(font)
	SWF::Font	font

short
SWFFont_getLeading(font)
	SWF::Font	font

const char *
SWFFont_getName(font)
        SWF::Font font

int 
SWFFont_getGlyphCount(font)
        SWF::Font font


