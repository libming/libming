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

/* soundinstance.h */

#ifndef SWF_SOUNDINSTANCE_H_INCLUDED
#define SWF_SOUNDINSTANCE_H_INCLUDED

typedef struct SWFSoundInstance_s *SWFSoundInstance;

#include "sound.h"

SWFSoundInstance newSWFSoundInstance(SWFSound sound);

SWFSoundInstance newSWFSoundInstance_stop(SWFSound sound);

SWFSoundInstance newSWFSoundInstance_startNoMultiple(SWFSound sound);

void SWFSoundInstance_setNoMultiple(SWFSoundInstance instance);

void SWFSoundInstance_setLoopInPoint(SWFSoundInstance instance, unsigned int point);

void SWFSoundInstance_setLoopOutPoint(SWFSoundInstance instance, unsigned int point);

void SWFSoundInstance_setLoopCount(SWFSoundInstance instance, int count);

void SWFSoundInstance_setNoMultiple(SWFSoundInstance instance);

// button.c calls these, so they're exposed:

void writeSWFSoundInstanceToMethod(SWFBlock block,
				SWFByteOutputMethod method, void *data);

int completeSWFSoundInstance(SWFBlock block);

#endif /* SWF_SOUNDINSTANCE_H_INCLUDED */
