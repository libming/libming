/* ====================================================================
 * Copyright (c) 2007 by Klaus Rechert. All rights reserved.
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


MODULE = SWF::ButtonRecord	PACKAGE = SWF::ButtonRecord	PREFIX = SWFButtonRecord_

PROTOTYPES: ENABLE

void SWFButtonRecord_setDepth(record, depth)
	SWF::ButtonRecord record
	int depth


void SWFButtonRecord_setBlendMode(record, mode)
	SWF::ButtonRecord record
	int mode


void SWFButtonRecord_move(record, x, y)
	SWF::ButtonRecord record
	float x
	float y


void SWFButtonRecord_moveTo(record, x, y)
	SWF::ButtonRecord record
	float x
	float y


void SWFButtonRecord_rotate(record, deg)
	SWF::ButtonRecord record
	float deg


void SWFButtonRecord_rotateTo(record, deg)
	SWF::ButtonRecord record
	float deg


void SWFButtonRecord_scale(record, scaleX, scaleY)
	SWF::ButtonRecord record
	float scaleX
	float scaleY


void SWFButtonRecord_scaleTo(record, scaleX, scaleY)
	SWF::ButtonRecord record
	float scaleX
	float scaleY


void SWFButtonRecord_skewX(record, skewX)
	SWF::ButtonRecord record
	float skewX


void SWFButtonRecord_skewXTo(record, skewX)
	SWF::ButtonRecord record
	float skewX


void SWFButtonRecord_skewY(record, skewY)
	SWF::ButtonRecord record
	float skewY


void SWFButtonRecord_skewYTo(record, skewY)
	SWF::ButtonRecord record
	float skewY

void
SWFButtonRecord_addFilter(record, filter)
	SWF::ButtonRecord record
	SWF::Filter filter

void SWFButtonRecord_DESTROY(br)
	SWF::ButtonRecord br
	CODE:
	S_DEBUG(2, fprintf(stderr, "ButtonRecord DESTROY CALLED\n"));

