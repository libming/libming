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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/stat.h>

#ifndef WIN32
	#include <unistd.h>
#endif

#include "input.h"
#include "method.h"

struct SWFInput_s
{
	void (*destroy)(SWFInput this);
	int (*getChar)(SWFInput this);
	int (*read)(SWFInput this, unsigned char* buffer, int count);
	void (*seek)(SWFInput this, long offset, int whence);
	int (*eof)(SWFInput this);

	int offset;
	int length;
	void *data;
};


int
SWFInput_getChar(SWFInput input)
{
	return input->getChar(input);
}


int
SWFInput_getUInt16(SWFInput input)
{
	return SWFInput_getChar(input) + (SWFInput_getChar(input)<<8);
}


int
SWFInput_getUInt16_BE(SWFInput input)
{
	return (SWFInput_getChar(input)<<8) + SWFInput_getChar(input);
}


int
SWFInput_getSInt16(SWFInput input)
{
	return SWFInput_getChar(input) + SWFInput_getChar(input)*256;
}


unsigned long
SWFInput_getUInt32(SWFInput input)
{
	return (unsigned long)(SWFInput_getChar(input) +
			 (SWFInput_getChar(input)<<8) +
			 (SWFInput_getChar(input)<<16) +
			 (SWFInput_getChar(input)<<24));
}


unsigned long
SWFInput_getUInt32_BE(SWFInput input)
{
	return (unsigned long)((SWFInput_getChar(input)<<24) +
			 (SWFInput_getChar(input)<<16) +
			 (SWFInput_getChar(input)<<8) +
			 SWFInput_getChar(input));
}


int
SWFInput_read(SWFInput input, char* buffer, int count)
{
	return input->read(input, buffer, count);
}


void
SWFInput_seek(SWFInput input, long offset, int whence)
{
	input->seek(input, offset, whence);
}


int
SWFInput_length(SWFInput input)
{
	int pos = SWFInput_tell(input);

	SWFInput_seek(input, 0, SEEK_END);
	SWFInput_seek(input, pos, SEEK_SET);

	return input->length;
}


int
SWFInput_eof(SWFInput input)
{
	return input->eof(input);
}


int
SWFInput_tell(SWFInput input)
{
	return input->offset;
}


void
SWFInput_rewind(SWFInput input)
{
	SWFInput_seek(input, 0, SEEK_SET);
}


void
destroySWFInput(SWFInput input)
{
	input->destroy(input);
}


static void
SWFInput_dtor(SWFInput input)
{
	free(input);
}


/* SWFInput_file */

static void
SWFInput_file_seek(SWFInput input, long offset, int whence)
{
	if ( fseek((FILE *)input->data, offset, whence) == -1 )
	{
		if ( errno == EBADF )
			SWF_error("This is not a seekable stream- use newSWFInput_stream instead");
		else if ( errno == EINVAL )
			SWF_error("Invalid whence argument");
		else
			SWF_error("Unknown error");
	}

	if ( whence == SEEK_SET )
		input->offset = offset;

	else if ( whence == SEEK_END )
		input->offset = input->length - offset;

	else if ( whence == SEEK_CUR )
		input->offset += offset;
}


static int
SWFInput_file_eof(SWFInput input)
{
	return feof((FILE *)input->data);
}


static int
SWFInput_file_getChar(SWFInput input)
{
	int c = fgetc((FILE *)input->data);

	if ( c == EOF )
		input->length = input->offset;
	else
		++input->offset;

	return c;
}


SWFInput
newSWFInput_file(FILE *f)
{
	SWFInput input;
	struct stat buf;

	/* XXX - doesn't check for validity of f.. */

	if ( fseek(f, 0, SEEK_CUR) == -1 )
		return newSWFInput_stream(f);

	if ( fstat(fileno(f), &buf) == -1 )
		SWF_error("Couldn't fstat filehandle in newSWFInput_file");;

	input = malloc(sizeof(struct SWFInput_s));

	input->getChar = SWFInput_file_getChar;
	input->destroy = SWFInput_dtor;
	input->eof = SWFInput_file_eof;
	input->seek = SWFInput_file_seek;
	input->data = f;

	input->offset = 0;
	input->length = buf.st_size;

	return input;
}


/* SWFInput_buffer */

static int
SWFInput_buffer_getChar(SWFInput input)
{
	if ( input->offset >= input->length )
		return EOF;
	else
		return ((unsigned char *)input->data)[input->offset++];
}


static int
SWFInput_buffer_eof(SWFInput input)
{
	return input->offset >= input->length;
}


static int
SWFInput_buffer_read(SWFInput input, unsigned char* buffer, int count)
{
	if ( count > input->length - input->offset )
		count = input->length - input->offset;

	memcpy(buffer, input->data + input->offset, count);

	return count;
}


static void
SWFInput_buffer_seek(SWFInput input, long offset, int whence)
{
	if ( whence == SEEK_CUR )
	{
		if ( offset >= 0 )
			input->offset = min(input->length, input->offset + offset);
		else
			input->offset = max(0, input->offset + offset);
	}

	else if ( whence == SEEK_END )
		input->offset = max(0, input->length - offset);

	else if ( whence == SEEK_SET )
		input->offset = min(input->length, offset);
}


SWFInput
newSWFInput_buffer(unsigned char* buffer, int length)
{
	SWFInput input = malloc(sizeof(struct SWFInput_s));

	input->getChar = SWFInput_buffer_getChar;
	input->destroy = SWFInput_dtor;
	input->eof = SWFInput_buffer_eof;
	input->read = SWFInput_buffer_read;
	input->seek = SWFInput_buffer_seek;
	input->data = buffer;

	input->offset = 0;
	input->length = length;

	return input;
}


static void
SWFInput_buffer_dtor(SWFInput input)
{
	free(input->data);
	free(input);
}


/* same as above but needs to be freed */
SWFInput
newSWFInput_allocedBuffer(unsigned char *buffer, int length)
{
	SWFInput input = newSWFInput_buffer(buffer, length);
	input->destroy = SWFInput_buffer_dtor;
	return input;
}


/* SWFInput_stream */

#define INPUTSTREAM_INCREMENT 1024

struct SWFInputStreamData
{
	FILE *file;
	unsigned char *buffer;
};


static void
SWFInput_stream_seek(SWFInput input, long offset, int whence)
{
	int len;
	struct SWFInputStreamData *data;

	if ( whence == SEEK_CUR )
		input->offset = min(input->length, input->offset + offset);

	else if ( whence == SEEK_SET )
		input->offset = offset;

	else if ( whence == SEEK_END )
	{
		/* suck data until EOF */
		/* XXX - might want to put a limit on how much we suck (ha) */

		while ( SWFInput_getChar(input) != EOF )
			;

		input->offset = input->length - offset;
	}

	if ( input->offset < input->length )
		return;

	/* now slurp up as much data as we need to get here */

	len = ((input->offset/INPUTSTREAM_INCREMENT)+1) * INPUTSTREAM_INCREMENT;

	data = input->data;

	data->buffer = realloc(data->buffer, sizeof(unsigned char) * len);

	while(len > 0)
		len -= fread(data->buffer, sizeof(unsigned char), len, data->file);
}


static int
SWFInput_stream_getChar(SWFInput input)
{
	struct SWFInputStreamData *data = input->data;

	if ( input->offset == input->length )
	{
		/* fetch from stream, save in buffer */

		FILE *f = data->file;
		int c = fgetc(f);

		++input->offset;

		if ( c != EOF )
		{
			if ( input->length % INPUTSTREAM_INCREMENT == 0 )
			{
				data->buffer = realloc(data->buffer,
															 sizeof(unsigned char) *
															 (input->length + INPUTSTREAM_INCREMENT));
			}

			data->buffer[input->length] = c;
			++input->length;
		}

		return c;
	}
	else if ( input->offset < input->length )
	{
		/* fetch from buffer */
		return data->buffer[input->offset++];
	}
	else
		return EOF;
}


static int
SWFInput_stream_read(SWFInput input, unsigned char* buffer, int count)
{
	struct SWFInputStreamData *data = input->data;
	int need = input->offset + count - input->length;

	if ( need > 0 )
	{
		int num;

		data->buffer =
			realloc(data->buffer,
							sizeof(unsigned char) * INPUTSTREAM_INCREMENT *
							(((input->offset + count) / INPUTSTREAM_INCREMENT) + 1));
		
		num = fread(data->buffer + input->length,
								sizeof(unsigned char), need, data->file);

		input->length += num;
	}

	if ( count > input->length - input->offset )
		count = input->length - input->offset;

	memcpy(buffer, data->buffer + input->offset, count);

	return count;
}


static void
SWFInput_stream_dtor(SWFInput input)
{
	free(input->data);
}


SWFInput
newSWFInput_stream(FILE* f)
{
	SWFInput input = malloc(sizeof(struct SWFInput_s));

	struct SWFInputStreamData *data = malloc(sizeof(struct SWFInputStreamData));

	input->getChar = SWFInput_stream_getChar;
	input->destroy = SWFInput_stream_dtor;
	input->eof = SWFInput_file_eof;
	input->read = SWFInput_stream_read;
	input->seek = SWFInput_stream_seek;
	input->data = (void *)f;

	input->offset = 0;
	input->length = 0;

	data->file = f;
	data->buffer = NULL;

	input->data = (void *)data;

	return input;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
