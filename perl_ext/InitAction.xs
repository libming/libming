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
SWFInitAction_new(package="SWF::Action", action, id=-1)
	char *package
	SWF::Action action
	int id
	CODE:
	if (id==-1) {
        	RETVAL = newSWFInitAction(action);
	} else {
        	RETVAL = newSWFInitAction_withId(action, id);
	}
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


void
SWFInitAction_DESTROY(action)
	SWF::InitAction	action
	CODE:
        S_DEBUG(2, fprintf(stderr, "InitAction DESTROY CALLED\n"));
