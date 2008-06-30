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


MODULE = SWF::Character	PACKAGE = SWF::Character	PREFIX = SWFCharacter_

PROTOTYPES: ENABLE

float
SWFCharacter_getWidth(c)
	SWF::Character c

float
SWFCharacter_getHeight(c)
	SWF::Character c

void
SWFCharacter_DESTROY(c)
	SWF::Character	c
	CODE:
        S_DEBUG(2, fprintf(stderr, "Character DESTROY CALLED\n"));

