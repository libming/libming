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

MODULE = SWF::FilterMatrix	PACKAGE = SWF::FilterMatrix		PREFIX = SWFFilterMatrix_
PROTOTYPES: ENABLE

SWF::FilterMatrix
SWFFilterMatrix_new(package="SWF::FilterMatrix", cols, rows, vals)
	char *package
	int cols
	int rows
	SV * vals
	INIT:
	int i;
	I32 count = cols * rows;
	if(!SvROK(vals) || (av_len((AV *)SvRV(vals)) + 1)  != count)
		XSRETURN_UNDEF;
	CODE:
	float *f_vals = (float *)malloc(count * sizeof(float));
	for(i = 0; i < count; i++)
	{
		float f = (float)SvNV(*av_fetch((AV *)SvRV(vals), i, 0));
		f_vals[i] = f;
	}
        RETVAL = newSWFFilterMatrix(cols, rows, f_vals);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);
	free(f_vals);

void
SWFFilterMatrix_DESTROY(fm)
	SWF::FilterMatrix fm
	CODE:
	destroySWFFilterMatrix(fm);
        S_DEBUG(2, fprintf(stderr, "SWFFilterMatrix DESTROY CALLED\n"));
