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

/* textfield.h
 * 
 * $Id$
 *
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_TEXTFIELD_H_INCLUDED
#define SWF_TEXTFIELD_H_INCLUDED

#include "ming.h"

#define SWFTEXTFIELD_SIZE sizeof(struct _textField)

void SWFTextField_setScaledBounds(SWFTextField field, int width, int height);
void SWFTextField_setScaledFontHeight(SWFTextField field, int height);
void SWFTextField_setScaledFieldHeight(SWFTextField field, int height);
void SWFTextField_setScaledWidth(SWFTextField field, int width);
void SWFTextField_setScaledLeftMargin(SWFTextField field, int leftMargin);
void SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin);
void SWFTextField_setScaledIndentation(SWFTextField field, int indentation);
void SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing);
void SWFTextField_setScaledPadding(SWFTextField field, int padding);

void SWFTextField_setFontCharacter(SWFTextField field, SWFFontCharacter fontchar);
SWFFont SWFTextField_getUnresolvedFont(SWFTextField field);

#endif /* SWF_TEXTFIELD_H_INCLUDED */
