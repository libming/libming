/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* fillstyle.h
 *
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_FILLSTYLE_H_INCLUDED
#define SWF_FILLSTYLE_H_INCLUDED

#include "ming.h"
#include "blocktypes.h"
#include "rect.h"

int SWFFillStyle_equals(SWFFillStyle fill1, SWFFillStyle fill2);

void SWFOutput_writeFillStyles(SWFOutput out,
			       SWFFillStyle *fills, int nFills,
			       SWFBlocktype shapeType, SWFRect bounds);

void SWFOutput_writeFillStyle(SWFOutput out, SWFFillStyle file, SWFBlocktype type, SWFRect bounds);

void SWFOutput_writeMorphFillStyles(SWFOutput out,
				    SWFFillStyle *fills1, int nFills1, SWFRect bounds1,
				    SWFFillStyle *fills2, int nFills2, SWFRect bounds2);

void
SWFOutput_writeMorphFillStyle(SWFOutput out, SWFFillStyle fill1, SWFRect bounds1,
                              SWFFillStyle fill2, SWFRect bounds2);

void SWFFillStyle_addDependency(SWFFillStyle fill, SWFCharacter c);
#endif /* SWF_FILLSTYLE_H_INCLUDED */
