
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "compile.h"

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
Buffer bufferCheckSize(Buffer out, int bytes)
{
  if(!out)
    out = newBuffer();

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

  return out;
}

void bufferWriteData(Buffer b, byte *data, int length)
{
  int i;

  b = bufferCheckSize(b, length);

  for(i=0; i<length; ++i)
    bufferWriteU8(b, data[i]);
}

void bufferWriteBuffer(Buffer a, Buffer b)
{
  if(!a)
    return;

  if(b)
    bufferWriteData(a, b->buffer, bufferLength(b));
}
void bufferConcat(Buffer a, Buffer b)
{
  if(!a)
    return;

  if(b)
    bufferWriteData(a, b->buffer, bufferLength(b));

  destroyBuffer(b);
}

/*
void bufferWriteToMethod(Buffer out, SWFByteOutputMethod method, void *data)
{
  int i, length = Buffer_length(out);

  for(i=0; i<length; ++i)
    method(out->buffer[i], data);
}
*/

void bufferWriteU8(Buffer out, int data)
{
  out = bufferCheckSize(out, 1);
  *(out->pos) = data;
  out->pos++;
  out->free--;
}
void bufferWriteS16(Buffer out, int data)
{
  if(data < 0)
    data = (1<<16)+data;

  bufferWriteU8(out, data%256);
  data >>= 8;
  bufferWriteU8(out, data%256);
}

void bufferWriteHardString(Buffer out, byte *string, int length)
{
  int i;

  for(i=0; i<length; ++i)
    bufferWriteU8(out, string[i]);
}

void bufferWriteString(Buffer out, byte *string, int length)
{
  int i;

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, length+1);
  bufferWriteU8(out, SWFACTION_PUSHDATA_STRING);

  for(i=0; i<length; ++i)
    bufferWriteU8(out, string[i]);
}

void lower(char *s)
{
  while(*s)
  {
    *s = tolower(*s);
    ++s;
  }
}

int lookupSetProperty(char *string)
{
  lower(string);

  if(strcmp(string,"x")==0)		return 0x0000;
  if(strcmp(string,"y")==0)		return 0x3f80;
  if(strcmp(string,"xscale")==0)	return 0x4000;
  if(strcmp(string,"yscale")==0)	return 0x4040;
  if(strcmp(string,"alpha")==0)		return 0x40c0;
  if(strcmp(string,"visible")==0)	return 0x40e0;
  if(strcmp(string,"rotation")==0)	return 0x4120;
  if(strcmp(string,"name")==0)		return 0x4140;
  if(strcmp(string,"quality")==0)	return 0x4180;
  if(strcmp(string,"focusrect")==0)	return 0x4188;
  if(strcmp(string,"soundbuftime")==0)	return 0x4190;

  fprintf(stderr, "No such property: %s\n", string);
  return -1;
}

void bufferWriteSetProperty(Buffer out, char *string)
{
  int property = lookupSetProperty(string);

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 5);
  bufferWriteU8(out, SWFACTION_PUSHDATA_PROPERTY);
  bufferWriteS16(out, 0);
  bufferWriteS16(out, property);
}

void bufferWriteWTHITProperty(Buffer out)
{
  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, 5);
  bufferWriteU8(out, SWFACTION_PUSHDATA_PROPERTY);
  bufferWriteS16(out, 0);
  bufferWriteS16(out, SWF_SETPROPERTY_WTHIT);
}

char *lookupGetProperty(char *string)
{
  lower(string);

  if(strcmp(string,"x")==0)		return "0";
  if(strcmp(string,"y")==0)		return "1";
  if(strcmp(string,"xscale")==0)	return "2";
  if(strcmp(string,"yscale")==0)	return "3";
  if(strcmp(string,"currentframe")==0)	return "4";
  if(strcmp(string,"totalframes")==0)	return "5";
  if(strcmp(string,"alpha")==0)		return "6";
  if(strcmp(string,"visible")==0)	return "7";
  if(strcmp(string,"width")==0)		return "8";
  if(strcmp(string,"height")==0)	return "9";
  if(strcmp(string,"rotation")==0)	return "10";
  if(strcmp(string,"target")==0)	return "11";
  if(strcmp(string,"framesloaded")==0)	return "12";
  if(strcmp(string,"name")==0)		return "13";
  if(strcmp(string,"droptarget")==0)	return "14";
  if(strcmp(string,"url")==0)		return "15";
  if(strcmp(string,"quality")==0)	return "16";
  if(strcmp(string,"focusrect")==0)	return "17";
  if(strcmp(string,"soundbuftime")==0)	return "18";

  fprintf(stderr, "No such property: %s\n", string);
  return "";
}

void bufferWriteGetProperty(Buffer out, char *string)
{
  char *property = lookupGetProperty(string);

  bufferWriteU8(out, SWFACTION_PUSHDATA);
  bufferWriteS16(out, strlen(property)+2);
  bufferWriteU8(out, SWFACTION_PUSHDATA_STRING);
  bufferWriteData(out, property, strlen(property)+1);
}

void bufferDup(Buffer out)
{
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_SETVARIABLE);
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_GETVARIABLE);
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_GETVARIABLE);
}

void bufferTrip(Buffer out)
{
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_SETVARIABLE);
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_GETVARIABLE);
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_GETVARIABLE);
  bufferWriteString(out, "_tmp", 5);
  bufferWriteU8(out, SWFACTION_GETVARIABLE);
}

/* replace MAGIC_CONTINUE_NUMBER and MAGIC_BREAK_NUMBER with jumps to
   head or tail, respectively */
/* jump offset is relative to end of jump instruction */
/* I can't believe this actually worked */
void resolveJumps(Buffer out)
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
