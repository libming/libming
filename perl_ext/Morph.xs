/* ====================================================================
 * Copyright (c) 2000-2003 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * Author: soheil 
 * $Id$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::Morph	PACKAGE = SWF::Morph		PREFIX = SWFMorph_
PROTOTYPES: ENABLE

SWF::Morph
SWFMorph_new(package="SWF::Morph")
	char *package
	CODE:
        RETVAL = newSWFMorphShape();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


void
destroySWFMorph(morph)
	SWF::Morph	morph
        ALIAS:
        SWF::Morph::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Morph DESTROY CALLED\n"));
        destroySWFMorph(morph);


SWF::Shape
SWFMorph_getShape1(morph)
	SWF::Morph	morph
	PREINIT:
	SV *sv;
	CODE:
        sv = (SV*)SvRV(ST(0));
	RETVAL = SWFMorph_getShape1(morph);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Shape", (void*)RETVAL);
	swf_stash_refcnt_inc(sv, (SV*)SvRV(ST(0)));	

SWF::Shape
SWFMorph_getShape2(morph)
	SWF::Morph	morph
	PREINIT:
	SV *sv;
	CODE:
        sv = (SV*)SvRV(ST(0));
	RETVAL = SWFMorph_getShape2(morph);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Shape", (void*)RETVAL);
	swf_stash_refcnt_inc(sv, (SV*)SvRV(ST(0)));
