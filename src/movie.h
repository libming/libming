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

#ifndef MING_MOVIE_H_INCLUDED
#define MING_MOVIE_H_INCLUDED

#include "blocks/swf.h"
#include "libming.h"
#include "displaylist.h"

struct _swfMovie
{
  SWFBlockList blockList;
  SWFDisplayList displayList;
  float rate;
  SWFRect bounds;
  unsigned short nFrames;
  unsigned short totalFrames;
  byte version;
};
typedef struct _swfMovie *SWFMovie;

#define SWFMOVIE_SIZE sizeof(struct _swfMovie)

void destroySWFMovie(SWFMovie movie);
SWFMovie newSWFMovie();
SWFMovie newSWFMovieWithVersion(int version);

void SWFMovie_setRate(SWFMovie movie, float rate);
void SWFMovie_setDimension(SWFMovie movie, float x, float y);
void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);

/* XXX - 0.1 name: */
#define SWFMovie_setFrames SWFMovie_setNumberOfFrames

void SWFMovie_setBackground(SWFMovie movie, int r, int g, int b);

void SWFMovie_setSoundStream(SWFMovie movie, SWFSound sound);

void SWFMovie_addBlock(SWFMovie movie, SWFBlock block);
SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block);
void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);
void SWFMovie_nextFrame(SWFMovie movie);
void SWFMovie_labelFrame(SWFMovie movie, char *label);

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);

static inline int SWFMovie_save(SWFMovie movie, char *filename)
{
  FILE *f = fopen(filename, "wb");
  int count;

  if(f == NULL)
    return -1;

  count = SWFMovie_output(movie, fileOutputMethod, f);
  fclose(f);
  return count;
}

#endif /* MING_MOVIE_H_INCLUDED */
