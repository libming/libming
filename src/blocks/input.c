
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/stat.h>

#ifndef WIN32
  #include <unistd.h>
#endif

#include "input.h"

int SWFInput_getChar(SWFInput input)
{
  return input->getChar(input);
}

int SWFInput_getUInt16(SWFInput input)
{
  return SWFInput_getChar(input) + (SWFInput_getChar(input)<<8);
}

int SWFInput_getUInt16_BE(SWFInput input)
{
  return (SWFInput_getChar(input)<<8) + SWFInput_getChar(input);
}

int SWFInput_getSInt16(SWFInput input)
{
  return SWFInput_getChar(input) + SWFInput_getChar(input)*256;
}

unsigned long SWFInput_getUInt32(SWFInput input)
{
  return (unsigned long)(SWFInput_getChar(input) +
			 (SWFInput_getChar(input)<<8) +
			 (SWFInput_getChar(input)<<16) +
			 (SWFInput_getChar(input)<<24));
}

unsigned long SWFInput_getUInt32_BE(SWFInput input)
{
  return (unsigned long)((SWFInput_getChar(input)<<24) +
			 (SWFInput_getChar(input)<<16) +
			 (SWFInput_getChar(input)<<8) +
			 SWFInput_getChar(input));
}

void SWFInput_seek(SWFInput input, long offset, int whence)
{
  input->seek(input, offset, whence);
}

int SWFInput_length(SWFInput input)
{
  int pos = SWFInput_tell(input);
  SWFInput_seek(input, 0, SEEK_END);
  SWFInput_seek(input, pos, SEEK_SET);
  return input->length;
}

int SWFInput_eof(SWFInput input)
{
  return input->eof(input);
}

int SWFInput_tell(SWFInput input)
{
  return input->offset;
}

void SWFInput_rewind(SWFInput input)
{
  SWFInput_seek(input, 0, SEEK_SET);
}

void destroySWFInput(SWFInput input)
{
  input->destroy(input);
}

void SWFInput_dtor(SWFInput input)
{
  free(input);
}


/* SWFInput_file */

void SWFInput_file_seek(SWFInput input, long offset, int whence)
{
  if(fseek((FILE *)input->data, offset, whence) == -1)
  {
    if(errno == EBADF)
      SWF_error("This is not a seekable stream- use newSWFInput_stream instead");
    else if(errno == EINVAL)
      SWF_error("Invalid whence argument");
    else
      SWF_error("Unknown error");
  }

  if(whence == SEEK_SET)
    input->offset = offset;

  else if(whence == SEEK_END)
    input->offset = input->length - offset;

  else if(whence == SEEK_CUR)
    input->offset += offset;
}

int SWFInput_file_eof(SWFInput input)
{
  return feof((FILE *)input->data);
}

int SWFInput_file_getChar(SWFInput input)
{
  int c = fgetc((FILE *)input->data);

  if(c == EOF)
    input->length = input->offset;
  else
    ++input->offset;

  return c;
}

SWFInput newSWFInput_file(FILE *f)
{
  SWFInput input;
  struct stat buf;

  /* XXX - doesn't check for validity of f.. */

  if(fseek(f, 0, SEEK_CUR) == -1)
    return newSWFInput_stream(f);

  if(fstat(fileno(f), &buf) == -1)
    SWF_error("Couldn't fstat filehandle in newSWFInput_file");;

  input = calloc(1, sizeof(struct _swfInput));

  input->getChar = SWFInput_file_getChar;
  input->destroy = SWFInput_dtor;
  input->eof = SWFInput_file_eof;
  input->seek = SWFInput_file_seek;
  input->data = (void *)f;

  input->offset = 0;
  input->length = buf.st_size;

  return input;
}


/* SWFInput_buffer */

int SWFInput_buffer_getChar(SWFInput input)
{
  if(input->offset >= input->length)
    return EOF;
  else
    return ((unsigned char *)input->data)[input->offset++];
}

int SWFInput_buffer_eof(SWFInput input)
{
  return input->offset >= input->length;
}

void SWFInput_buffer_seek(SWFInput input, long offset, int whence)
{
  if(whence == SEEK_CUR)
  {
    if(offset >= 0)
      input->offset = min(input->length, input->offset + offset);
    else
      input->offset = max(0, input->offset + offset);
  }

  else if(whence == SEEK_END)
    input->offset = max(0, input->length - offset);

  else if(whence == SEEK_SET)
    input->offset = min(input->length, offset);
}

SWFInput newSWFInput_buffer(unsigned char *buffer, int length)
{
  SWFInput input = calloc(1, sizeof(struct _swfInput));

  input->getChar = SWFInput_buffer_getChar;
  input->destroy = SWFInput_dtor;
  input->eof = SWFInput_buffer_eof;
  input->seek = SWFInput_buffer_seek;
  input->data = (void *)buffer;

  input->offset = 0;
  input->length = length;

  return input;
}

void SWFInput_buffer_dtor(SWFInput input)
{
  free(input->data);
  free(input);
}

/* same as above but needs to be freed */
SWFInput newSWFInput_allocedBuffer(unsigned char *buffer, int length)
{
  SWFInput input = newSWFInput_buffer(buffer, length);
  input->destroy = SWFInput_buffer_dtor;
  return input;
}


/* SWFInput_stream */

#define INPUTSTREAM_INCREMENT 1024

struct _inputStreamData
{
  FILE *file;
  unsigned char *buffer;
};

void SWFInput_stream_seek(SWFInput input, long offset, int whence)
{
  int len;
  unsigned char *buffer;

  if(whence == SEEK_CUR)
    input->offset = min(input->length, input->offset + offset);

  else if(whence == SEEK_SET)
    input->offset = offset;

  else if(whence == SEEK_END)
  {
    /* suck data until EOF */
    /* XXX - might want to put a limit on how much we suck */

    while(SWFInput_getChar(input) != EOF)
      ;

    input->offset = input->length - offset;
  }

  if(input->offset < input->length)
    return;

  /* now slurp up as much data as we need to get here */

  len = ((input->offset/INPUTSTREAM_INCREMENT)+1) * INPUTSTREAM_INCREMENT;

  buffer = ((struct _inputStreamData *)input->data)->buffer =
    realloc(((struct _inputStreamData *)input->data)->buffer,
	    sizeof(unsigned char) * len);

  while(len > 0)
  {
    len -= fread(buffer, sizeof(unsigned char), len,
		 ((struct _inputStreamData *)input->data)->file);
  }
}

int SWFInput_stream_getChar(SWFInput input)
{
  if(input->offset == input->length)
  {
    /* fetch from stream, save in buffer */

    FILE *f = ((struct _inputStreamData *)input->data)->file;
    int c = fgetc(f);

    ++input->offset;

    if(c != EOF)
    {
      unsigned char *buffer = ((struct _inputStreamData *)input->data)->buffer;

      if(input->length % INPUTSTREAM_INCREMENT == 0)
      {
	buffer = ((struct _inputStreamData *)input->data)->buffer =
	  realloc(buffer, sizeof(unsigned char) *
		  (input->length + INPUTSTREAM_INCREMENT));
      }

      buffer[input->length] = c;
      ++input->length;
    }

    return c;
  }
  else if(input->offset < input->length)
  {
    /* fetch from buffer */
    return ((struct _inputStreamData *)input->data)->buffer[input->offset++];
  }
  else
    return EOF;
}

void SWFInput_stream_dtor(SWFInput input)
{
  free(input->data);
}

SWFInput newSWFInput_stream(FILE *f)
{
  SWFInput input = calloc(1, sizeof(struct _swfInput));
  struct _inputStreamData *data = calloc(1, sizeof(struct _inputStreamData));

  input->getChar = SWFInput_stream_getChar;
  input->destroy = SWFInput_stream_dtor;
  input->eof = SWFInput_file_eof;
  input->seek = SWFInput_stream_seek;
  input->data = (void *)f;

  input->offset = 0;
  input->length = 0;

  data->file = f;
  data->buffer = NULL;

  input->data = (void *)data;

  return input;
}
