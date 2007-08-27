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

/* output.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_OUTPUT_H_INCLUDED
#define SWF_OUTPUT_H_INCLUDED

#include "ming.h"

typedef struct SWFOutput_s *SWFOutput;

#define OUTPUT_BUFFER_INCREMENT 1024


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

/* truncate a SizedSWFOutput after the final length is known */
void SWFOutput_truncate(SWFOutput out, int size);

void SWFOutput_byteAlign(SWFOutput out);

int SWFOutput_getLength(SWFOutput out);

void SWFOutput_setNext(SWFOutput out, SWFOutput next);

SWFOutput SWFOutput_getNext(SWFOutput out);

byte* SWFOutput_getBuffer(SWFOutput out);

byte* SWFOutput_getCurPos(SWFOutput out);

/* write data to output */
void SWFOutput_writeBits(SWFOutput out, int data, int bits);

void SWFOutput_writeSBits(SWFOutput out, int data, int bits);

void SWFOutput_writeUInt8(SWFOutput out, int data);

void SWFOutput_writeSInt8(SWFOutput out, int data);

void SWFOutput_writeUInt16(SWFOutput out, int data);

void SWFOutput_writeSInt16(SWFOutput out, int data);

void SWFOutput_writeUInt32(SWFOutput out, long data);

void SWFOutput_writeSInt32(SWFOutput out, long data);

void SWFOutput_writeBuffer(SWFOutput out, unsigned char *buffer, int bytes);

void SWFOutput_writeFixed(SWFOutput out, double val);

void SWFOutput_writeFixed8(SWFOutput out, double val);

void SWFOutput_writeFloat(SWFOutput out, float val);

void SWFOutput_writeDouble(SWFOutput out, double val);

void SWFOutput_writeFloat16(SWFOutput out, float val);

/* number of bits required to store num */
int SWFOutput_numBits(int num);

int SWFOutput_numSBits(int num);

void SWFOutput_writeString(SWFOutput out, const byte *string);

void SWFOutput_writeEncUInt32(SWFOutput out, unsigned int i);

#endif /* SWF_OUTPUT_H_INCLUDED */
