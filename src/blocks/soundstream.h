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

/* soundstream.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_SOUNDSTREAM_H_INCLUDED
#define SWF_SOUNDSTREAM_H_INCLUDED

#include "ming.h"

typedef struct SWFSoundStreamBlock_s *SWFSoundStreamBlock;

#define SWF_SOUNDSTREAM_COMPRESSION      0xf0
#define SWF_SOUNDSTREAM_NOT_COMPRESSED   (0<<4)
#define SWF_SOUNDSTREAM_ADPCM_COMPRESSED (1<<4)
#define SWF_SOUNDSTREAM_MP3_COMPRESSED   (2<<4)

#define SWF_SOUNDSTREAM_RATE             0x0c
#define SWF_SOUNDSTREAM_5KHZ             (0<<2)
#define SWF_SOUNDSTREAM_11KHZ            (1<<2)
#define SWF_SOUNDSTREAM_22KHZ            (2<<2)
#define SWF_SOUNDSTREAM_44KHZ            (3<<2)

#define SWF_SOUNDSTREAM_BITS             0x02
#define SWF_SOUNDSTREAM_8BITS            (0<<1)
#define SWF_SOUNDSTREAM_16BITS           (1<<1)

#define SWF_SOUNDSTREAM_CHANNELS         0x01
#define SWF_SOUNDSTREAM_MONO             (0<<0)
#define SWF_SOUNDSTREAM_STEREO           (1<<0)

SWFBlock SWFSoundStream_getStreamHead(SWFSoundStream stream, float frameRate, float skip);

SWFBlock SWFSoundStream_getStreamBlock(SWFSoundStream sound);

void SWFSoundStream_rewind(SWFSoundStream sound);

#endif /* SWF_SOUNDSTREAM_H_INCLUDED */
