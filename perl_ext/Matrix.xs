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

float
SWFMatrix_getScaleX(m)
	SWF::Matrix m

float
SWFMatrix_getScaleY(m)
	SWF::Matrix m

float 
SWFMatrix_getRotate0(m)
	SWF::Matrix m

float 
SWFMatrix_getRotate1(m)
	SWF::Matrix m

int
SWFMatrix_getTranslateX(m)
	SWF::Matrix m

int 
SWFMatrix_getTranslateY(m)
	SWF::Matrix m
