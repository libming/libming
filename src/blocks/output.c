/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

#ifndef __C2MAN__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "output.h"
#include "error.h"

#include "libming.h"

struct SWFOutput_s
{
	SWFOutput next;

	byte *buffer;
	byte *pos;
	int buffersize;
	int free;
	int bitpos;
};


SWFOutput
newSWFOutput()
{
	SWFOutput out = (SWFOutput) malloc(sizeof(struct SWFOutput_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == out)
		return NULL;

	out->next = NULL;
	out->buffer = (byte*) malloc(OUTPUT_BUFFER_INCREMENT);

	/* If malloc failed, return NULL to signify this */
	if (NULL == out->buffer)
	{
		free(out);
		return NULL;
	}

	out->pos = out->buffer;
	*(out->pos) = 0;
	out->buffersize = out->free = OUTPUT_BUFFER_INCREMENT;
	out->bitpos = 0;

	return out;
}


/* same as above but with specified buffer size,
	 use if you have an upper limit to how big this'll get */

SWFOutput
newSizedSWFOutput(int size)
{
	SWFOutput out = (SWFOutput)malloc(sizeof(struct SWFOutput_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == out)
		return NULL;

	out->next = NULL;
	out->buffer = (byte*) malloc(size+1);

	/* If malloc failed, return NULL to signify this */
	if (NULL == out->buffer)
	{
		free(out);
		return NULL;
	}

	out->pos = out->buffer;
	*(out->pos) = 0;
	out->buffersize = out->free = size+1;
	out->bitpos = 0;

	return out;
}


void
SWFOutput_writeToMethod(SWFOutput out, SWFByteOutputMethod method, void *data)
{
	int i, l;
	SWFOutput o = out;
	byte *buffer;

	SWFOutput_byteAlign(out);

	while ( o != NULL )
	{
		buffer = o->buffer;
		l = o->pos - buffer;

		for(i=0; i<l; ++i)
			method(buffer[i], data);

		o = o->next;
	}
}


void
destroySWFOutput(SWFOutput out)
{
	SWFOutput o = out, next;

	while(o != NULL)
	{
		next = o->next;
		free(o->buffer);
		free(o);
		o = next;
	}
}


void
SWFOutput_grow(SWFOutput out)
{
	int num = out->pos - out->buffer; /* in case buffer gets displaced.. */

	unsigned char* newbuf =
		(unsigned char*)realloc(out->buffer, out->buffersize + OUTPUT_BUFFER_INCREMENT);

	if ( newbuf != out->buffer )
		out->pos = newbuf+num;

	out->buffer = newbuf;
	out->buffersize += OUTPUT_BUFFER_INCREMENT;
	out->free += OUTPUT_BUFFER_INCREMENT;
}


int
SWFOutput_getLength(SWFOutput out)
{
	int size = 0;

	while ( out!=NULL )
	{
		size += (out->pos) - (out->buffer) + (out->bitpos>0 ? 1 : 0);
		out = out->next;
	}

	return size;
}


/* make sure there's enough space for bytes bytes */
void
SWFOutput_checkSize(SWFOutput out, int bytes)
{
	if ( bytes >= out->free )
	{
		int New = OUTPUT_BUFFER_INCREMENT *
							((bytes-out->free-1)/OUTPUT_BUFFER_INCREMENT + 1);

		int num = out->pos - out->buffer; /* in case buffer gets displaced.. */

		unsigned char *newbuf = (unsigned char*)realloc(out->buffer, out->buffersize+New);

		if ( newbuf != out->buffer )
			out->pos = newbuf + num;

		out->buffer = newbuf;
		out->buffersize += New;
		out->free += New;
	}
}


void
SWFOutput_byteAlign(SWFOutput out)
{
	if ( out->bitpos > 0 )
	{
		SWFOutput_checkSize(out, 1);
		++out->pos;
		--out->free;
		out->bitpos = 0;
	}
}


void
SWFOutput_setNext(SWFOutput out, SWFOutput next)
{
	out->next = next;
}


SWFOutput
SWFOutput_getNext(SWFOutput out)
{
	return out->next;
}


byte*
SWFOutput_getBuffer(SWFOutput out)
{
	return out->buffer;
}


byte*
SWFOutput_getCurPos(SWFOutput out)
{
	return out->pos;
}


void
SWFOutput_writeBits(SWFOutput out, int data, int bits)
{
	int bitpos = out->bitpos;

	if ( bitpos == 0 )
		*(out->pos) = 0;

	SWFOutput_checkSize(out, (bits+bitpos+7)/8);

	while ( bits > 0 )
	{
		if ( bits + bitpos >= 8 )
		{
			*(out->pos) += (data >> (bits+bitpos-8)) & 0xff;
			bits -= 8-bitpos;
			++out->pos;
			*(out->pos) = 0;
			--out->free;
			bitpos = 0;
		}
		else
		{
			*(out->pos) += (data << (8-bits-bitpos)) & 0xff;
			bitpos += bits;
			bits = 0;
		}
	}

	out->bitpos = bitpos;
}


void
SWFOutput_writeSBits(SWFOutput out, int data, int bits)
{
	if ( data < 0 )
		data = (1<<bits)+data;

	SWFOutput_writeBits(out, data, bits);
}


void
SWFOutput_writeUInt8(SWFOutput out, int data)
{
	/* think we have to byte align when we write whole bytes */
	SWFOutput_byteAlign(out);

	SWFOutput_checkSize(out, 1);
	*(out->pos) = data;
	++out->pos;
	--out->free;
}

void
SWFOutput_truncate(SWFOutput out, int size)
{	SWF_assert(size < out->buffersize);
	out->pos = out->buffer + size;
	out->free = out->buffersize - size;
}

void
SWFOutput_writeSInt8(SWFOutput out, int data)
{
	if ( data < 0 )
		data = (1<<8)+data;

	SWFOutput_writeUInt8(out, data);
}


void
SWFOutput_writeUInt16(SWFOutput out, int data)
{
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
}


void
SWFOutput_writeSInt16(SWFOutput out, int data)
{
	if ( data < 0 )
		data = (1<<16)+data;

	SWFOutput_writeUInt8(out, data%256);
	data >>= 8;
	SWFOutput_writeUInt8(out, data%256);
}


void
SWFOutput_writeUInt32(SWFOutput out, long data)
{
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
}


void
SWFOutput_writeSInt32(SWFOutput out, long data)
{
	if ( data < 0 )
		data = (0xffffffff + data) + 1;

	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
	data >>= 8;
	SWFOutput_writeUInt8(out, data&0xff);
}


void
SWFOutput_writeBuffer(SWFOutput out, unsigned char *buffer, int bytes)
{
	SWFOutput_checkSize(out, bytes);
	memcpy(out->pos, buffer, bytes);
	out->pos += bytes;
	out->free -= bytes;
}


int
SWFOutput_numBits(int num)
{
	int i=0;

	while ( num > 0 )
	{
		num >>= 1;
		++i;
	}

	return i;
}


int
SWFOutput_numSBits(int num)
{
	if ( num < 0 )
		// XXX - one more than necessary if num = -2^n:
		return SWFOutput_numBits(-num)+1;
	else
		return SWFOutput_numBits(num)+1;
}


void
SWFOutput_writeString(SWFOutput out, const unsigned char *string)
{
	char c;

	SWFOutput_byteAlign(out);

	if ( string )
	{
		while ( (c = *(string++)) != 0 )
			SWFOutput_writeUInt8(out, c);
	}

	SWFOutput_writeUInt8(out, 0);
}

void 
SWFOutput_writeFixed(SWFOutput out, double val)
{
	unsigned int fixed;

	fixed = val * (1<<16);
	SWFOutput_writeUInt32(out, fixed);
}

void 
SWFOutput_writeFixed8(SWFOutput out, double val)
{
	unsigned int fixed;

	fixed = 0xffff & (int)(val * (1<<8));
	SWFOutput_writeUInt16(out, fixed);
}



void
SWFOutput_writeFloat(SWFOutput out, float f)
{
	unsigned char *p = (unsigned char *)&f;
#if SWF_LITTLE_ENDIAN
	SWFOutput_writeUInt8(out, p[0]);
	SWFOutput_writeUInt8(out, p[1]);
	SWFOutput_writeUInt8(out, p[2]);
	SWFOutput_writeUInt8(out, p[3]);
#else
	SWFOutput_writeUInt8(out, p[3]);
	SWFOutput_writeUInt8(out, p[2]);
	SWFOutput_writeUInt8(out, p[1]);
	SWFOutput_writeUInt8(out, p[0]);
#endif	
}

void
SWFOutput_writeDouble(SWFOutput out, double d)
{
	unsigned char *p = (unsigned char *)&d;

#if SWF_LITTLE_ENDIAN
	SWFOutput_writeUInt8(out, p[4]);
	SWFOutput_writeUInt8(out, p[5]);
	SWFOutput_writeUInt8(out, p[6]);
	SWFOutput_writeUInt8(out, p[7]);
	SWFOutput_writeUInt8(out, p[0]);
	SWFOutput_writeUInt8(out, p[1]);
	SWFOutput_writeUInt8(out, p[2]);
	SWFOutput_writeUInt8(out, p[3]);
#else
	SWFOutput_writeUInt8(out, p[3]);
	SWFOutput_writeUInt8(out, p[2]);
	SWFOutput_writeUInt8(out, p[1]);
	SWFOutput_writeUInt8(out, p[0]);
	SWFOutput_writeUInt8(out, p[7]);
	SWFOutput_writeUInt8(out, p[6]);
	SWFOutput_writeUInt8(out, p[5]);
	SWFOutput_writeUInt8(out, p[4]);
#endif
}

#define FLOAT_SIGN_MASK 	0x80000000
#define FLOAT_SIGN_SHIFT 	31
#define FLOAT_EXP_MASK		0x7f800000
#define FLOAT_EXP_SHIFT 	23
#define FLOAT_MAT_MASK		0x7fff
#define FLOAT_BIAS		127;

/*
 * FLOAT16 - half precision
 * 1 - bit sign
 * 5 - bit exp / bias = 16
 * 10 - bit mantissa
 */

#define FLOAT16_SIGN_SHIFT 	15
#define FLOAT16_EXP_SHIFT	11
#define FLOAT16_BIAS		16

void
SWFOutput_writeFloat16(SWFOutput out, float f)
{
	unsigned char buf[4];
	float *pf = (float *)buf;
	int sig, exp, mat;
	unsigned int i;

	*pf = f;
	i = (unsigned int)*buf;
	
	sig = (FLOAT_SIGN_MASK & i) >> FLOAT_SIGN_SHIFT;

	exp = (FLOAT_EXP_MASK & i) >> FLOAT_EXP_SHIFT;
	exp -= FLOAT_BIAS;

	mat = (FLOAT_MAT_MASK &i);

	exp += FLOAT16_BIAS;
	exp &= 0x1f;
	
	// reduce mantissa to 11-bit
	mat >>= 12;

	i = sig << FLOAT16_SIGN_SHIFT | exp << FLOAT16_EXP_SHIFT | (mat & 0x7ff);
	SWFOutput_writeUInt16(out, i);
}

#define ENC_HIGH_BIT 0x80
#define ENC_BYTE_MASK 0x7f

void 
SWFOutput_writeEncUInt32(SWFOutput out, unsigned int i)
{
	if(i == 0) // special case 
	{
		SWFOutput_writeUInt8(out, 0);
		return;
	}
	while(i > 0)
	{
		unsigned char temp = i & ENC_BYTE_MASK;
		i = i >> 7;
		if(i > 0)
			temp |= ENC_HIGH_BIT;
		SWFOutput_writeUInt8(out, temp);
	}
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
