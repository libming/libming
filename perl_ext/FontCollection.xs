/* ====================================================================
 * Copyright (c) 2008 Klaus Rechert All rights reserved.
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


MODULE = SWF::FontCollection		PACKAGE = SWF::FontCollection		PREFIX = SWFFontCollection_
PROTOTYPES: ENABLE

SWF::FontCollection
SWFFontCollection_new(package="SWF::FontCollection", filename)
	char *package
	char *filename
	CODE:
	RETVAL = newSWFFontCollection_fromFile(filename);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), package, (void*)RETVAL);

void
destroySWFFontCollection(block)
	SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
	ALIAS:
	SWF::FontCollection::DESTROY = 1
	CODE:
	destroySWFFontCollection((SWFFontCollection)block);

int 
SWFFontCollection_getFontCount(fc)
	SWF::FontCollection fc

SWF::Font
SWFFontCollection_getFont(fc, index)
	SWF::FontCollection fc
	int index
	CODE:
	RETVAL = SWFFontCollection_getFont(fc, index);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Font", (void*)RETVAL);
