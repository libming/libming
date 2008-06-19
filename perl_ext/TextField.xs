/* ====================================================================
 * Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Id$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::TextField	    PACKAGE = SWF::TextField	PREFIX = SWFTextField_
PROTOTYPES: ENABLE

SWF::TextField
SWFTextField_new(package="SWF::TextField", ...)
	char *package
        PREINIT:
        int flags;
	CODE:
        RETVAL = newSWFTextField();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);
        if(items>1){
            flags = (int)SvIV(ST(1));
            SWFTextField_setFlags(RETVAL, flags);
        }

void
destroySWFTextField(field)
        SWF::TextField	field
        ALIAS:
        SWF::TextField::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "TextField DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFTextField(field);

void
SWFTextField_setFont(field, font)
	SWF::TextField	field
	SWF::Block	font = (SWF__Block) SvIV((SV*)SvRV(ST(1)));
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFTextField_setFont(field, font);


void
SWFTextField_setBounds(field, width, height)
	SWF::TextField	field
	int	width
	int	height

void
SWFTextField_setFlags(field, flags)
	SWF::TextField	field
	int	flags

void
SWFTextField_setColor(field, r, g, b, a=0xff)
	SWF::TextField	field
	byte	r
	byte	g
	byte	b
	byte	a

void
SWFTextField_setVariableName(field, name)
	SWF::TextField	field
	char *	name
        ALIAS:
        SWF::TextField::setName = 1

void
SWFTextField_addString(field, string)
	SWF::TextField	field
	char *	string

void
SWFTextField_addUTF8String(field, string)
	SWF::TextField	field
	char *	string

void
SWFTextField_setHeight(field, height)
	SWF::TextField	field
	int	height

void
SWFTextField_setPadding(field, padding)
	SWF::TextField	field
	int	padding

void
SWFTextField_setMargins(field, left, right)
	SWF::TextField	field
	int	left
        int     right
        CODE:
        SWFTextField_setLeftMargin(field, left);
        SWFTextField_setRightMargin(field, right);


void
SWFTextField_setLeftMargin(field, leftMargin)
	SWF::TextField	field
	int	leftMargin

void
SWFTextField_setRightMargin(field, rightMargin)
	SWF::TextField	field
	int	rightMargin

void
SWFTextField_setIndentation(field, indentation)
	SWF::TextField	field
	int	indentation

void
SWFTextField_setLineSpacing(field, lineSpacing)
	SWF::TextField	field
	int	lineSpacing

void
SWFTextField_setAlignment(field, alignment)
	SWF::TextField	field
	int	alignment
        ALIAS:
        SWF::TextField::align =  1

void
SWFTextField_addChars(field, string)
	SWF::TextField	field
	char *	string

void
SWFTextField_addUTF8Chars(field, string)
	SWF::TextField	field
	char *	string

void 
SWFTextField_setLength(field, len)
	SWF::TextField field
	int len

void 
SWFTextField_setFieldHeight(field, height)
	SWF::TextField field
	int height

