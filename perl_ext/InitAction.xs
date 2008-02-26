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

MODULE = SWF::InitAction	PACKAGE = SWF::InitAction		PREFIX = SWFInitAction_
PROTOTYPES: ENABLE

SWF::InitAction
SWFInitAction_new(package="SWF::Action", action)
	char *package
	SWF::Action action
	CODE:
        RETVAL = newSWFInitAction(action);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
SWFInitAction_DESTROY(action)
	SWF::InitAction	action
	CODE:
        S_DEBUG(2, fprintf(stderr, "InitAction DESTROY CALLED\n"));
