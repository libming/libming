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

/* $Id$ */

/* generic input object.  Wraps files, buffers and streams, replaces
   standard file funcs */

#ifndef SWF_INPUT_H_INCLUDED
#define SWF_INPUT_H_INCLUDED

typedef struct SWFInput_s *SWFInput;

#include "libswf.h"


int SWFInput_length (SWFInput input);

void SWFInput_rewind (SWFInput input);

int SWFInput_tell (SWFInput input);

void SWFInput_seek (SWFInput input, long offset, int whence);

int SWFInput_eof (SWFInput input);

int SWFInput_getChar (SWFInput input);

int SWFInput_getUInt16 (SWFInput input);

int SWFInput_getUInt16_BE (SWFInput input);

int SWFInput_getSInt16 (SWFInput input);

unsigned long SWFInput_getUInt32 (SWFInput input);

unsigned long SWFInput_getUInt32_BE (SWFInput input);

SWFInput newSWFInput_file (FILE * f);

SWFInput newSWFInput_stream (FILE * f);

SWFInput newSWFInput_buffer (unsigned char *buffer, int length);

SWFInput newSWFInput_allocedBuffer (unsigned char *buffer, int length);

void destroySWFInput (SWFInput input);

#endif /* SWF_INPUT_H_INCLUDED */
