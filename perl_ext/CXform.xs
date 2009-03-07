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

MODULE = SWF::CXform	PACKAGE = SWF::CXform		PREFIX = SWFCXform_
PROTOTYPES: ENABLE

SWF::CXform
SWFCXform_new(package="SWF::CXform", rAdd=0, gAdd=0, bAdd=0, aAdd=0, rMult=1.0, gMult=1.0, bMult=1.0, aMult=1.0)
	char *package
	float rAdd
	float gAdd
	float bAdd
	float aAdd
	float rMult
	float gMult
	float bMult
	float aMult
	CODE:
	RETVAL = newSWFCXform(rAdd, gAdd, bAdd, aAdd, rMult, gMult, bMult, aMult);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), package, (void*)RETVAL);

void
SWFCXform_setColorAdd(cx, rAdd, gAdd, bAdd, aAdd)
	SWF::CXform cx
	float rAdd
	float gAdd
	float bAdd
	float aAdd

void
SWFCXform_setColorMult(cx, rMult, gMult, bMult, aMult)
	SWF::CXform cx
	float rMult
	float gMult
	float bMult
	float aMult

void
SWFCXform_DESTROY(cx)
	SWF::CXform cx
	CODE:
/*	destroySWFCXform(cx);					*/
/*	removed March 07th, 2009				*/
/*	because could be destroyed twice (and segfaults later)  */
/*	see destroySWFPlaceObject2Block() in placeobject.c 	*/
	S_DEBUG(2, fprintf(stderr, "CXform DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
