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

#ifndef SWF_JPEG_H_INCLUDED
#define SWF_JPEG_H_INCLUDED

typedef struct _swfJpegBitmap *SWFJpegBitmap;

#include <stdio.h>
#include "bitmap.h"
#include "input.h"

struct _swfJpegBitmap
{
  struct _bitmap bitmap;
  SWFInput input;
  int length;
};

#define SWFJPEGBITMAP_SIZE sizeof(struct _swfJpegBitmap)

SWFJpegBitmap newSWFJpegBitmap(FILE *f);
SWFJpegBitmap newSWFJpegBitmap_fromInput(SWFInput input);

struct _swfJpegWithAlpha
{
  struct _bitmap bitmap;
  SWFInput input; /* leave these here so that we */
  int length; /* can cast this to swfJpegBitmap */
  SWFInput alpha;
  int jpegLength;
};
typedef struct _swfJpegWithAlpha *SWFJpegWithAlpha;

#define SWFJPEGWITHALPHA_SIZE sizeof(struct _swfJpegWithAlpha)

SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha);
SWFJpegWithAlpha newSWFJpegWithAlpha_fromInput(SWFInput input, SWFInput alpha);

#endif /* SWF_JPEG_H_INCLUDED */
