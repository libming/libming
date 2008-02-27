/* ====================================================================
 * Copyright (c) 2008 by Klaus Rechert. All rights reserved.
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

MODULE = SWF::Blur	PACKAGE = SWF::Blur		PREFIX = SWFBlur_
PROTOTYPES: ENABLE

SWF::Blur
SWFBlur_new(package="SWF::Blur", blurX, blurY, passes)
	char *package
	float blurX
	float blurY
	int passes
	CODE:
        RETVAL = newSWFBlur(blurX, blurY, passes);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
SWFBlur_DESTROY(blur)
	SWF::Blur blur
	CODE:
	destroySWFBlur(blur);
        S_DEBUG(2, fprintf(stderr, "Blur DESTROY CALLED\n"));
