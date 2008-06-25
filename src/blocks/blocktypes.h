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
#include "error.h"

typedef enum
{
  SWF_UNUSEDBLOCK               = -1, /* tell ming to skip this */

  SWF_DEFINEBITS                =  6,
  SWF_DEFINEBITSJPEG2           = 21,
  SWF_DEFINEBITSJPEG3           = 35,
  SWF_DEFINEBUTTON              =  7,
  SWF_DEFINEBUTTON2             = 34,
  SWF_DEFINEBUTTONCXFORM        = 23,
  SWF_DEFINEBUTTONSOUND         = 17,
  SWF_DEFINEEDITTEXT		= 37,
  SWF_DEFINEFONT                = 10,
  SWF_DEFINEFONT2               = 48,
  SWF_DEFINEFONT3		= 75,  /* SWF_version  >= 8 */
  SWF_DEFINEFONTALIGNZONES	= 73,  /* SWF_version  >= 8 */
  SWF_CSMTEXTSETTINGS		= 74,  /* SWF_version  >= 8 */
  SWF_DEFINEFONTINFO            = 13,
  SWF_DEFINEFONTINFO2		= 62,  /* SWF_version  >= 6 */
  SWF_DEFINELOSSLESS            = 20,
  SWF_DEFINELOSSLESS2           = 36,
  SWF_DEFINEMORPHSHAPE          = 46,
  SWF_DEFINEMORPHSHAPE2		= 84,  /* SWF_version  >= 8 */
  SWF_DEFINESHAPE               =  2,
  SWF_DEFINESHAPE2              = 22,
  SWF_DEFINESHAPE3              = 32,
  SWF_DEFINESHAPE4		= 83,  /* SWF_version  >= 8 */
  SWF_DEFINESOUND               = 14,
  SWF_DEFINESPRITE              = 39,
  SWF_DEFINETEXT                = 11,
  SWF_DEFINETEXT2               = 33,
  SWF_DEFINESCALINGGRID		= 78,  /* SWF_version  >= 8 */
  SWF_DOACTION                  = 12,
  SWF_ENABLEDEBUGGER		= 58,  /* SWF_version  <  6 */ 
  SWF_ENABLEDEBUGGER2		= 64,  /* SWF_version  >= 6 */
  SWF_SCRIPTLIMITS		= 65,  /* SWF_version  >= 7 */
  SWF_SETTABINDEX		= 66,  /* SWF_version  >= 7 */
  SWF_END                       =  0,
  SWF_EXPORTASSETS              = 56,
  SWF_FRAMELABEL                = 43,
  SWF_IMPORTASSETS              = 57,  /* SWF_version  < 8 only */
  SWF_IMPORTASSETS2		= 71,  /* SWF_version  >= 8 */
  SWF_JPEGTABLES                =  8,
  SWF_PLACEOBJECT               =  4,
  SWF_PLACEOBJECT2              = 26,
  SWF_PLACEOBJECT3		= 70,  /* SWF_version >= 8 */
  SWF_PROTECT                   = 24,
  SWF_REMOVEOBJECT              =  5,
  SWF_REMOVEOBJECT2             = 28,
  SWF_SETBACKGROUNDCOLOR        =  9,
  SWF_SHOWFRAME                 =  1,
  SWF_SOUNDSTREAMBLOCK          = 19,
  SWF_SOUNDSTREAMHEAD           = 18,
  SWF_SOUNDSTREAMHEAD2          = 45,
  SWF_STARTSOUND                = 15,
  SWF_INITACTION		= 59,
  SWF_DEFINEVIDEOSTREAM		= 60,
  SWF_VIDEOFRAME		= 61,
  SWF_SERIALNUMBER		= 41,
  SWF_DEBUGID			= 63,

  /* SWF8 tags */
  SWF_FILEATTRIBUTES            = 69,
  SWF_METADATA                  = 77,

  /* SWF9 tags */
  SWF_SYMBOLCLASS               = 76,
  SWF_DOABC                     = 82,
  SWF_DEFINEBINARYDATA		= 87,
  SWF_DEFINESCENEANDFRAMEDATA   = 86,
  SWF_STARTSOUND2		= 89,
  SWF_DEFINEFONTNAME		= 88,

  /* Unconfirmed tags */
  SWF_REFLEX			= 777,

  /*obsolete and/or undocumented tags */
  SWF_DEFINECOMMANDOBJ		= 50,  
  SWF_DEFINETEXTFORMAT		= 42,  
  SWF_DEFINEVIDEO		= 38, 
  SWF_FRAMETAG			= 47,  
  SWF_FREEALL			= 31,  
  SWF_FREECHARACTER		=  3,  
  SWF_GENCOMMAND		= 49,  
  SWF_FONTREF			= 52,  
  SWF_NAMECHARACTER		= 40,  
  SWF_PATHSAREPOSTSCRIPT	= 25,
  SWF_SYNCFRAME			= 29,
  SWF_CHARACTERSET		= 51,
  SWF_DEFINEBITSPTR		= 1023,

/* these are pseudo definitions that do not correspond to actual swf file tags */
  SWF_MINGFONT = 252,
  SWF_BROWSERFONT = 253,
  SWF_PREBUILTCLIP = 254,
  SWF_PREBUILT = 255
} SWFBlocktype;


static inline int SWFBlocktype_swfVersion(int type)
{
	switch(type)
	{	
		case SWF_END:
		case SWF_SHOWFRAME:
		case SWF_DEFINESHAPE:
		case SWF_PLACEOBJECT:
		case SWF_REMOVEOBJECT:
		case SWF_DEFINEBITS:
		case SWF_DEFINEBUTTON:
		case SWF_JPEGTABLES:	
		case SWF_SETBACKGROUNDCOLOR:
		case SWF_DEFINEFONT:
		case SWF_DEFINETEXT:
		case SWF_DOACTION:
		case SWF_DEFINEFONTINFO:
		case SWF_PREBUILTCLIP:
		case SWF_PREBUILT:
			return 1;
		
		case SWF_DEFINESOUND:
		case SWF_STARTSOUND:
		case SWF_DEFINEBUTTONSOUND:
		case SWF_SOUNDSTREAMHEAD:
		case SWF_SOUNDSTREAMBLOCK:
		case SWF_DEFINELOSSLESS:
		case SWF_DEFINEBITSJPEG2:
		case SWF_DEFINESHAPE2:
		case SWF_DEFINEBUTTONCXFORM:
		case SWF_PROTECT:
			return 2;

		case SWF_PLACEOBJECT2:
		case SWF_REMOVEOBJECT2:
		case SWF_DEFINESHAPE3:
		case SWF_DEFINETEXT2:
		case SWF_DEFINEBUTTON2:
		case SWF_DEFINEBITSJPEG3:
		case SWF_DEFINELOSSLESS2:
		case SWF_DEFINESPRITE:
		case SWF_FRAMELABEL:
		case SWF_SOUNDSTREAMHEAD2:
		case SWF_DEFINEMORPHSHAPE:
		case SWF_DEFINEFONT2:
		case SWF_GENCOMMAND:
			return 3;
		
		case SWF_DEFINEEDITTEXT:
			return 4;		

		case SWF_EXPORTASSETS:
		case SWF_IMPORTASSETS:
		case SWF_ENABLEDEBUGGER:
			return 5;

		case SWF_INITACTION:
		case SWF_DEFINEVIDEOSTREAM:
		case SWF_VIDEOFRAME:
		case SWF_DEFINEFONTINFO2:
		case SWF_ENABLEDEBUGGER2:
			return 6;

		case SWF_SCRIPTLIMITS:
		case SWF_SETTABINDEX:
			return 7;

		case SWF_FILEATTRIBUTES:
		case SWF_PLACEOBJECT3:
		case SWF_IMPORTASSETS2:
		case SWF_DEFINEFONTALIGNZONES:
		case SWF_CSMTEXTSETTINGS:
		case SWF_DEFINEFONT3:
		case SWF_METADATA:
		case SWF_DEFINESHAPE4:
		case SWF_DEFINESCALINGGRID:
		case SWF_DEFINEMORPHSHAPE2:
			return 8;
		default:
			SWF_warn("unknown tag type %i\n", type);
			return -1;
	}
}

#endif /* SWF_BLOCKTYPES_H_INCLUDED */
