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
#include <math.h>

#include "block.h"
#include "soundstream.h"
#include "output.h"
#include "outputblock.h"
#include "sound.h"
#include "method.h"
#include "input.h"
#include "libming.h"


struct SWFSoundStream_s
{
	byte flags;
	byte isFinished;
	int delay;
	int start;
	int samplesPerFrame;
	int sampleRate;
	byte freeInput;
	SWFInput input;
};

struct SWFSoundStreamBlock_s
{
	struct SWFBlock_s block;

	SWFSoundStream stream;
	int numFrames;
	int delay;
	int length;
};


int
nextMP3Frame(SWFInput input);

void skipMP3(SWFSoundStream stream, float skip) 
{
	int frameSize;
	int skipFrames, l;
	
	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
	        frameSize = 576;
		 
	skipFrames = (int)floor((skip  / frameSize) / stream->sampleRate);

	if(skipFrames <= 0)
		return;

	while(skipFrames > 0) {
		l = nextMP3Frame(stream->input);
		if (l < 0) {
			printf("no more frames to skip \n");
			break;
		}
		--skipFrames;
		stream->start += l;
	}
}

int
completeSWFSoundStream(SWFBlock block)
{
	return ((SWFSoundStreamBlock)block)->length + 4;
}


void
writeSWFSoundStreamToMethod(SWFBlock block,
														SWFByteOutputMethod method, void *data)
{
	SWFSoundStreamBlock streamblock = (SWFSoundStreamBlock)block;
	SWFInput input = streamblock->stream->input;
	int l = streamblock->length;

	methodWriteUInt16(streamblock->numFrames *
				(streamblock->stream->sampleRate > 32000 ? 1152 : 576),
				method, data);

	methodWriteUInt16(streamblock->delay, method, data);

	for ( ; l>0; --l )
		method((unsigned char)SWFInput_getChar(input), data);
}


SWFBlock
SWFSoundStream_getStreamBlock(SWFSoundStream stream)
{
	int delay, length;
	SWFSoundStreamBlock block;
	int frameSize;

	if ( stream->isFinished )
		return NULL;

	block = (SWFSoundStreamBlock) malloc(sizeof(struct SWFSoundStreamBlock_s));

	SWFBlockInit((SWFBlock)block);

	BLOCK(block)->complete = completeSWFSoundStream;
	BLOCK(block)->writeBlock = writeSWFSoundStreamToMethod;
	BLOCK(block)->dtor = NULL;
	BLOCK(block)->type = SWF_SOUNDSTREAMBLOCK;

	block->stream = stream;
	block->length = 0;

	/* see how many frames we can put in this block,
		 see how big they are */

	block->delay = stream->delay;

	delay = stream->delay + stream->samplesPerFrame;

	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;

	while ( delay > frameSize )
	{
		++block->numFrames;
		length = nextMP3Frame(stream->input);

		if ( length <= 0 )
		{
			stream->isFinished = TRUE;
			SWFSoundStream_rewind(stream);
			break;
		}

		block->length += length;
		delay -= frameSize;
	}

	stream->delay = delay;

	return (SWFBlock)block;
}

int SWFSoundStream_getFrames(SWFSoundStream stream)
{	int n, frameSize;
	
	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;
	n = 0;
	while(nextMP3Frame(stream->input) > 0)
		n++;
	SWFSoundStream_rewind(stream);
	return n * frameSize / stream->samplesPerFrame;
}
	
#define MP3_FRAME_SYNC			 0xFFE00000

#define MP3_VERSION					 0x00180000
#define MP3_VERSION_25			 0x00000000
#define MP3_VERSION_RESERVED 0x00080000
#define MP3_VERSION_2				 0x00100000
#define MP3_VERSION_1				 0x00180000

#define MP3_SAMPLERATE			 0x00000C00
#define MP3_SAMPLERATE_SHIFT 10

#define MP3_CHANNEL					 0x000000C0
#define MP3_CHANNEL_STEREO	 0x00000000
#define MP3_CHANNEL_JOINT		 0x00000040
#define MP3_CHANNEL_DUAL		 0x00000080
#define MP3_CHANNEL_MONO		 0x000000C0

SWFBlock
SWFSoundStream_getStreamHead(SWFSoundStream stream, float frameRate, float skip)
{
	SWFOutput out = newSizedSWFOutput(6);
	SWFOutputBlock block = newSWFOutputBlock(out, SWF_SOUNDSTREAMHEAD);
	SWFInput input = stream->input;
	
	int rate, channels, flags, start = 0;

	/* get 4-byte header, bigendian */
	flags = SWFInput_getChar(input);

	if ( flags == EOF )
		return NULL;

	/* XXX - fix this mad hackery */

	if ( flags == 'I' &&
			 SWFInput_getChar(input) == 'D' &&
			 SWFInput_getChar(input) == '3' )
	{
		start = 2;

		do
		{
			++start;
			flags = SWFInput_getChar(input);
		}
		while(flags != 0xFF && flags != EOF);
	}

	if ( flags == EOF )
		return NULL;

	SWFInput_seek(input, -1, SEEK_CUR);
	flags = SWFInput_getUInt32_BE(input);

	SWFInput_seek(input, start, SEEK_SET);

	stream->start = start;

	if ( (flags & MP3_FRAME_SYNC) != MP3_FRAME_SYNC )
		return NULL;

	if ( (flags & MP3_CHANNEL) == MP3_CHANNEL_MONO )
		channels = SWF_SOUNDSTREAM_MONO;
	else
		channels = SWF_SOUNDSTREAM_STEREO;

	/* XXX - this is a gross oversimplification */
	switch ( flags & MP3_VERSION )
	{
		case MP3_VERSION_1:
			stream->sampleRate = 44100; rate = SWF_SOUNDSTREAM_44KHZ; break;

		case MP3_VERSION_2:
			stream->sampleRate = 22050; rate = SWF_SOUNDSTREAM_22KHZ; break;

		case MP3_VERSION_25:
			stream->sampleRate = 11025; rate = SWF_SOUNDSTREAM_11KHZ; break;
	}

	flags =
		SWF_SOUNDSTREAM_MP3_COMPRESSED | rate | SWF_SOUNDSTREAM_16BITS | channels;

	stream->flags = flags;
	
	stream->samplesPerFrame = (int)floor(stream->sampleRate / frameRate);
	
	skipMP3(stream, skip);
	
	SWFOutput_writeUInt8(out, flags & 0x0f); /* preferred mix format.. (?) */
	SWFOutput_writeUInt8(out, flags);
	SWFOutput_writeUInt16(out, stream->samplesPerFrame);
	SWFOutput_writeUInt16(out, SWFSOUND_INITIAL_DELAY);

	return (SWFBlock)block;
}


/* XXX - kill this somehow.. */
void
SWFSoundStream_rewind(SWFSoundStream stream)
{
	SWFInput_seek(stream->input, stream->start, SEEK_SET);
}


SWFSoundStream
newSWFSoundStream_fromInput(SWFInput input)
{
	SWFSoundStream stream = (SWFSoundStream)malloc(sizeof(struct SWFSoundStream_s));

	/* XXX - destructor? */

	stream->input = input;
	stream->delay = SWFSOUND_INITIAL_DELAY;
	stream->flags = 0;
	stream->isFinished = FALSE;
	stream->start = 0;
	stream->samplesPerFrame = 0;
	stream->sampleRate = 0;
	stream->freeInput = FALSE;

	return stream;
}


void
destroySWFSoundStream(SWFSoundStream stream)
{
	if ( stream->freeInput )
		destroySWFInput(stream->input);

	free(stream);
}


SWFSoundStream
newSWFSoundStream(FILE* file)
{
	SWFSoundStream s = newSWFSoundStream_fromInput(newSWFInput_file(file));
	s->freeInput = TRUE;
	return s;
}

SWFSoundStream
newSWFSoundStreamFromFileno(int fd)
{
  FILE *fp = fdopen(fd, "r");
  return newSWFSoundStream(fp);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
