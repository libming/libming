/* ====================================================================
 * Copyright (c) 2007 Klaus Rechert All rights reserved.
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


MODULE = SWF::BrowserFont		PACKAGE = SWF::BrowserFont		PREFIX = SWFBrowserFont_
PROTOTYPES: ENABLE

SWF::BrowserFont
SWFBrowserFont_new(package="SWF::BrowserFont", name)
	char *package
	char *name
	CODE:
	RETVAL = newSWFBrowserFont(name);

	ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
destroySWFBrowserFont(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::BrowserFont::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "BROWSERFONT DESTROY CALLED\n"));
	destroySWFBrowserFont((SWFBrowserFont)block);
