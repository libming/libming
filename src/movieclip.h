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

/* movieclip.h */

#ifndef MING_MOVIECLIP_H_INCLUDED
#define MING_MOVIECLIP_H_INCLUDED

typedef struct SWFMovieClip_s *SWFMovieClip;

#include "displaylist.h"
#include "blocks/soundinstance.h"

SWFMovieClip
newSWFMovieClip();

void
destroySWFMovieClip(SWFMovieClip clip);

void
SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);

SWFDisplayItem
SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);

void
SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);

void
SWFMovieClip_nextFrame(SWFMovieClip clip);

void
SWFMovieClip_labelFrame(SWFMovieClip clip, const char *label);

void
SWFMovie_setSoundStream(SWFMovieClip movie, SWFSound sound, float rate);

SWFSoundInstance
SWFMovieClip_startSound(SWFMovieClip clip, SWFSound sound);

void
SWFMovieClip_stopSound(SWFMovieClip clip, SWFSound sound);

int
SWFMovieClip_output(SWFMovieClip clip, SWFByteOutputMethod method, void *data);

BOOL
SWFMovieClip_getDependencies(SWFMovieClip clip,
			     SWFCharacter** deps, int* nDeps);

#endif /* MING_MOVIECLIP_H_INCLUDED */
