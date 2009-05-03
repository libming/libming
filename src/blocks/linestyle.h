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

/* linestyle.h
 * 
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_LINESTYLE_H_INCLUDED
#define SWF_LINESTYLE_H_INCLUDED

#include "ming.h"
#include "output.h"
#include "blocktypes.h"
#include "rect.h"


/* okay, this is kinda lame.. */
byte SWFLineStyle_equals(SWFLineStyle line, unsigned short width,
			 byte r, byte g, byte b, byte a, int flags);

byte SWFLineStyle_equals2filled(SWFLineStyle line, unsigned short width,
                                SWFFillStyle fill, int flags);


unsigned short SWFLineStyle_getWidth(SWFLineStyle line);

void SWFOutput_writeLineStyles(SWFOutput out,
			       SWFLineStyle *lines, int nLines,
			       SWFBlocktype shapeType,
			       SWFRect bounds);

void SWFOutput_writeMorphLineStyles(SWFOutput out,
				    SWFLineStyle *lines1, int nLines1,
				    SWFLineStyle *lines2, int nLines2);

void SWFOutput_writeMorphLineStyles2(SWFOutput out,
                SWFLineStyle *lines1, int nLines1,
                SWFLineStyle *lines2, int nLines2);

#endif /* SWF_LINESTYLE_H_INCLUDED */
