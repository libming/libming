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

static void callXS (void (*subaddr)(CV* cv), CV *cv, SV **mark) {
	int items;
	dSP;
	PUSHMARK (mark);
	(*subaddr)(cv);
	PUTBACK;  
}



MODULE = SWF		PACKAGE = SWF			PREFIX = Ming_
PROTOTYPES: ENABLE

INCLUDE: Boot.xsh

void
fileOutputMethod(b, data)
	byte	b
	void *	data


void 
Ming_setScale(scale)
    float scale

void 
Ming_useSWFVersion(version)
    int version
    ALIAS:
        SWF::setVersion = 1

















