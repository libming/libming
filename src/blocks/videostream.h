/*
    
   Ming, an SWF output library
   Copyright (C) 2002  Opaque Industries - http://www.opaque.net/
   
   15.12.2003 Klaus Rechert
   
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


#ifndef SWF_VIDEOSTREAM_H_INCLUDED
#define SWF_VIDEOSTREAM_H_INCLUDED

#include "ming.h"

typedef struct SWFVideoFrame_s *SWFVideoFrame;

#define VIDEO_DEF_WIDTH 160
#define VIDEO_DEF_HEIGHT 120

int SWFVideoStream_getFrameNumber(SWFVideoFrame frame);
int SWFVideoStream_getNumFrames(SWFVideoStream stream);

SWFVideoStream newSWFVideoStream_fromFile(FILE *f);
SWFVideoStream newSWFVideoStream(void);

SWFBlock SWFVideoStream_getVideoFrame(SWFVideoStream stream); 
void SWFVideoStream_setDimension(SWFVideoStream stream, int width, int height);
#endif
