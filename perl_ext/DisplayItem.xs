/* ====================================================================
 * Copyright (c) 2000-2002 by Soheil Seyfaie. All rights reserved.
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


MODULE = SWF::DisplayItem	PACKAGE = SWF::DisplayItem	PREFIX = SWFDisplayItem_

PROTOTYPES: ENABLE

void 
SWFDisplayItem_setMaskLevel(item, masklevel)
	SWF::DisplayItem item
	int masklevel

void
SWFDisplayItem_endMask(item)
	SWF::DisplayItem item

void 
SWFDisplayItem_addAction(item, action, flags)
	SWF::DisplayItem item
	SWF::Action action
	int flags

void
SWFDisplayItem_moveTo(item, x, y)
	SWF::DisplayItem item
	double x 
	double y

void
SWFDisplayItem_move(item, x, y)
	SWF::DisplayItem item
	double x 
	double y


void 
SWFDisplayItem_scaleTo(item, x, ...)
        SWF::DisplayItem item
        double            x
        double            y   = NO_INIT
        CODE:
        y = (items > 2) ? (float)SvNV(ST(2)) : x; 
        SWFDisplayItem_scaleTo(item, x, y);

void 
SWFDisplayItem_scale(item, x, ...)
        SWF::DisplayItem item
        double            x
        double            y   = NO_INIT
        CODE:
        y = (items > 2) ? (float)SvNV(ST(2)) : x; 
        SWFDisplayItem_scale(item, x, y);


void 
SWFDisplayItem_rotate(item, degrees)
	SWF::DisplayItem item
	double degrees

void 
SWFDisplayItem_rotateTo(item, degrees)
	SWF::DisplayItem item
	double degrees

void 
SWFDisplayItem_skewX(item, x)
        SWF::DisplayItem item 
        double x


void
SWFDisplayItem_skewXTo(item, x)
       SWF::DisplayItem item
       double x

void 
SWFDisplayItem_skewY(item, y)
        SWF::DisplayItem item
        double y

void 
SWFDisplayItem_skewYTo(item, y)
        SWF::DisplayItem item
        double y

void
SWFDisplayItem_setDepth(item, depth)
        SWF::DisplayItem item
        int depth

void
SWFDisplayItem_setRatio(item, ratio)
        SWF::DisplayItem item
        float ratio

void 
SWFDisplayItem_setBlendMode(item, mode)
	SWF::DisplayItem item
	int mode

void
SWFDisplayItem_cacheAsBitmap(item, flag)
	SWF::DisplayItem item
	int flag

void
SWFDisplayItem_setColorAdd(item, r, g, b, a=0)
        SWF::DisplayItem item
        int r
        int g
        int b
        int a
        ALIAS:
        SWF::DisplayItem::addColor = 1

void
SWFDisplayItem_setColorMult(item, r, g, b, a=1.0)
        SWF::DisplayItem item
        float r
        float g
        float b
        float a
        ALIAS:
        SWF::DisplayItem::multColor = 1

void 
SWFDisplayItem_setName(item, name)
       SWF::DisplayItem item
       char *name


void 
SWFDisplayItem_remove(item)
	SWF::DisplayItem item


void 
SWFDisplayItem_getPosition(item)
    	SWF::DisplayItem item
	PREINIT:
        double           x;
        double           y;
	PPCODE:
	SWFDisplayItem_getPosition(item, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(x)));
	PUSHs(sv_2mortal(newSVnv(y)));

void 
SWFDisplayItem_getScale(item)
	SWF::DisplayItem item
	PREINIT:
	double             xScale;
	double             yScale;
	PPCODE:
	SWFDisplayItem_getScale(item, &xScale, &yScale);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(xScale)));
	PUSHs(sv_2mortal(newSVnv(yScale)));

void 
SWFDisplayItem_getSkew(item)
	SWF::DisplayItem item
	PREINIT:
	double             xSkew;
	double             ySkew;
	PPCODE:
	SWFDisplayItem_getSkew(item, &xSkew, &ySkew);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(xSkew)));
	PUSHs(sv_2mortal(newSVnv(ySkew)));


void 
SWFDisplayItem_getRotation(item)
	SWF::DisplayItem item
	PREINIT:
	double    degrees;
	CODE:
	SWFDisplayItem_getRotation(item, &degrees);
	ST(0) = sv_newmortal();
	sv_setnv(ST(0), degrees);


int
SWFDisplayItem_getDepth(item)
	SWF::DisplayItem item

void 
SWFDisplayItem_flush(item)
	SWF::DisplayItem item

void 
SWFDisplayItem_setMatrix(item, a, b, c, d, x, y)
	SWF::DisplayItem item
	double a
	double b
	double c
	double d
	double x
	double y

SWF::Matrix
SWFDisplayItem_getMatrix(item)
	SWF::DisplayItem item
	CODE:
	RETVAL = SWFDisplayItem_getMatrix(item);
	ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), "SWF::Matrix", (void*)RETVAL);

SWF::Character
SWFDisplayItem_getCharacter(item)
	SWF::DisplayItem item 
	CODE:
	RETVAL = SWFDisplayItem_getCharacter(item);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Character", (void*)RETVAL);

void
SWFDisplayItem_addFilter(item, filter)
	SWF::DisplayItem item
	SWF::Filter filter

void 
SWFDisplayItem_setCXform(item, cx)
	SWF::DisplayItem item
	SWF::CXform cx

void
SWFDisplayItem_DESTROY(item)
	SWF::DisplayItem	item
	CODE:
        S_DEBUG(2, fprintf(stderr, "DisplayItem DESTROY CALLED\n"));
	
