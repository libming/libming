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


MODULE = SWF::Shape	PACKAGE = SWF::Shape		PREFIX = SWFShape_

PROTOTYPES: ENABLE

SWF::Shape
SWFShape_new(package="SWF::Shape")
     	char *package
	CODE:
        RETVAL = newSWFShape();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
destroySWFShape(shape)
	SWF::Shape 	shape
        ALIAS:
        SWF::Shape::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Shape DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFShape(shape);

void
SWFShape_movePenTo(shape, x, y)
	SWF::Shape	shape
	int	x
	int	y

void 
SWFShape_movePen(shape, x, y)
        SWF::Shape shape
        int x
        int y

float
SWFShape_getPenX(shape)
        SWF::Shape shape

float
SWFShape_getPenY(shape)
        SWF::Shape shape


void
SWFShape_getPen(shape)
        SWF::Shape shape
        PREINIT:
        float           x;
        float           y;
        PPCODE:
        SWFShape_getPen(shape, &x, &y);
        EXTEND(SP, 2);
        PUSHs(sv_2mortal(newSVnv((double)x)));
        PUSHs(sv_2mortal(newSVnv((double)y)));

void
SWFShape_drawLineTo(shape, x, y)
	SWF::Shape	shape
	int	x
	int	y

void
SWFShape_drawLine(shape, dx, dy)
	SWF::Shape	shape
	int	dx
	int	dy

void
SWFShape_drawCurveTo(shape, controlx, controly, anchorx, anchory)
	SWF::Shape	shape
	int		controlx
	int		controly
	int		anchorx
	int		anchory

void
SWFShape_drawCurve(shape, controldx, controldy, anchordx, anchordy)
	SWF::Shape	shape
	int		controldx
	int		controldy
	int		anchordx
	int		anchordy


void
SWFShape_end(shape)
	SWF::Shape	shape

int
SWFShape_setLineStyle(shape, width, r, g, b, a=0xff)
	SWF::Shape	shape
	unsigned short	width
	byte	r
	byte	g
	byte	b
	byte	a

SWF::Fill
SWFShape_addFill(shape, ...)
       SWF::Shape      shape
       PREINIT:
       CV        *cv;
       char      *my_sub;
       CODE:
       
       PUSHMARK(mark);
       if (sv_derived_from(ST(1), "SWF::Gradient"))
           my_sub = "SWF::Shape::addGradientFill";
       else if(sv_derived_from(ST(1), "SWF::Bitmap"))
           my_sub = "SWF::Shape::addBitmapFill";
       else
           my_sub = "SWF::Shape::addSolidFill";
       cv = GvCV(gv_fetchpv(my_sub, FALSE, SVt_PVCV));
#ifdef PERL_OBJECT
        (void)(*CvXSUB(cv))(cv, pPerl);
#else
        (void)(*CvXSUB(cv))(aTHXo_ cv);
#endif


SWF::Fill
SWFShape_addSolidFill(shape, r, g, b, a=0xff)
        SWF::Shape      shape
        byte            r
        byte            g
        byte            b
        byte            a
        PREINIT:
	SV *sv;
	CODE:
        sv = (SV*)SvRV(ST(0));
	RETVAL = SWFShape_addSolidFill(shape, r, g, b, a);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);
	swf_stash_refcnt_inc(sv, (SV*)SvRV(ST(0)));	

SWF::Fill
SWFShape_addGradientFill(shape, gradient, flags=SWFFILL_LINEAR_GRADIENT)
	SWF::Shape      shape
	SWF::Gradient   gradient
	byte		flags
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFShape_addGradientFill(shape, gradient, flags);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);

SWF::Fill
SWFShape_addBitmapFill(shape, bitmap, flags=SWFFILL_TILED_BITMAP)
	SWF::Shape	shape
	SWF::Bitmap	bitmap
	byte	        flags
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFShape_addBitmapFill(shape, bitmap, flags);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);

void
SWFShape_xs_setLeftFill(shape, fill=NULL)
	SWF::Shape	shape
	SWF::Fill	fill
	CODE:
	SWFShape_setLeftFill(shape, fill);

void
SWFShape_xs_setRightFill(shape, fill=NULL)
	SWF::Shape	shape
	SWF::Fill	fill
	CODE:
	SWFShape_setRightFill(shape, fill);

void
SWFShape_drawGlyph(shape, font, c)
        SWF::Shape shape
        SWF::Font font
        int c
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFShape_drawGlyph(shape, font, c);

void
SWFShape_drawArc(shape, r, startAngle, endAngle)
        SWF::Shape shape
        int r
        float startAngle
        float endAngle

void
SWFShape_drawCircle(shape, r)
        SWF::Shape shape
        int r
