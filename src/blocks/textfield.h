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

#ifndef SWF_DEFINEEDITTEXT_H_INCLUDED
#define SWF_DEFINEEDITTEXT_H_INCLUDED

typedef struct SWFTextField_s *SWFTextField;

#include "output.h"
#include "block.h"
#include "character.h"
#include "matrix.h"
#include "rect.h"
#include "font.h"
#include "browserfont.h"

#define SWFTEXTFIELD_ONMASK  0x2005 /* on bits */
#define SWFTEXTFIELD_OFFMASK 0x3BFF /* off bits */

#define SWFTEXTFIELD_HASFONT   (1<<0)   /* font and size given */
#define SWFTEXTFIELD_HASLENGTH (1<<1)
#define SWFTEXTFIELD_HASCOLOR  (1<<2)
#define SWFTEXTFIELD_NOEDIT    (1<<3)
#define SWFTEXTFIELD_PASSWORD  (1<<4)
#define SWFTEXTFIELD_MULTILINE (1<<5)
#define SWFTEXTFIELD_WORDWRAP  (1<<6)
#define SWFTEXTFIELD_HASTEXT   (1<<7)   /* initial text present */
#define SWFTEXTFIELD_USEFONT   (1<<8)
#define SWFTEXTFIELD_HTML      (1<<9)
#define SWFTEXTFIELD_DRAWBOX   (1<<11)
#define SWFTEXTFIELD_NOSELECT  (1<<12)
#define SWFTEXTFIELD_HASLAYOUT (1<<13)  /* align, margin, lspace, indent */
#define SWFTEXTFIELD_AUTOSIZE  (1<<14)	/* SWF6 */

typedef enum
{
  SWFTEXTFIELD_ALIGN_LEFT    = 0,
  SWFTEXTFIELD_ALIGN_RIGHT   = 1,
  SWFTEXTFIELD_ALIGN_CENTER  = 2,
  SWFTEXTFIELD_ALIGN_JUSTIFY = 3
} SWFTextFieldAlignment;

#define SWFTEXTFIELD_SIZE sizeof(struct _textField)

SWFTextField newSWFTextField();
SWFBrowserFont newSWFBrowserFont(char *name);

void SWFTextField_setFont(SWFTextField field, SWFBlock font);
void SWFTextField_setScaledBounds(SWFTextField field, int width, int height);
void SWFTextField_setFlags(SWFTextField field, int flags);
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a);
void SWFTextField_setVariableName(SWFTextField field, char *name);
void SWFTextField_addString(SWFTextField field, char *string);

void SWFTextField_setScaledFontHeight(SWFTextField field, int height);
void SWFTextField_setScaledFieldHeight(SWFTextField field, int height);
void SWFTextField_setScaledWidth(SWFTextField field, int width);
void SWFTextField_setScaledLeftMargin(SWFTextField field, int leftMargin);
void SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin);
void SWFTextField_setScaledIndentation(SWFTextField field, int indentation);
void SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing);
void SWFTextField_setScaledPadding(SWFTextField field, int padding);
void SWFTextField_setAlignment(SWFTextField field,
			       SWFTextFieldAlignment alignment);
void SWFTextField_setLength(SWFTextField field, int length);

#endif /* SWF_DEFINEEDITTEXT_H_INCLUDED */
