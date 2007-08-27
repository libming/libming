/*
    Ming, an SWF output library
    Copyright (C) 2007 Klaus Rechert

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

/*
 * This file contains compat wraper for deprecated functions. 
 * These functions will be REMOVED in future releases!
 */


#include "ming.h"

#include "blocks/shape.h"
void SWFShape_setLineStyle(SWFShape shape, unsigned short width,
                           byte r, byte g, byte b, byte a)
{
	SWFShape_setLineStyle_internal(shape, width, r, g, b, a);
}

void 
SWFShape_setLineStyle2(SWFShape shape, unsigned short width,
                       byte r, byte g, byte b, byte a,
                       int flags, float miterLimit)
{
	SWFShape_setLineStyle2_internal(shape, width, r, g, b, a, 
		flags, miterLimit);
}

void 
SWFShape_setLineStyle2filled(SWFShape shape, unsigned short width,
                       SWFFillStyle fill,
                       int flags, float miterLimit)
{
	SWFShape_setLineStyle2filled_internal(shape, width, fill,
		flags, miterLimit);
}


#include "blocks/fdbfont.h"

SWFFont loadSWFFontFromFile(FILE *file)
{
	return loadSWFFont_fromFdbFile(file);
}
