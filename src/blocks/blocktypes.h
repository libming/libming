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

/* blocktypes.h
 *
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_BLOCKTYPES_H_INCLUDED
#define SWF_BLOCKTYPES_H_INCLUDED

#include "ming.h"

typedef enum
{
  SWF_UNUSEDBLOCK               = -1, // tell ming to skip this

  SWF_CHARACTERSET		= 51,
  SWF_DEFINEBITS                =  6,
  SWF_DEFINEBITSJPEG2           = 21,
  SWF_DEFINEBITSJPEG3           = 35,
  SWF_DEFINEBITSPTR		= 1023,
  SWF_DEFINEBUTTON              =  7,
  SWF_DEFINEBUTTON2             = 34,
  SWF_DEFINEBUTTONCXFORM        = 23,
  SWF_DEFINEBUTTONSOUND         = 17,
  SWF_DEFINECOMMANDOBJ		= 50,
  SWF_DEFINEEDITTEXT		= 37,
  SWF_DEFINEFONT                = 10,
  SWF_DEFINEFONT2               = 48,
  SWF_DEFINEFONTINFO            = 13,
  SWF_DEFINELOSSLESS            = 20,
  SWF_DEFINELOSSLESS2           = 36,
  SWF_DEFINEMORPHSHAPE          = 46,
  SWF_DEFINESHAPE               =  2,
  SWF_DEFINESHAPE2              = 22,
  SWF_DEFINESHAPE3              = 32,
  SWF_DEFINESOUND               = 14,
  SWF_DEFINESPRITE              = 39,
  SWF_DEFINETEXT                = 11,
  SWF_DEFINETEXT2               = 33,
  SWF_DEFINETEXTFORMAT		= 42,
  SWF_DEFINEVIDEO		= 38,
  SWF_DOACTION                  = 12,
  SWF_ENABLEDEBUGGER		= 58,
  SWF_END                       =  0,
  SWF_EXPORTASSETS              = 56,
  SWF_FONTREF			= 52,
  SWF_FRAMELABEL                = 43,
  SWF_FRAMETAG			= 47,
  SWF_FREEALL			= 31,
  SWF_FREECHARACTER		=  3,
  SWF_GENCOMMAND		= 49,
  SWF_IMPORTASSETS              = 57,
  SWF_JPEGTABLES                =  8,
  SWF_NAMECHARACTER		= 40,
  SWF_PATHSAREPOSTSCRIPT	= 25,
  SWF_PLACEOBJECT               =  4,
  SWF_PLACEOBJECT2              = 26,
  SWF_PROTECT                   = 24,
  SWF_REMOVEOBJECT              =  5,
  SWF_REMOVEOBJECT2             = 28,
  SWF_SERIALNUMBER		= 41,
  SWF_SETBACKGROUNDCOLOR        =  9,
  SWF_SHOWFRAME                 =  1,
  SWF_SOUNDSTREAMBLOCK          = 19,
  SWF_SOUNDSTREAMHEAD           = 18,
  SWF_SOUNDSTREAMHEAD2          = 45,
  SWF_STARTSOUND                = 15,
  SWF_SYNCFRAME			= 29,
  SWF_INITACTION				= 59,
  SWF_DEFINEVIDEOSTREAM		= 60,
  SWF_VIDEOFRAME		= 61,
// these are pseudo definitions that do not correspond to actual swf file tags
  SWF_PREBUILTCLIP = 254,
  SWF_PREBUILT = 255
} SWFBlocktype;

#endif /* SWF_BLOCKTYPES_H_INCLUDED */
