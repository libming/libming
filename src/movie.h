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

/* movie.h */

/* $Id$ */

#ifndef MING_MOVIE_H_INCLUDED
#define MING_MOVIE_H_INCLUDED

typedef struct SWFMovie_s *SWFMovie;

#include "libming.h"
#include "displaylist.h"


void destroySWFMovie(SWFMovie movie);

SWFMovie newSWFMovie();

SWFMovie newSWFMovieWithVersion(int version);

void SWFMovie_setRate(SWFMovie movie, float rate);

void SWFMovie_setDimension(SWFMovie movie, float x, float y);

void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);

void SWFMovie_setBackground(SWFMovie movie, byte r, byte g, byte b);

void SWFMovie_Protect(SWFMovie movie);

void SWFMovie_setSoundStream(SWFMovie movie, SWFSound sound);

void SWFMovie_addBlock(SWFMovie movie, SWFBlock block);

SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block);

void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);

void SWFMovie_nextFrame(SWFMovie movie);

void SWFMovie_labelFrame(SWFMovie movie, char *label);

void SWFMovie_addExport(SWFMovie movie, SWFBlock block, char *name);

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);

int SWFMovie_save(SWFMovie movie, char *filename);

#endif /* MING_MOVIE_H_INCLUDED */
