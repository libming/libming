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

#include <stdlib.h>
#include <stdio.h>

#include "output.h"

SWFOutput newSWFOutput()
{
  SWFOutput out = calloc(1, OUTPUT_SIZE);

  out->buffer = malloc(OUTPUT_BUFFER_INCREMENT);
  out->pos = out->buffer;
  *(out->pos) = 0;
  out->buffersize = out->free = OUTPUT_BUFFER_INCREMENT;
  out->bitpos = 0;

  return out;
}

/* same as above but with specified buffer size,
   use if you have an upper limit to how big this'll get */
SWFOutput newSizedSWFOutput(int size)
{
  SWFOutput out = calloc(1, OUTPUT_SIZE);

  out->buffer = malloc(size+1);
  out->pos = out->buffer;
  *(out->pos) = 0;
  out->buffersize = out->free = size+1;
  out->bitpos = 0;

  return out;
}

void SWFOutput_writeToMethod(SWFOutput out,
			     SWFByteOutputMethod method, void *data)
{
  int i, l;
  SWFOutput o = out;
  byte *buffer;

  SWFOutput_byteAlign(out);

  while(o != NULL)
  {
    buffer = o->buffer;
    l = o->pos - buffer;

    for(i=0; i<l; ++i)
      method(buffer[i], data);

    o = o->next;
  }
}

void destroySWFOutput(SWFOutput out)
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

void SWFOutput_grow(SWFOutput out)
{
  int num = out->pos - out->buffer; /* in case buffer gets displaced.. */
  unsigned char *newbuf = realloc(out->buffer, out->buffersize+OUTPUT_BUFFER_INCREMENT);

  if(newbuf != out->buffer)
    out->pos = newbuf+num;

  out->buffer = newbuf;
  out->buffersize += OUTPUT_BUFFER_INCREMENT;
  out->free += OUTPUT_BUFFER_INCREMENT;
}

int SWFOutput_length(SWFOutput out)
{
  int size = 0;

  while(out!=NULL)
  {
    size += (out->pos)-(out->buffer);
    out = out->next;
  }

  return size;
}

/* make sure there's enough space for bytes bytes */
void SWFOutput_checkSize(SWFOutput out, int bytes)
{
  if(bytes > out->free)
  {
    int new = OUTPUT_BUFFER_INCREMENT *
              ((bytes-out->free-1)/OUTPUT_BUFFER_INCREMENT + 1);

    int num = SWFOutput_length(out); /* in case buffer gets displaced.. */
    unsigned char *newbuf = realloc(out->buffer, out->buffersize+new);

    if(newbuf != out->buffer)
      out->pos = newbuf+num;

    out->buffer = newbuf;
    out->buffersize += new;
    out->free += new;
  }
}

void SWFOutput_byteAlign(SWFOutput out)
{
  if(out->bitpos > 0)
  {
    SWFOutput_checkSize(out, 1);
    out->pos++;
    out->free--;
    out->bitpos = 0;
  }
}
void SWFOutput_writeBits(SWFOutput out, int data, int bits)
{
  int bitpos = out->bitpos;

  if(bitpos==0)
    *(out->pos) = 0;

  SWFOutput_checkSize(out, (bits+bitpos)/8);
  while(bits > 0)
  {
    if(bits+bitpos >= 8)
    {
      *(out->pos) += (data>>(bits+bitpos-8)) & 0xff;
      bits -= 8-bitpos;
      out->pos++;
      *(out->pos) = 0;
      out->free--;
      bitpos = 0;
    }
    else
    {
      *(out->pos) += (data<<(8-bits-bitpos))&0xff;
      bitpos += bits;
      bits = 0;
    }
  }

  out->bitpos = bitpos;
}

void SWFOutput_writeSBits(SWFOutput out, int data, int bits)
{
  if(data < 0)
    data = (1<<bits)+data;

  SWFOutput_writeBits(out, data, bits);
}
void SWFOutput_writeUInt8(SWFOutput out, int data)
{
  /* think we have to byte align when we write whole bytes */
  SWFOutput_byteAlign(out);

  SWFOutput_checkSize(out, 1);
  *(out->pos) = data;
  out->pos++;
  out->free--;
}
void SWFOutput_writeSInt8(SWFOutput out, int data)
{
  if(data < 0)
    data = (1<<8)+data;

  SWFOutput_writeUInt8(out, data);
}
void SWFOutput_writeUInt16(SWFOutput out, int data)
{
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
}
void SWFOutput_writeSInt16(SWFOutput out, int data)
{
  if(data < 0)
    data = (1<<16)+data;

  SWFOutput_writeUInt8(out, data%256);
  data >>= 8;
  SWFOutput_writeUInt8(out, data%256);
}
void SWFOutput_writeUInt32(SWFOutput out, long data)
{
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
}
void SWFOutput_writeSInt32(SWFOutput out, long data)
{
  if(data < 0)
    data = (0xffffffff + data) + 1;

  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
  data >>= 8;
  SWFOutput_writeUInt8(out, data&0xff);
}

void SWFOutput_writeBuffer(SWFOutput out, char *buffer, int bytes)
{
  SWFOutput_checkSize(out, bytes);
  memcpy(out->pos, buffer, bytes);
  out->pos += bytes;
  out->free -= bytes;
}

int SWFOutput_numBits(int num)
{
  int i=0;

  while(num>0)
  {
      num >>= 1;
      ++i;
  }

  return i;
}

int SWFOutput_numSBits(int num)
{
  if(num<0)
    return SWFOutput_numBits(-num)+1;   /* XXX one more than necessary if num = -2^n */
  else
    return SWFOutput_numBits(num)+1;
}

void SWFOutput_writeString(SWFOutput out, unsigned char *string)
{
  char c;

  SWFOutput_byteAlign(out);

  if(string)
  {
    while((c=*(string++)) != 0)
      SWFOutput_writeUInt8(out, c);
  }

  SWFOutput_writeUInt8(out, 0);
}
