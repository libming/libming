/* ====================================================================
 * Copyright (c) 2000-2002 by Soheil Seyfaie. All rights reserved.
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


MODULE = SWF::FontCharacter	PACKAGE = SWF::FontCharacter	PREFIX = SWFFontCharacter_

PROTOTYPES: ENABLE

void SWFFontCharacter_addChars(font, string)
	SWF::FontCharacter font
	const char *string


void SWFFontCharacter_addUTF8Chars(font, string)
	SWF::FontCharacter font
	const char *string

void SWFFontCharacter_addAllChars(font)
	SWF::FontCharacter font

void
SWFFontCharacter_DESTROY(fc)
	SWF::FontCharacter	fc
	CODE:
        S_DEBUG(2, fprintf(stderr, "FontCharacter DESTROY CALLED\n"));

