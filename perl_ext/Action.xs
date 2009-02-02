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

MODULE = SWF::Action	PACKAGE = SWF::Action		PREFIX = SWFAction_
PROTOTYPES: ENABLE

SWF::Action
SWFAction_new(package="SWF::Action", script)
	char *package
        char *script
	CODE:
	
        RETVAL = newSWFAction(script);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
SWFAction_DESTROY(action)
	SWF::Action	action
	CODE:
        S_DEBUG(2, fprintf(stderr, "Action DESTROY CALLED\n"));

int 
SWFAction_compile(action, version)
	SWF::Action	action
	int version
	PREINIT:
	int len;
	CODE:
	 RETVAL = SWFAction_compile(action, version, &len);
	OUTPUT:
	 RETVAL
	
