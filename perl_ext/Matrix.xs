/* ====================================================================
 * Copyright (c) 2008 by Klaus Rechert. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Author$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::Matrix    PACKAGE = SWF::Matrix     PREFIX = SWFMatrix_
PROTOTYPES: ENABLE


void
SWFMatrix_DESTROY(m)
	SWF::Matrix	m
	CODE:
        S_DEBUG(2, fprintf(stderr, "Matrix DESTROY CALLED\n"));
        /* currently no destroy(m) call here,
        see destroySWFPlaceObject2Block() in placeobject.c */

double
SWFMatrix_getScaleX(m)
	SWF::Matrix m

double
SWFMatrix_getScaleY(m)
	SWF::Matrix m

double
SWFMatrix_getRotate0(m)
	SWF::Matrix m

double 
SWFMatrix_getRotate1(m)
	SWF::Matrix m

int
SWFMatrix_getTranslateX(m)
	SWF::Matrix m

int 
SWFMatrix_getTranslateY(m)
	SWF::Matrix m
