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


MODULE = SWF::Text	PACKAGE = SWF::Text		PREFIX = SWFText_
PROTOTYPES: ENABLE

SWF::Text
SWFText_new(package="SWF::Text")
	char *package
	CODE:
        RETVAL = newSWFText2();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


void
destroySWFText(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Text::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "Text DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFText(block);


void
SWFText_setFont(text, font)
	SWF::Text	text
	SWF::Font	font
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFText_setFont(text, font);

void
SWFText_setHeight(text, height)
	SWF::Text	text
	int		height

void
SWFText_moveTo(text, x, y)
	SWF::Text	text
	int		x
	int		y

void
SWFText_setColor(text, r, g, b, a=0xff)
	SWF::Text	text
	byte		r
	byte		g
	byte		b
	byte		a

void
SWFText_addString(text, string, advance=NULL)
	SWF::Text	text
	char *		string
	int *		advance
	

void
SWFText_setSpacing(text, spacing)
	SWF::Text	text
	float		spacing

int
SWFText_getStringWidth(text, string)
	SWF::Text	text
	char *		string
        ALIAS:
        SWF::Text::getWidth = 1

short
SWFText_getAscent(text)
	SWF::Text	text

short
SWFText_getDescent(text)
	SWF::Text	text

short
SWFText_getLeading(text)
	SWF::Text	text
