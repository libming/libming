/*
    Ming, an SWF output library
    Copyright (C) 2000  Opaque Industries - http://www.opaque.net/

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

#include <stdio.h>

#include "blocks/swf.h"

#define MING_VERSION        0.1.0
#define MING_VERSION_TEXT  "0.1.0"

#include "movie.h"
#include "displaylist.h"
#include "fill.h"
#include "shape_util.h"

int Ming_init();

/* stupid redef problem if we just include movieclip.h */

typedef void *SWFMovieClip;

void destroySWFMovieClip(SWFMovieClip clip);
SWFMovieClip newSWFMovieClip();

void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);
SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);
void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);
void SWFMovieClip_nextFrame(SWFMovieClip clip);
void SWFMovieClip_labelFrame(SWFMovieClip clip, char *label);

int SWFMovieClip_output(SWFMovieClip clip, SWFByteOutputMethod method, void *data);
