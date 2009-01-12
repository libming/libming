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

MODULE = SWF::Shadow	PACKAGE = SWF::Shadow		PREFIX = SWFShadow_
PROTOTYPES: ENABLE

SWF::Shadow
SWFShadow_new(package="SWF::Shadow", angle, distance, strength)
	char *package
	float angle
	float distance
	float strength
	CODE:
        RETVAL = newSWFShadow(angle, distance, strength);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
SWFShadow_DESTROY(shadow)
	SWF::Shadow shadow
	CODE:
	destroySWFShadow(shadow);
        S_DEBUG(2, fprintf(stderr, "Shadow DESTROY CALLED\n"));
