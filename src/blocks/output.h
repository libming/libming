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

/* output.h */

#ifndef SWF_OUTPUT_H_INCLUDED
#define SWF_OUTPUT_H_INCLUDED

#include "libswf.h"
#include "method.h"

#define OUTPUT_BUFFER_INCREMENT 1024

struct _output
{
  struct _output *next;
  byte *buffer;
  byte *pos;
  int buffersize;
  int free;
  int bitpos;
};
typedef struct _output *SWFOutput;

#define OUTPUT_SIZE sizeof(struct _output)

/* create/destroy output object */
SWFOutput newSWFOutput();
SWFOutput newSizedSWFOutput(int size);
void destroySWFOutput(SWFOutput out);

/* write output's buffer to stream */
void SWFOutput_writeToMethod(SWFOutput out,
			     SWFByteOutputMethod method, void *data);

/* utilities for writing */
void SWFOutput_grow(SWFOutput out);
void SWFOutput_checkSize(SWFOutput out, int bytes);
void SWFOutput_byteAlign(SWFOutput out);

int SWFOutput_length(SWFOutput out);

/* write data to output */
void SWFOutput_writeBits(SWFOutput out, int data, int bits);
void SWFOutput_writeSBits(SWFOutput out, int data, int bits);
void SWFOutput_writeUInt8(SWFOutput out, int data);
void SWFOutput_writeSInt8(SWFOutput out, int data);
void SWFOutput_writeUInt16(SWFOutput out, int data);
void SWFOutput_writeSInt16(SWFOutput out, int data);
void SWFOutput_writeUInt32(SWFOutput out, long data);
void SWFOutput_writeSInt32(SWFOutput out, long data);
void SWFOutput_writeBuffer(SWFOutput out, char *buffer, int bytes);

/* number of bits required to store num */
int SWFOutput_numBits(int num);
int SWFOutput_numSBits(int num);

void SWFOutput_writeString(SWFOutput out, byte *string);

#endif /* SWF_OUTPUT_H_INCLUDED */
