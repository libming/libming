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

MODULE = SWF::Filter	PACKAGE = SWF::Filter		PREFIX = SWFFilter_
PROTOTYPES: ENABLE

SWF::Filter
newColorMatrixFilter(matrix)
	SWF::FilterMatrix matrix 
	CODE:
        RETVAL = newColorMatrixFilter(matrix);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);


SWF::Filter
newBlurFilter(blur)
	SWF::Blur blur
	CODE:
        RETVAL = newBlurFilter(blur);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

SWF::Filter
newGradientBevelFilter(gradient, blur, shadow, flags)
	SWF::Gradient gradient
	SWF::Blur blur
	SWF::Shadow shadow
	int flags
	CODE:
        RETVAL = newGradientBevelFilter(gradient, blur, shadow, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

SWF::Filter
newGradientGlowFilter(gradient, blur, shadow, flags)
	SWF::Gradient gradient
	SWF::Blur blur
	SWF::Shadow shadow
	int flags
	CODE:
        RETVAL = newGradientGlowFilter(gradient, blur, shadow, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

SWF::Filter 
newBevelFilter(shadowColor, highlightColor, blur, shadow, flags)
	SV* shadowColor
	SV* highlightColor
	SWF::Blur blur
	SWF::Shadow shadow
	int flags
	PREINIT:
	SWFColor c1;
	SWFColor c2;
	INIT:
	if(!SvROK(shadowColor) || av_len((AV *)SvRV(shadowColor)) < 2)
		XSRETURN_UNDEF;
	if(!SvROK(highlightColor) || av_len((AV *)SvRV(highlightColor)) < 2)
		XSRETURN_UNDEF;
	CODE:
	c1.red = (char)SvNV(*av_fetch((AV *)SvRV(shadowColor), 0, 0));
	c1.green = (char)SvNV(*av_fetch((AV *)SvRV(shadowColor), 1, 0));
	c1.blue = (char)SvNV(*av_fetch((AV *)SvRV(shadowColor), 2, 0));
	if(av_len((AV *)SvRV(shadowColor)) == 3)
		c1.alpha = (char)SvNV(*av_fetch((AV *)SvRV(shadowColor), 3, 0));
	else
		c1.alpha = 255;

	c2.red = (char)SvNV(*av_fetch((AV *)SvRV(highlightColor), 0, 0));
	c2.green = (char)SvNV(*av_fetch((AV *)SvRV(highlightColor), 1, 0));
	c2.blue = (char)SvNV(*av_fetch((AV *)SvRV(highlightColor), 2, 0));
	if(av_len((AV *)SvRV(highlightColor)) == 3)
		c2.alpha = (char)SvNV(*av_fetch((AV *)SvRV(highlightColor), 3, 0));
	else
		c2.alpha = 255;

	RETVAL = newBevelFilter(c1, c2, blur, shadow, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);



SWF::Filter
newConvolutionFilter(matrix, divisor, bias, color, flags)
	SWF::FilterMatrix matrix
	float divisor
	float bias
	SV * color
	int flags
	PREINIT:
	SWFColor c;
	INIT:
	if(!SvROK(color) || av_len((AV *)SvRV(color)) < 2)
		XSRETURN_UNDEF;
	CODE:
	c.red = (char)SvNV(*av_fetch((AV *)SvRV(color), 0, 0));
	c.green = (char)SvNV(*av_fetch((AV *)SvRV(color), 1, 0));
	c.blue = (char)SvNV(*av_fetch((AV *)SvRV(color), 2, 0));
	if(av_len((AV *)SvRV(color)) == 3)
		c.alpha = (char)SvNV(*av_fetch((AV *)SvRV(color), 3, 0));
	else
		c.alpha = 255;
        RETVAL = newConvolutionFilter(matrix, divisor, bias, c, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

SWF::Filter
newGlowFilter(color, blur, strength, flags)
	SWF::Blur blur
	float strength
	SV * color
	int flags
	PREINIT:
	SWFColor c;
	INIT:
	if(!SvROK(color) || av_len((AV *)SvRV(color)) < 2)
		XSRETURN_UNDEF;
	CODE:
	c.red = (char)SvNV(*av_fetch((AV *)SvRV(color), 0, 0));
	c.green = (char)SvNV(*av_fetch((AV *)SvRV(color), 1, 0));
	c.blue = (char)SvNV(*av_fetch((AV *)SvRV(color), 2, 0));
	if(av_len((AV *)SvRV(color)) == 3)
		c.alpha = (char)SvNV(*av_fetch((AV *)SvRV(color), 3, 0));
	else
		c.alpha = 255;
        RETVAL = newGlowFilter(c, blur, strength, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

SWF::Filter
newDropShadowFilter(color, blur, shadow, flags)
	SWF::Blur blur
	SWF::Shadow shadow
	SV * color
	int flags
	PREINIT:
	SWFColor c;
	INIT:
	/* av_len returns the highest index in the array. */
	/* The number of elements in the array is "av_len(av) + 1" */
	/* see "man perlapi" or "man perlguts" */
	if(!SvROK(color) || av_len((AV *)SvRV(color)) < 2)
		XSRETURN_UNDEF;
	CODE:
	c.red = (char)SvNV(*av_fetch((AV *)SvRV(color), 0, 0));
	c.green = (char)SvNV(*av_fetch((AV *)SvRV(color), 1, 0));
	c.blue = (char)SvNV(*av_fetch((AV *)SvRV(color), 2, 0));
	if(av_len((AV *)SvRV(color)) == 3)
		c.alpha = (char)SvNV(*av_fetch((AV *)SvRV(color), 3, 0));
	else
		c.alpha = 255;
        RETVAL = newDropShadowFilter(c, blur, shadow, flags);
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Filter", (void*)RETVAL);

void
SWFFilter_DESTROY(filter)
	SWF::Filter filter
	CODE:
	destroySWFFilter(filter);
        S_DEBUG(2, fprintf(stderr, "SWFFilter DESTROY CALLED\n"));
