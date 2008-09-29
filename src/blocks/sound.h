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

/* sound.h
 *
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_SOUND_H_INCLUDED
#define SWF_SOUND_H_INCLUDED

#include "ming.h"

#define SWF_SOUNDINFO_SYNCSTOPSOUND  (1<<5)
#define SWF_SOUNDINFO_SYNCNOMULTIPLE (1<<4)
#define SWF_SOUNDINFO_HASENVELOPE    (1<<3)
#define SWF_SOUNDINFO_HASLOOPS       (1<<2)
#define SWF_SOUNDINFO_HASOUTPOINT    (1<<1)
#define SWF_SOUNDINFO_HASINPOINT     (1<<0)

SWFBlock newDefineSWFSoundBlock(SWFSound sound);

static inline int SWFSound_getSampleRate(int flags)
{
	switch ( flags & SWF_SOUND_RATE)
	{
		case SWF_SOUND_44KHZ:
			return 44100;
		case SWF_SOUND_22KHZ:
			return 22050;
		case SWF_SOUND_11KHZ:
			return 11025; 
		default:
			return 0;
	}
}

#endif /* SWF_SOUND_H_INCLUDED */
