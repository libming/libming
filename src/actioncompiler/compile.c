
#ifndef WIN32
  #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "compile.h"

int nConstants;
char *constants[256];

char *stringConcat(char *a, char *b)
{
  if(a)
  {
    a = realloc(a, strlen(a)+strlen(b)+1);
    strcat(a,b);
    free(b);
    return a;
  }
  else
    return b;
}

void bufferPatchLength(Buffer buffer, int back)
{
  unsigned char *output = buffer->buffer;
  int len = bufferLength(buffer);

  output[len-back-1] = (back>>8) & 0xff;
  output[len-back-2] = back & 0xff;
}

int addConstant(char *s)
{
  int i;

  for(i=0; i<nConstants; ++i)
  {
    if(strcmp(s, constants[i]) == 0)
      return i;
  }

  if(nConstants < 256)
  {
    constants[nConstants] = strdup(s);
    return nConstants++;
  }
  else
    return -1;
}

int bufferWriteConstants(Buffer out)
{
  int i, len=2;

  bufferWriteU8(out, SWFACTION_CONSTANTPOOL);
  bufferWriteS16(out, 0); /* length */
  bufferWriteS16(out, nConstants);

  for(i=0; i<nConstants; ++i)
    len += bufferWriteHardString(out, constants[i], strlen(constants[i])+1);

  bufferPatchLength(out, len);
  return len+3;
}

Buffer newBuffer()
{
  Buffer out = (Buffer)malloc(BUFFER_SIZE);
  memset(out, 0, BUFFER_SIZE);

  out->buffer = malloc(BUFFER_INCREMENT);
  out->pos = out->buffer;
  *(out->pos) = 0;
  out->buffersize = out->free = BUFFER_INCREMENT;

  return out;
}

void destroyBuffer(Buffer out)
{
  if(out)
  {
    free(out->buffer);
    free(out);
  }
}

int bufferLength(Buffer out)
{
  if(out)
    return (out->pos)-(out->buffer);
  else
    return 0;
}

/* make sure there's enough space for bytes bytes */
void bufferCheckSize(Buffer out, int bytes)
{
  if(bytes > out->free)
  {
    int new = BUFFER_INCREMENT * ((bytes-out->free-1)/BUFFER_INCREMENT + 1);

    int num = bufferLength(out); /* in case buffer gets displaced.. */
    unsigned char *newbuf = realloc(out->buffer, out->buffersize+new);

    if(newbuf != out->buffer)
      out->pos = newbuf+num;

    out->buffer = newbuf;
    out->buffersize += new;
    out->free += new;
  }
}

int bufferWriteData(Buffer b, byte *data, int length)
{
  int i;

  bufferCheckSize(b, length);

  for(i=0; i<length; ++i)
    bufferWriteU8(b, data[i]);

  return length;
}

int bufferWriteBuffer(Buffer a, Buffer b)
{
  if(!a)
    return 0;

  if(b)
    return bufferWriteData(a, b->buffer, bufferLength(b));

  return 0;
}

int bufferConcat(Buffer a, Buffer b)
{
  int len;

  if(!a)
    return 0;

  if(b)
    len = bufferWriteData(a, b->buffer, bufferLength(b));

  destroyBuffer(b);

  return len;
}

int bufferWriteU8(Buffer out, int data)
{
  bufferCheckSize(out, 1);
  *(out->pos) = data;
  out->pos++;
  out->free--;

  return 1;
}

int bufferWriteS16(Buffer out, int data)
{
  if(data < 0)
    data = (1<<16)+data;

  bufferWriteU8(out, data%256);
  data >>= 8;
  bufferWriteU8(out, data%256);

  return 2;
}

int bufferWriteHardString(Buffer out, byte *string, int length)
{
  int i;

  for(i=0; i<length; ++i)
    bufferWriteU8(out, string[i]);

  return length;
}

int bufferWriteConstantString(Buffer out, byte *string, int length)
{
  int n = addConstant(string);

  if(n == -1)
  {
    bufferWriteU8(out, PUSH_STRING);
    return bufferWriteHardString(out, string, length) + 1;
  }
  else
  {
    bufferWriteU8(out, PUSH_CONSTANT);
    return bufferWriteU8(out, n) + 1;
  }
}

int bufferWriteString(Buffer out, byte *string, int length)
{
  int l;

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 0);
  l = bufferWriteConstantString(out, string, length);
  bufferPatchLength(out, l);

  return 3 + l;
}

int bufferWriteInt(Buffer out, int i)
{
  unsigned char *p = (unsigned char *)&i;

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 5);
  bufferWriteU8(out, PUSH_INT);

  bufferWriteU8(out, p[0]);
  bufferWriteU8(out, p[1]);
  bufferWriteU8(out, p[2]);
  bufferWriteU8(out, p[3]);

  return 8;
}

int bufferWriteDouble(Buffer out, double d)
{
  unsigned char *p = (unsigned char *)&d;

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 9);
  bufferWriteU8(out, PUSH_DOUBLE);

  bufferWriteU8(out, p[4]);
  bufferWriteU8(out, p[5]);
  bufferWriteU8(out, p[6]);
  bufferWriteU8(out, p[7]);
  bufferWriteU8(out, p[0]);
  bufferWriteU8(out, p[1]);
  bufferWriteU8(out, p[2]);
  bufferWriteU8(out, p[3]);

  return 12;
}

int bufferWriteNull(Buffer out)
{
  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 1);
  bufferWriteU8(out, PUSH_NULL);
  return 4;
}

int bufferWriteBoolean(Buffer out, int val)
{
  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 2);
  bufferWriteU8(out, PUSH_BOOLEAN);
  bufferWriteU8(out, val ? 1 : 0);
  return 5;
}

int bufferWriteRegister(Buffer out, int num)
{
  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 2);
  bufferWriteU8(out, PUSH_REGISTER);
  bufferWriteU8(out, num);
  return 5;
}

int bufferWriteSetRegister(Buffer out, int num)
{
  bufferWriteU8(out, SWFACTION_SETREGISTER);
  bufferWriteS16(out, 1);
  bufferWriteU8(out, num);
  return 4;
}

void lower(char *s)
{
  while(*s)
  {
    *s = tolower(*s);
    ++s;
  }
}

/* replace MAGIC_CONTINUE_NUMBER and MAGIC_BREAK_NUMBER with jumps to
   head or tail, respectively */
/* jump offset is relative to end of jump instruction */
/* I can't believe this actually worked */

void bufferResolveJumps(Buffer out)
{
  byte *p = out->buffer;
  int l, target;

  while(p < out->pos)
  {
    if(*p & 0x80) /* then it's a multibyte instruction */
    {
      if(*p == SWFACTION_BRANCHALWAYS)
      {
	p += 3; /* plus instruction plus two-byte length */
	if(*p == MAGIC_CONTINUE_NUMBER_LO &&
	   *(p+1) == MAGIC_CONTINUE_NUMBER_HI)
	{
	  target = out->buffer - (p+2);
	  *p = target & 0xff;
	  *(p+1) = (target>>8) & 0xff;
	}
	else if(*p == MAGIC_BREAK_NUMBER_LO &&
		*(p+1) == MAGIC_BREAK_NUMBER_HI)
	{
	  target = out->pos - (p+2);
	  *p = target & 0xff;
	  *(p+1) = (target>>8) & 0xff;
	}

	p+=2;
      }
      else
      {
	++p;
	l = *p;
	++p;
	l += *p<<8;

	p += l;
      }
    }
    else
      ++p;
  }
}
